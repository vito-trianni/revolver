#include "parallel_evolution.h"

const string CONFIGURATION_EVOLUTION_NUMPROCESSES    = "num_processes";
const string CONFIGURATION_EVOLUTION_INVALID_NAME    = "invalid_name";

/****************************************/
/****************************************/

CParallelEvolution::CParallelEvolution() :
   CEvolution(),
   m_unNumProcesses(),
   m_unMsgTag(),
   m_sInvalidName("build/revolver/invalid"),
   m_punEvaluationSeeds(NULL)
{}


/****************************************/
/****************************************/

CParallelEvolution::~CParallelEvolution() {
   if( m_punEvaluationSeeds != NULL ) delete [] m_punEvaluationSeeds;
}



void CParallelEvolution::Init( TConfigurationNode& t_configuration_tree ) {
   CEvolution::Init(t_configuration_tree);

   m_punEvaluationSeeds = new UInt32[m_pcEvaluationStrategy->GetNumSamples()];


   ////////////////////////////////////////////////////////////////////////////////
   // number of parallel processes
   ////////////////////////////////////////////////////////////////////////////////
   GetNodeAttributeOrDefault(t_configuration_tree, CONFIGURATION_EVOLUTION_NUMPROCESSES, m_unNumProcesses, m_unNumProcesses );
   
   ////////////////////////////////////////////////////////////////////////////////
   // name of "invalid simulator"
   ////////////////////////////////////////////////////////////////////////////////
   GetNodeAttributeOrDefault(t_configuration_tree, CONFIGURATION_EVOLUTION_INVALID_NAME, m_sInvalidName, m_sInvalidName );
   
}


/****************************************/
/****************************************/

void CParallelEvolution::StartEvaluationProcess() {
   ////////////////////////////////////////////////////////////////////////////////
   // Start MPI environment
   ////////////////////////////////////////////////////////////////////////////////
   try {
      MPI::Init();
   }
   catch(CARGoSException& ex) {
      SInt32 errorcode = 0;
      MPI::COMM_WORLD.Abort(errorcode);
      THROW_ARGOSEXCEPTION_NESTED("Initialization error of MPI parallel environment", ex);
   }


   ////////////////////////////////////////////////////////////////////////////////
   // spawn child processes that will evaluate the indivudals of the population
   ////////////////////////////////////////////////////////////////////////////////
   // build arguments vector
   char** c_args = new char*[6];
   c_args[0] = strdup(m_sInvalidName.c_str());
   c_args[1] = strdup("-c");
   c_args[2] = strdup((m_sWorkingDir+"/"+m_sExperimentConfigurationFile).c_str());
   c_args[3] = strdup( "-d" );;
   c_args[4] = strdup(m_sWorkingDir.c_str());;
   c_args[5] = NULL;

   // try to spawn processes
   try {
      m_cEvaluatorComm = MPI::COMM_WORLD.Spawn(c_args[0], (const char**)(c_args+1), m_unNumProcesses, MPI::Info(), 0);
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("Could not spawn process all requested processes", ex);
   }

   LOG << "[REVOLVER] spawned " << m_unNumProcesses << " MPI processs" << endl;
   LOG.Flush();

   // Porcata step 2: make sure evaluation strategy gets the needed info from population
   m_pcEvaluationStrategy->GetInfoFromPopulation(m_pcPopulation);

   // initialise the spawned process
   UInt32 unGenotypeSize = m_pcPopulation->GetGenotypeSize();
   UInt32 unNumTeams = m_pcEvaluationStrategy->GetNumTeams();
   UInt32 unTeamSize = m_pcEvaluationStrategy->GetTeamSize();
   UInt32 unNumSamples = m_pcEvaluationStrategy->GetNumSamples();
   UInt32 unNumObjectives = m_pcPopulation->GetNumObjectives();
   Real fRecombinationFactor = m_pcPopulation->GetRecombinationFactor();

   for( UInt32 tid = 0; tid < m_unNumProcesses; ++tid ) {
      LOG << "[REVOLVER] sending initialisation parameters to child process (tid = " << tid << ")" << endl;
      LOG.Flush();

      m_cEvaluatorComm.Send(&unGenotypeSize, 1, MPI_INT, tid, 1);
      m_cEvaluatorComm.Send(&unNumTeams, 1, MPI_INT, tid, 1);
      m_cEvaluatorComm.Send(&unTeamSize, 1, MPI_INT, tid, 1);
      m_cEvaluatorComm.Send(&unNumSamples, 1, MPI_INT, tid, 1);
      m_cEvaluatorComm.Send(&unNumObjectives, 1, MPI_INT, tid, 1);
      m_cEvaluatorComm.Send(&fRecombinationFactor, 1, MPI_ARGOSREAL, tid, 1);
      

      // initialise the map that associate to each spawned process the individual that it is evaluating
      m_mapProcessToIndividual[tid] = -1;
   }

   // free allocated memory
   delete [] c_args[0];
   delete [] c_args[1];
   delete [] c_args[2];
   delete [] c_args[3];
   delete [] c_args[4];
   delete [] c_args;
}


