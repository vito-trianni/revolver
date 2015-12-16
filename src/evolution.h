/* -*- Mode:C++ -*- */

#ifndef EVOLUTION_H
#define EVOLUTION_H

#include <argos_utility/plugins/factory.h>
#include <argos_utility/configuration/base_configurable_resource.h>
#include <argos_utility/datatypes/datatypes.h>
#include <argos_utility/logging/argos_log.h>

#include <utility/objectives.h>

#include <fstream>
#include <sstream>
#include <iomanip>



#include <map>
#include <limits.h>

//#include <Rng/GlobalRng.h>

#include "revolver_general.h"
#include "population.h"
#include "evaluation_strategy.h"

using namespace std;
using namespace argos;

class CEvolution : public CBaseConfigurableResource {
protected:
   // variables for the random number generation
   CRandom::CRNG*  m_pcRNG;

   // population class
   CPopulation* m_pcPopulation;
   
   // evaluation strategy class
   CEvaluationStrategy* m_pcEvaluationStrategy;

   // verbose flag
   bool m_bVerbose;
   
   // evolution flag (to distinguish from post-evaluation)
   bool m_bEvolutionFlag;
   
   // random seed of the evolutionary run
   UInt32 m_unRandomSeed;
   
   // maximisation/minimisation flag
   bool m_bMaximization;
   
   // counter and limits for evolutionary generations
   UInt32 m_unNumGeneration;
   UInt32 m_unMaxGeneration;
   UInt32 m_unSaveGenerationStep;
   
   // filenames and directories to store the evolutionary results
   string m_sWorkingDir;
   string m_sEvolutionBaseFilename;
   string m_sEvaluationBaseFilename;
   string m_sBestIndividualBaseFilename;
   string m_sFitnessBaseFilename;
      
   // configuration file of the evolutionary experiment
   string m_sExperimentConfigurationFile;

   // structures for storing evaluation configuration and results
   TVecEvaluationConigurations  m_vecEvaluationConfigurations;
   vector<TVecObjectives> m_vecPerformance;
   
   
public:
   CEvolution();
   virtual ~CEvolution();
   
   // generic initialisation from the configuration tree, loaded from the xml file
   virtual void Init( TConfigurationNode& t_configuration_tree );
   virtual void InitPopulation(TConfigurationNode& t_tree);
   virtual void InitEvaluationStrategy(TConfigurationNode& t_tree);


   // start the evaluation process with initialisation operations
   virtual void StartEvaluationProcess() = 0;

   // stop the evaluation process, with termination operations
   virtual void StopEvaluationProcess() = 0;;

   // main evolutionary loop
   virtual void Evolve();

   // main loop for post evaluation
   virtual void PostEvaluation() = 0;

   // evaluate the population of genotypes
   virtual void EvaluatePopulation() = 0;

   // functions for dumping into files
   virtual void DumpPopulation();
   virtual void DumpEvaluationResults();
   virtual void DumpBestIndividuals();
   virtual void DumpPerformance();

   virtual void ClearLoggingStructures();

   // functions for loading from files
   // virtual void LoadGeneration() = 0;
   // virtual void LoadBestIndividuals() = 0;

   // destroy function
   inline virtual void Destroy() {};
      
   // reset function
   inline virtual void Reset() {};
      

   ////////////////////////////////////////////////////////////////////////////////
   // get/set functions
   ////////////////////////////////////////////////////////////////////////////////

   // get/set verbose flag
   inline const bool GetVerbose() { return m_bVerbose; };
   inline void SetVerbose( const bool& b_verbose ) { m_bVerbose = b_verbose; };
      
   // get/set post evaluation flag
   inline const bool GetEvolutionFlag() { return m_bEvolutionFlag; };
   inline void SetEvolutionFlag( const bool& b_evolution ) { m_bEvolutionFlag = b_evolution; };
      
   // get/set random seed
   inline const UInt32 GetRandomSeed() { return m_unRandomSeed; };
   inline void SetRandomSeed( const UInt32& un_seed ) { m_unRandomSeed = un_seed; m_pcRNG->SetSeed(un_seed); m_pcRNG->Reset();};
      
   // get/set maximistaion flag
   inline const bool GetMaximization() { return m_bMaximization; };
   inline void SetMaximization( const bool& b_maximisation ) { m_bMaximization = b_maximisation; };
      
   // get/set generation number
   inline const UInt32 GetNumGeneration() { return m_unNumGeneration; };
   inline void SetNumGeneration( const UInt32& un_num_generation ) { m_unNumGeneration = un_num_generation; };
    
   // get/set max generation number
   inline const UInt32 GetMaxGeneration() { return m_unMaxGeneration; };
   inline void SetMaxGeneration( const UInt32& un_max_generation ) { m_unMaxGeneration = un_max_generation; };
    
   // get/set number of step of generations to jump beofre saving
   inline const UInt32 GetSaveGenerationStep() { return m_unSaveGenerationStep; };
   inline void SetSaveGenerationStep( const UInt32& un_save_generation_step ) { m_unSaveGenerationStep = un_save_generation_step; };

   // get/set working dir
   inline const string GetWorkingDir() { return m_sWorkingDir; };
   inline void SetWorkingDir( const string& dir ) { m_sWorkingDir = dir; };

   // get/set base filename for full generation dump
   inline const string GetEvolutionBaseFilename() { return m_sEvolutionBaseFilename; };
   inline void SetEvolutionBaseFilename( const string& base ) { m_sEvolutionBaseFilename = base; };

   // get/set base filename for the best individual/pareto front
   inline const string GetBestIndividualBaseFilename() { return m_sBestIndividualBaseFilename; };
   inline void SetBestIndividualBaseFilename( const string& base ) { m_sBestIndividualBaseFilename = base; };

   // get/set experiment configuration file
   inline const string GetExperimentConfigurationFile() { return m_sExperimentConfigurationFile; };
   inline void SetExperimentConfigurationFile( const string& s_config ) { m_sExperimentConfigurationFile = s_config; };

};

typedef CFactory<CEvolution> TFactoryEvolution;
#define REGISTER_EVOLUTION(CLASSNAME, LABEL)	  \
   REGISTER_SYMBOL(CEvolution,			  \
                   CLASSNAME,                     \
                   LABEL,                         \
                   "undefined",                   \
                   "undefined",                   \
                   "undefined",                   \
                   "undefined",                   \
                   "undefined")

#endif
