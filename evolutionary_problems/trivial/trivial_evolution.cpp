#include "trivial_evolution.h"

/****************************************/
/****************************************/

CTrivialEvolution::CTrivialEvolution() :
   CEvolution()
{}


/****************************************/
/****************************************/

CTrivialEvolution::~CTrivialEvolution()
{}


/****************************************/
/****************************************/

// TODO: reimplement ex-novo this class

void CTrivialEvolution::StartEvaluationProcess() {
   ////////////////////////////////////////////////////////////////////////////////
   // parse the command line
   ////////////////////////////////////////////////////////////////////////////////
   
   LOG.Flush();
   
   // Initialize simulator
}


/****************************************/
/****************************************/

void CTrivialEvolution::StopEvaluationProcess() {
   // Destroy simulator
}


/****************************************/
/****************************************/

void CTrivialEvolution::EvaluatePopulation() {
   
   for( UInt32 i = 0; i < m_pcPopulation->GetSize(); ++i ) {
      
      CEvaluationConfig* pc_evaluation_config = m_pcEvaluationStrategy->GetEvaluationConfig( i, *m_pcPopulation );
      m_vecEvaluationConfigurations[i] = pc_evaluation_config;
      
      CControlParameters genotype = m_pcPopulation->GetIndividualParameters(i);
      //LOG << "Individual " << i << " genotype: " << std::endl;
      vector<Real> values = genotype.GetValues();
      Real fitness = 0.0;
      for (std::vector<Real>::iterator it = values.begin(); it != values.end(); ++it){
         fitness += *it;
         //LOG << ' ' << *it;
      }
      
      //LOG << " Fitness: " << fitness <<  std::endl;
      
      CObjectives objs(1,&fitness);
      //objs.push_back(fitness);
      
      m_pcPopulation->SetPerformance( i, objs);
      
      TVecObjectives vec_objectives;
      
	   vec_objectives.push_back( objs );
	   
	   m_vecEvaluationConfigurations[i]->SetEvaluationResults( vec_objectives );
   }
   
   // get an instance of the simulator
   //CSimulator& cSimulator = CSimulator::GetInstance();
   //CEvolutionaryLoopFunctions* pcEvoLoopFunctions = &dynamic_cast<CEvolutionaryLoopFunctions&>(cSimulator.GetLoopFunctions());

   //UInt32 unNumSampleSeeds = m_pcEvaluationStrategy->GetNumSamples();
   //CVector<UInt32> punSampleSeeds(unNumSampleSeeds);
   //for( UInt32 i = 0; i < unNumSampleSeeds; ++i ) {
   //   punSampleSeeds[i] = m_pcRNG->Uniform(CRange<UInt32>(0,INT_MAX));
   //}

   // iterate in the whole population
   //for( UInt32 i = 0; i < m_pcPopulation->GetSize(); ++i ) {
   //   CEvaluationConfig* pc_evaluation_config = m_pcEvaluationStrategy->GetEvaluationConfig( i, *m_pcPopulation );
   //   pc_evaluation_config->SetSampleSeeds( punSampleSeeds );

      // store the evaluation configuration
   //   m_vecEvaluationConfigurations[i] = pc_evaluation_config;

      // iterate for all sample trials
   //   TVecObjectives vec_objectives;
   //   for( UInt32 s = 0; s < unNumSampleSeeds; ++s ) {
	 // set the random seed in the simulator
	// cSimulator.SetRandomSeed(pc_evaluation_config->GetSampleSeed(s));
	 
 	 // resetting the experiment
 	// cSimulator.Reset();

	 // set the controller parameters
	// pcEvoLoopFunctions->SetEvolution();
 	// pcEvoLoopFunctions->SetTrialNumber( s );
	// pcEvoLoopFunctions->ConfigureEvaluation( pc_evaluation_config );
	 
	 // run the simulation
	// cSimulator.Execute();

	 // retrieve the fitness values
	// CObjectives objs = pcEvoLoopFunctions->ComputePerformanceInExperiment();
	// vec_objectives.push_back( objs );
   //   }
      
      // compute average performance and set objectives
   //   CObjectives mean_performance(m_pcPopulation->GetNumObjectives());
   //   mean_performance.Mean( vec_objectives );
   //   m_pcPopulation->SetPerformance( i, mean_performance);

      // store the results
   //   m_vecEvaluationConfigurations[i]->SetEvaluationResults( vec_objectives );
   //}
}

REGISTER_EVOLUTION(CTrivialEvolution, "trivial")
