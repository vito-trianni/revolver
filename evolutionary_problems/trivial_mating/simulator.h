#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include <algorithm>
#include <limits>
// #include <fstream>
// #include <sstream>
// #include <iomanip>
#include <argos_utility/configuration/base_configurable_resource.h>
#include <utility/evaluation_config.h>
#include "definitions.h"

using namespace std;
using namespace argos;

// the class that contains the code of the numerical simulation. It performs: mating, simulation, and fitness computation
class CSimulator {
 public:
   // agent structure
   struct Agent {
    // Control parameters
    Real m_fThresholdTaskA;
    Real m_fThresholdTaskB;
    // Current task executed by agent. Needed for determining task switching (otherwise system is memoryless).
    // 0 is idle, 1 is task A, 2 is task B, and so on (if more than 2). 
    // In this current implementation I won't track idle agents.
    // This is because we are assuming doing A -> (idle)+ -> A is equivalent to A -> A
    UInt32 m_unCurrentTask;
    // A counter that is used to wait while switching (in presence of switching cost)
    UInt32 m_unSwitchingTimestep;
    // A state variable that memorizes whether the agent is switching or non-switching state
    bool m_bSwitchingTask;
    // A counter that counts how many times the task was NOT switched
    UInt32 m_unNonSwitchingTaskCounter;
    // A counter that counts how many total actions the agent has done
    UInt32 m_unTotalActionsPerAgent;
   };
   
 protected:
   
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
   // time steps to skip when computing fitness
   UInt32 m_unTimestepsToSkipForFitness;
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
   // sigma standard deviation for fitness 2 formula. 0.1 in Duarte's experiments
   Real m_fSigmaFitness2;
 
   // fitness of the experiment
   Real m_fFitnessWeakOverall;
   Real m_fFitnessStrongOverall;
   Real m_fFitnessStrongOverallLogActs;
 
   // Control parameters
   //Real m_fThresholdTaskA;
   //Real m_fThresholdTaskB;
   
   // Time-varying variables
   Real m_fStimulusTaskA;
   Real m_fStimulusTaskB;
   
   UInt32 m_unOverallTotalActionsA;
   UInt32 m_unOverallTotalActionsB;
   Real m_fOverallProportionTaskA;
   Real m_fOverallProportionTaskB;
   Real m_fOverallTotalActions;
   Real m_fSpecialization;
   
   // filenames and directories
   string m_sExperimentFilename;
   bool m_bWriteResults;
   bool m_bWriteResultsTime;
   string m_sResultsFilename;
   string m_sEndrunResultsBasename;
   ofstream outputResults;
   ofstream outputResultsEndrun;
   
   // other variables
   string m_sFitnessToUse;
   string m_sFitnessAveraging;
   
   // model type
   string m_sModelType;
   
 public:
   CSimulator();
   ~CSimulator();
   
   inline UInt32 GetColonySize(){return m_unColonySize;};
   inline void AddAgentToColony(Agent& agent){ agents.push_back(agent); };
   
   inline void SetExperimentFileName( const string& filename ) { m_sExperimentFilename = filename; };
   inline void SetRandomSeed(){m_unRandomSeed = m_pcRNG->Uniform(CRange<UInt32>(0,INT_MAX));m_pcRNG->SetSeed(m_unRandomSeed); m_pcRNG->Reset();};
   inline void SetRandomSeed(const UInt32 random_seed) {m_unRandomSeed = random_seed; m_pcRNG->SetSeed(random_seed); m_pcRNG->Reset();};
   inline void SetTrialNumber(const UInt32 trial_number) {m_unTrialNumber = trial_number;};
   
   virtual void LoadExperiment();
   
   virtual void SetControlParameters(CEvaluationConfig* e_config);
   
   virtual void WriteResults(UInt32 u_timestep);
   
   virtual void StepTwoThresholdsDuarte(std::vector<Agent>::iterator it, Actions& c_actions_this_timestep);
   virtual void StepOneThreshold(std::vector<Agent>::iterator it, Actions& c_actions_this_timestep);
   virtual void Execute();
   
   virtual Real ComputeFitnessWeak(UInt32 u_actions_A, UInt32 u_actions_B);
   virtual Real ComputeFitnessStrong(UInt32 u_actions_A, UInt32 u_actions_B, bool logActions);
   virtual Real ComputeSpecializationUpToTimestep(UInt32 u_end_timestep);
   virtual void UpdateFitness3and4OverallActions(UInt32 u_initial_timestep, UInt32 u_end_timestep);
   virtual CObjectives ComputePerformanceInExperiment();
   
   virtual CRandom::CRNG* GetRNG(){return m_pcRNG;};
   
   //virtual inline void SetModelType(string s_model_type){m_sModelType = s_model_type;};
   virtual inline string GetModelType(){return m_sModelType;};
   
   virtual void Reset();
   virtual void Destroy();
   
};


#endif
