#include "evolution.h"

const string CONFIGURATION_EVOLUTION_TYPE            = "type";
const string CONFIGURATION_EVOLUTION_EVOLUTION       = "evolution";
const string CONFIGURATION_EVOLUTION_MAXGENERATION   = "max_generation";
const string CONFIGURATION_EVOLUTION_SAVEGENERATION  = "save_generation_step";
const string CONFIGURATION_EVOLUTION_NUMGENERATION   = "num_generation";
const string CONFIGURATION_EVOLUTION_MAXIMISTAION    = "maximisation";
const string CONFIGURATION_EVOLUTION_RANDOMSEED      = "random_seed";
const string CONFIGURATION_EVOLUTION_EVOBASEFILE     = "evolution_basefile";
const string CONFIGURATION_EVALUATION_EVOBASEFILE    = "evaluation_basefile";
const string CONFIGURATION_EVOLUTION_FITNESSBASEFILE = "fitness_basefile";
const string CONFIGURATION_EVOLUTION_BESTINDBASEFILE = "bestind_basefile";
const string CONFIGURATION_EVOLUTION_EXPERIMENT      = "experiment";


/****************************************/
/****************************************/

CEvolution::CEvolution() :
   m_pcPopulation( NULL ),
   m_bVerbose( true ),
   m_unRandomSeed( 1 ),
   m_unNumGeneration( 0 ),
   m_unMaxGeneration( 0 ),
   m_unSaveGenerationStep( 1 ),
   m_sWorkingDir( "" ),
   m_sEvolutionBaseFilename(""),
   m_sEvaluationBaseFilename(""),
   m_sBestIndividualBaseFilename(""),
   m_sFitnessBaseFilename(""),
   m_sExperimentConfigurationFile("")
{
   if( !CRandom::ExistsCategory( "revolver" ) ) {
      CRandom::CreateCategory(  "revolver", 1 );
   }
   m_pcRNG = CRandom::CreateRNG(  "revolver" );   
}


/****************************************/
/****************************************/

CEvolution::~CEvolution() {
   CFactory<CPopulation>::Destroy();
   CFactory<CEvaluationStrategy>::Destroy();

   delete m_pcPopulation;
   delete m_pcEvaluationStrategy;
}


/****************************************/
/****************************************/

void CEvolution::Init( TConfigurationNode& t_configuration_tree ) {
   ////////////////////////////////////////////////////////////////////////////////
   // evolutionary paramters
   ////////////////////////////////////////////////////////////////////////////////
   GetNodeAttributeOrDefault(t_configuration_tree, CONFIGURATION_EVOLUTION_NUMGENERATION, m_unNumGeneration, m_unNumGeneration );
   GetNodeAttribute(t_configuration_tree, CONFIGURATION_EVOLUTION_MAXGENERATION, m_unMaxGeneration );
   GetNodeAttributeOrDefault(t_configuration_tree, CONFIGURATION_EVOLUTION_SAVEGENERATION, m_unSaveGenerationStep, m_unSaveGenerationStep );

   ////////////////////////////////////////////////////////////////////////////////
   // evolutionary base filenames
   ////////////////////////////////////////////////////////////////////////////////
   GetNodeAttribute(t_configuration_tree, CONFIGURATION_EVOLUTION_EVOBASEFILE, m_sEvolutionBaseFilename );
   GetNodeAttribute(t_configuration_tree, CONFIGURATION_EVALUATION_EVOBASEFILE, m_sEvaluationBaseFilename );
   GetNodeAttribute(t_configuration_tree, CONFIGURATION_EVOLUTION_BESTINDBASEFILE, m_sBestIndividualBaseFilename );
   GetNodeAttribute(t_configuration_tree, CONFIGURATION_EVOLUTION_FITNESSBASEFILE, m_sFitnessBaseFilename );

   ////////////////////////////////////////////////////////////////////////////////
   // experiment configuration file
   ////////////////////////////////////////////////////////////////////////////////
   GetNodeAttribute(t_configuration_tree, CONFIGURATION_EVOLUTION_EXPERIMENT, m_sExperimentConfigurationFile );


   ////////////////////////////////////////////////////////////////////////////////
   // initialise population of individuals
   ////////////////////////////////////////////////////////////////////////////////
   try {
      InitPopulation(GetNode(t_configuration_tree, "population"));
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("[REVOLVER] Error initializing population", ex);
   }

   ////////////////////////////////////////////////////////////////////////////////
   // initialise evaluation configuration
   ////////////////////////////////////////////////////////////////////////////////
   try {
      InitEvaluationStrategy(GetNode(t_configuration_tree, "evaluation"));
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("[REVOLVER] Error initializing evaluation strategy", ex);
   }
}


/****************************************/
/****************************************/