/****************************************/
/****************************************/

void CParallelEvolution::StopEvaluationProcess() {
   ////////////////////////////////////////////////////////////////////////////////
   // send a stop signal to all the spawned processes
   ////////////////////////////////////////////////////////////////////////////////
   SInt32 stop = -1;
   for( TMapProcessToIndividualIterator it = m_mapProcessToIndividual.begin(); it != m_mapProcessToIndividual.end(); it++ ) {
      m_cEvaluatorComm.Send(&stop, 1, MPI_INT, it->first, 1);
   }

   ////////////////////////////////////////////////////////////////////////////////
   // Terminate parallel environment
   ////////////////////////////////////////////////////////////////////////////////
   MPI::Finalize();
}



/****************************************/
/****************************************/

void CParallelEvolution::EvaluatePopulation() {
   // generate the seeds for the population evaluation
   for( UInt32 i = 0; i < m_pcEvaluationStrategy->GetNumSamples(); ++i ) {
      m_punEvaluationSeeds[i] = m_pcRNG->Uniform(CRange<UInt32>(0,INT_MAX));
   }
   
   // fist, send numProcess individuals in the queue. The queueing script
   // will write in a file the results, a file different for each
   // individual sample. 
   UInt32 queued    = 0;
   UInt32 ind       = 0;
   UInt32 remaining = m_pcPopulation->GetSize();
   while( remaining > 0 ) {    
      // every time there are less queued evaluations than the maximum
      // number, stated in numProcesses, start another one
      while( (queued < m_unNumProcesses) && (ind < m_pcPopulation->GetSize()) ) {
         SendIndividualParameters( ind ); // send the individual genotype to the evaluation program
	      queued  += 1;
	      ind += 1;
      }
    
      // wait for any results: receive message and add fitness values
      ReceiveIndividualFitness();
      remaining -= 1;
      queued    -= 1;
   }
}


/****************************************/
/****************************************/

