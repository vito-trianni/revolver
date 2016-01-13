#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include <algorithm>
#include <argos_utility/configuration/base_configurable_resource.h>
#include <utility/evaluation_config.h>
#include "definitions.h"

using namespace std;
using namespace argos;

// the class that contains the code of the numerical simulation. It performs: mating, simulation, and fitness computation
class CSimulator {
 protected:
   // agent structure
   struct Agent {
    // Control parameters
    Real m_fThresholdTaskA;
    Real m_fThresholdTaskB;
    // A counter that is used to wait while switching (in presence of switching cost)
    UInt32 m_unSwitchingTimestep;
   };
   
   struct Actions {
     UInt32 m_unIdle;
     UInt32 m_unTaskA;
     UInt32 m_unTaskB;
   };
   
   // the vector containing all agents
   vector<Agent> agents;
   
   // the vector that stores all tasks executed over time. Used for computing fitness and to decrease stimulus.
   vector<Actions> actionsOverTime;
 
   // random seed
   UInt32 m_unRandomSeed;
   
   // variables for the random number generation
   CRandom::CRNG*  m_pcRNG;
   
   // trial number
   UInt32 m_unTrialNumber;
 
   // Parameters of the experiment
   // size of the colony
   UInt32 m_unColonySize;
   // total simulation duration
   UInt32 m_unTotalDurationTimesteps;
   // delta parameters for stimulus increase
   Real m_fDeltaStimulusIncreaseTaskA;
   Real m_fDeltaStimulusIncreaseTaskB;
   // alpha parameters for stimulus decrease (as a function of active workers for task i)
   Real m_fAlphaStimulusDecreaseTaskA;
   Real m_fAlphaStimulusDecreaseTaskB;
   // switching cost
   UInt32 m_unSwitchingCost;
   // beta weight factor for fitness
   Real m_fBetaFitnessWeightFactor;
 
   // fitness of the experiment
   Real m_fFitness;
 
   // Control parameters
   //Real m_fThresholdTaskA;
   //Real m_fThresholdTaskB;
   
   // Time-varying variables
   Real m_fStimulusTaskA;
   Real m_fStimulusTaskB;
   
   // filenames and directories
   string m_sExperimentFilename;
   
 public:
   CSimulator();
   ~CSimulator();
   
   inline void SetExperimentFileName( const string& filename ) { m_sExperimentFilename = filename; };
   inline void SetRandomSeed(const UInt32 random_seed) {m_unRandomSeed = random_seed; m_pcRNG->SetSeed(random_seed); m_pcRNG->Reset();};
   inline void SetTrialNumber(const UInt32 trial_number) {m_unTrialNumber = trial_number;};
   
   virtual void LoadExperiment();
   
   virtual void SetControlParameters(CEvaluationConfig* e_config);
   
   virtual void Execute();
   
   virtual Real ComputePerformanceInExperiment();
   
   virtual CRandom::CRNG* GetRNG(){return m_pcRNG;};
   
   virtual void Reset();
   virtual void Destroy();

};


#endif