void CEvolution::InitPopulation(TConfigurationNode& t_tree) {
   try {
      string population_type;
      GetNodeAttribute(t_tree, "type", population_type);
      m_pcPopulation = CFactory<CPopulation>::New(population_type);
      m_pcPopulation->SetRNG(m_pcRNG);
      m_pcPopulation->Init(t_tree);

      // resize the vector for storing the evaluation configurations and the results
      m_vecEvaluationConfigurations.resize(m_pcPopulation->GetSize());
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("Error initializing population", ex);
   }
}


/****************************************/
/****************************************/

void CEvolution::InitEvaluationStrategy(TConfigurationNode& t_tree) {
   try {
      string evaluation_type;
      GetNodeAttribute(t_tree, "type", evaluation_type);
      m_pcEvaluationStrategy = CFactory<CEvaluationStrategy>::New(evaluation_type);
      m_pcEvaluationStrategy->SetRNG(m_pcRNG);
      m_pcEvaluationStrategy->Init(t_tree);
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("Error initializing evaluation stategy", ex);
   }
}


/****************************************/
/****************************************/

void CEvolution::Evolve() {
   // Initialise the evaluation process
   StartEvaluationProcess();

   // Main evaluation loop
   while( m_unNumGeneration < m_unMaxGeneration ) {
      // initialise random number generator for this generation
      SetRandomSeed( m_unRandomSeed );
      
      // evaluate popoulation
      LOG << "[REVOLVER] Evaluating population for generation " << m_unNumGeneration << endl; std::cout.flush();
      EvaluatePopulation();
      
      // dump population
      if( (m_unNumGeneration % m_unSaveGenerationStep == 0) || (m_unNumGeneration == m_unMaxGeneration -1) ) {
         DumpPopulation();
	      DumpEvaluationResults();
	      DumpBestIndividuals();
	      DumpPerformance();
      }

      // clear the structures used for logging evaluation configuration and results
      ClearLoggingStructures();

      LOG << "[REVOLVER] Generation " << m_unNumGeneration 
	  << " - best fitness: " << m_pcPopulation->GetBestIndividualFitness() 
	  << " - worst fitness: " << m_pcPopulation->GetWorstIndividualFitness() 
	  << " - avg fitness: " << m_pcPopulation->GetAveragePopulationFitness() << endl;
      LOG.Flush();

      // update the population through selection and mutation operators
      m_pcPopulation->Update();

      // get a random seed for the next generation
      m_unRandomSeed = m_pcRNG->Uniform(CRange<UInt32>(0,INT_MAX));
      
      m_unNumGeneration += 1;
   }

   // Terminate the evaluation procees
   StopEvaluationProcess();
}


/****************************************/
/****************************************/

void CEvolution::DumpPopulation() {
   // Write the population
   ofstream out;
   ostringstream filename;
   filename.fill( '0' );

   filename.str("");
   filename << m_sWorkingDir << "/" << m_sEvolutionBaseFilename << "_gen" << setw(6) << m_unNumGeneration << ".log";
   m_pcPopulation->Dump( filename.str() );
}


/****************************************/
/****************************************/

void CEvolution::DumpEvaluationResults() {
   // Write the population
   ofstream out;
   ostringstream filename;
   filename.fill( '0' );
   filename.str("");
   filename << m_sWorkingDir << "/" << m_sEvaluationBaseFilename << "_gen" << setw(6) << m_unNumGeneration << ".log";
   out.open( filename.str().c_str(), ios::out );
   for( UInt32 i = 0; i < m_pcPopulation->GetSize(); ++i ) {
      out << *m_vecEvaluationConfigurations[i];
   }
   out.close();
}


/****************************************/
/****************************************/

void CEvolution::DumpBestIndividuals() {
   ofstream out;
   ostringstream filename;
   filename.fill( '0' );
   filename << m_sWorkingDir << "/" << m_sBestIndividualBaseFilename << "_gen" << setw(6) << m_unNumGeneration << ".log";
   m_pcPopulation->DumpBestIndividuals( filename.str() );
}

/****************************************/
/****************************************/

void CEvolution::DumpPerformance() {
   ofstream out;
   ostringstream filename;
   filename.fill( '0' );

   // write the fitness value
   filename.str("");
   filename << m_sWorkingDir << "/" << m_sFitnessBaseFilename << ".log";

   if( m_unNumGeneration ) {
      out.open( filename.str().c_str(), ios::app );
   }
   else {
      out.open( filename.str().c_str(), ios::out );
   }
   out << m_unNumGeneration << " " 
       << m_pcPopulation->GetBestIndividualFitness() << " " 
       << m_pcPopulation->GetWorstIndividualFitness()<< endl;
   out.close();
}


/****************************************/
/****************************************/

void CEvolution::ClearLoggingStructures() {
   for( UInt32 i = 0; i < m_pcPopulation->GetSize(); ++i ) {
      if( m_vecEvaluationConfigurations[i] ){
         delete m_vecEvaluationConfigurations[i];
         m_vecEvaluationConfigurations[i] = NULL;
      }
   }
}