void CParallelEvolution::SendIndividualParameters( UInt32 individualNumber ) {
   // search for the first available process
   SInt32 tid = -1;
   for( TMapProcessToIndividualIterator it = m_mapProcessToIndividual.begin(); it != m_mapProcessToIndividual.end();	++it ) {
      if( it->second == -1 ) {
	      tid = it->first;
	      break;
      }
   }

   if( tid == -1 ) {
      THROW_ARGOSEXCEPTION( "error: no process available to start evaluation" );
   }
   
   
   CEvaluationConfig* pc_evaluation_config = m_pcEvaluationStrategy->GetEvaluationConfig( individualNumber, *m_pcPopulation );;

   // sending seed and individual number
   m_cEvaluatorComm.Send(&individualNumber, 1, MPI_INT, tid, 1);

   // sending team composition
   UInt32 pun_teams[pc_evaluation_config->GetNumTeams()*pc_evaluation_config->GetNumTeams()];
   
   for( UInt32 i = 0; i < pc_evaluation_config->GetNumTeams(); i++ ) {
      TTeam team = pc_evaluation_config->GetTeam(i);
      
      for( UInt32 j = 0; j < pc_evaluation_config->GetTeamSize(); j++ ) {
	      pun_teams[i*pc_evaluation_config->GetTeamSize()+j] = team[j];
	      }      
   }
   
   m_cEvaluatorComm.Send(pun_teams, pc_evaluation_config->GetNumTeams()*pc_evaluation_config->GetTeamSize(), MPI_INT, tid, 1);
   
   // sending control parameters
   UInt32 unNumControllers = pc_evaluation_config->GetNumControllers();
   TMapParamters map_controllers = pc_evaluation_config->GetMapControlParameters();
   Real pf_control_parameters[m_pcPopulation->GetGenotypeSize()];
   
   m_cEvaluatorComm.Send(&unNumControllers, 1, MPI_INT, tid, 1);
   
   
   
   for( TMapParamtersIterator it = map_controllers.begin(); it != map_controllers.end(); ++it ) {
      UInt32 un_index = it->first;
      
      vector<Real> values = it->second.GetValues();
      std::copy(values.begin(), values.end(), pf_control_parameters);
      
      m_cEvaluatorComm.Send(&un_index, 1, MPI_INT, tid, 1);
      
      m_cEvaluatorComm.Send(pf_control_parameters, m_pcPopulation->GetGenotypeSize(), MPI_ARGOSREAL, tid, 1);
      
   }

   // sending evaluation random seeds
   m_cEvaluatorComm.Send(m_punEvaluationSeeds, m_pcEvaluationStrategy->GetNumSamples(), MPI_INT, tid, 1);

   // record the individual into the map, for later retrieval
   m_mapProcessToIndividual[tid] = individualNumber;

   // store the reference to the evaluation config
   m_vecEvaluationConfigurations[individualNumber] = pc_evaluation_config;
}


/****************************************/
/****************************************/

void CParallelEvolution::ReceiveIndividualFitness() {
   SInt32 tid;
   MPI::Status status;

   UInt32 num_objs = m_pcPopulation->GetNumObjectives()*m_pcEvaluationStrategy->GetNumSamples() + 
      m_pcEvaluationStrategy->GetTeamSize();
   Real objs[num_objs];
   m_cEvaluatorComm.Recv(objs, num_objs, MPI_ARGOSREAL, MPI_ANY_SOURCE, 1, status);
   
   // get the indi vidual number and reset the process2individual map
   tid = status.Get_source();
   UInt32 un_individual = m_mapProcessToIndividual[tid];
   m_mapProcessToIndividual[tid] = -1;

   // set the performance of the individual
   TVecObjectives vec_objs;
   for( UInt32 i = 0; i < m_pcEvaluationStrategy->GetNumSamples(); i++ ) {
      vec_objs.push_back( CObjectives(m_pcPopulation->GetNumObjectives(), objs + i*m_pcPopulation->GetNumObjectives()) );
   }
   
   // set the teams with which the individual participated
   vector<UInt32> vec_teams;
   for( UInt32 i = m_pcPopulation->GetNumObjectives()*m_pcEvaluationStrategy->GetNumSamples(); i < num_objs; i++ ) {
      vec_teams.push_back( (UInt32)rint(objs[i]) );
   }
   
   

   // compute average performance and set objectives
   CObjectives mean_performance(m_pcPopulation->GetNumObjectives());
   mean_performance.Mean( vec_objs );
   m_pcPopulation->SetPerformance( un_individual, mean_performance);
   // store the results
   m_vecEvaluationConfigurations[un_individual]->SetEvaluationResults(vec_objs);
   
   
}




REGISTER_EVOLUTION(CParallelEvolution, "parallel")
