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
 protected:
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
    // A counter that counts how many times the task was NOT switched
    UInt32 m_unNonSwitchingTaskCounter;
    // A counter that counts how many total actions the agent has done
    UInt32 m_unTotalActionsPerAgent;
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
   // sigma standard deviation for fitness 2 formula. 0.1 in Duarte's experiments
   Real m_fSigmaFitness2;
 
   // fitness of the experiment
   Real m_fFitness;
 
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
   string m_sResultsFilename;
   string m_sEndrunResultsBasename;
   ofstream outputResults;
   ofstream outputResultsEndrun;
   
   // other variables
   Real m_fMonomorphicGenotype;
   string m_sFitnessToUse;
   string m_sFitnessAveraging;
   
   string m_sStandaloneGenotypeString;
   CVector<Real> m_cStandaloneGenotype;
   
 public:
   CSimulator();
   ~CSimulator();
   
   inline void SetExperimentFileName( const string& filename ) { m_sExperimentFilename = filename; };
   inline void SetRandomSeed(const UInt32 random_seed) {m_unRandomSeed = random_seed; m_pcRNG->SetSeed(random_seed); m_pcRNG->Reset();};
   inline void SetTrialNumber(const UInt32 trial_number) {m_unTrialNumber = trial_number;};
   
   virtual void LoadExperiment();
   
   virtual void SetControlParameters(CEvaluationConfig* e_config);
   inline void SetMonomorphicGenotype(Real f_monomorphic_genotype){m_fMonomorphicGenotype = f_monomorphic_genotype;};
   
   virtual void WriteResults(UInt32 u_timestep);
   
   virtual void Execute();
   
   virtual Real ComputeFitnessWeak(UInt32 u_actions_A, UInt32 u_actions_B);
   virtual Real ComputeFitnessStrong(UInt32 u_actions_A, UInt32 u_actions_B);
   virtual Real ComputeSpecializationUpToTimestep(UInt32 u_end_timestep);
   virtual Real ComputeFitness3OverallActions(UInt32 u_initial_timestep, UInt32 u_end_timestep);
   virtual CObjectives ComputePerformanceInExperiment();
   
   virtual CRandom::CRNG* GetRNG(){return m_pcRNG;};
   
   virtual void Reset();
   virtual void Destroy();
   
   virtual CEvaluationConfig GenerateFoundingTeam(UInt32 un_team_size, UInt32 un_genotype_length, CRange<Real>& m_cGenotypeValueRange, Real f_recombination_factor, UInt32 un_num_samples);

};


#endif
