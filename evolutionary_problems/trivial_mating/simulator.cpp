#include "simulator.h"

const string CONFIGURATION_RANDOM_SEED                  = "random_seed";

const string CONFIGURATION_COLONY_SIZE                  = "colony_size";
const string CONFIGURATION_TOTAL_DURATION               = "total_duration_timesteps";
const string CONFIGURATION_TIMESTEPS_TO_SKIP_FITNESS    = "timesteps_to_skip_for_fitness";

const string CONFIGURATION_DELTA_STIMULUS_INCREASE_A    = "delta_stimulus_increase_A";
const string CONFIGURATION_DELTA_STIMULUS_INCREASE_B    = "delta_stimulus_increase_B";

const string CONFIGURATION_ALPHA_STIMULUS_DECREASE_A    = "alpha_stimulus_decrease_A";
const string CONFIGURATION_ALPHA_STIMULUS_DECREASE_B    = "alpha_stimulus_decrease_B";

const string CONFIGURATION_SWITCHING_COST               = "switching_cost";
const string CONFIGURATION_BETA_FITNESS_WEIGHT_FACTOR   = "beta_fitness_weight_factor";
const string CONFIGURATION_SIGMA_FITNESS2               = "sigma_fitness2";

const string CONFIGURATION_WRITE_RESULTS                = "write_results";
const string CONFIGURATION_WRITE_RESULTS_TIME           = "write_results_time";
const string CONFIGURATION_RESULTS_FILENAME             = "results_filename";
const string CONFIGURATION_ENDRUN_RESULTS_BASENAME      = "results_endrune_basename";

const string CONFIGURATION_FITNESS_TO_USE               = "fitness_to_use";
const string CONFIGURATION_FITNESS_AVERAGING            = "fitness_averaging";

const string CONFIGURATION_MODEL_TYPE                   = "model_type";

const string FITNESS_TYPE_WEAK                          = "weak";
const string FITNESS_TYPE_STRONG                        = "strong";
const string FITNESS_TYPE_WEAK_OVERALL                  = "weak_overall";
const string FITNESS_TYPE_STRONG_OVERALL                = "strong_overall";
const string FITNESS_TYPE_STRONG_OVERALL_LOG_ACTS       = "strong_overall_log_acts";

const string FITNESS_AVERAGING_ARITHMETIC               = "ari";
const string FITNESS_AVERAGING_GEOMETRIC                = "geo";

const string MODEL_TYPE_TWO_THRESHOLDS_DUARTE           = "two_thresholds";
const string MODEL_TYPE_SINGLE_THRESHOLD                = "single_threshold";

/****************************************/
/****************************************/

CSimulator::CSimulator():
    m_unRandomSeed(0),
    m_unTrialNumber(0),
    m_unColonySize(0),
    m_unTotalDurationTimesteps(0),
    m_unTimestepsToSkipForFitness(0),
    m_fDeltaStimulusIncreaseTaskA(0.0),
    m_fDeltaStimulusIncreaseTaskB(0.0),
    m_fAlphaStimulusDecreaseTaskA(0.0),
    m_fAlphaStimulusDecreaseTaskB(0.0),
    m_unSwitchingCost(0),
    m_fBetaFitnessWeightFactor(0.0),
    m_fSigmaFitness2(0.0),
    m_fFitnessWeakOverall(0.0),
    m_fFitnessStrongOverall(0.0),
    m_fFitnessStrongOverallLogActs(0.0),
    m_fStimulusTaskA(0.0),
    m_fStimulusTaskB(0.0),
    m_unOverallTotalActionsA(0),
    m_unOverallTotalActionsB(0),
    m_fOverallProportionTaskA(0.0),
    m_fOverallProportionTaskB(0.0),
    m_fOverallTotalActions(0.0),
    m_fSpecialization(0.0),
    m_bWriteResults(false),
    m_bWriteResultsTime(false),
    m_sResultsFilename(""),
    m_sEndrunResultsBasename(""),
    m_sExperimentFilename(""),
    m_sFitnessToUse(""),
    m_sFitnessAveraging(""),
    m_sModelType("")
{
    if( !CRandom::ExistsCategory( "simulator" ) ) {
      CRandom::CreateCategory(  "simulator", 1 );
    }
    m_pcRNG = CRandom::CreateRNG(  "simulator" );   
}

CSimulator::~CSimulator(){
    
}


/****************************************/
/****************************************/

void CSimulator::LoadExperiment(){
    
    ticpp::Document tConfiguration;
    tConfiguration.LoadFile(m_sExperimentFilename);
    TConfigurationNode tConfigurationRoot = *tConfiguration.FirstChildElement();
    TConfigurationNode t_simulator_configuration = GetNode(tConfigurationRoot, "simulator"); 
    
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_RANDOM_SEED, m_unRandomSeed );
    m_pcRNG->SetSeed(m_unRandomSeed); 
    m_pcRNG->Reset();
    
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_COLONY_SIZE, m_unColonySize );
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_TOTAL_DURATION, m_unTotalDurationTimesteps );
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_TIMESTEPS_TO_SKIP_FITNESS, m_unTimestepsToSkipForFitness );
    
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_DELTA_STIMULUS_INCREASE_A, m_fDeltaStimulusIncreaseTaskA );
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_DELTA_STIMULUS_INCREASE_B, m_fDeltaStimulusIncreaseTaskB );
    
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_ALPHA_STIMULUS_DECREASE_A, m_fAlphaStimulusDecreaseTaskA );
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_ALPHA_STIMULUS_DECREASE_B, m_fAlphaStimulusDecreaseTaskB );
    
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_SWITCHING_COST, m_unSwitchingCost );
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_BETA_FITNESS_WEIGHT_FACTOR, m_fBetaFitnessWeightFactor );
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_SIGMA_FITNESS2, m_fSigmaFitness2 );
    
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_WRITE_RESULTS, m_bWriteResults );
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_WRITE_RESULTS_TIME, m_bWriteResultsTime );
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_RESULTS_FILENAME, m_sResultsFilename);
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_ENDRUN_RESULTS_BASENAME, m_sEndrunResultsBasename);
    
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_FITNESS_TO_USE,    m_sFitnessToUse);
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_FITNESS_AVERAGING, m_sFitnessAveraging);

    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_MODEL_TYPE, m_sModelType);

    if(m_bWriteResultsTime){
        ostringstream filename;
        filename.fill( '0' );
        filename.str("");
        filename << m_sResultsFilename;
        outputResults.open( filename.str().c_str(), ios::out );
        outputResults << "Timestep\tStimA\tStimB\tRobotsA\tRobotsB\tRobotsIDLE\tFitness1\tFitness2\tSpecialization" << std::endl;

    }
}

/****************************************/
/****************************************/

void CSimulator::SetControlParameters(CEvaluationConfig* e_config){
    for(UInt32 i = 0 ; i < m_unColonySize ; ++i){
        Agent cNewAgent = {};
        CGenotype cOffSpringGenotype = e_config->GetOffspringGenotype(m_pcRNG);
        if(m_sModelType.compare(MODEL_TYPE_TWO_THRESHOLDS_DUARTE) == 0 && cOffSpringGenotype.GetSize() == 2){
            cNewAgent.m_fThresholdTaskA = cOffSpringGenotype.GetValues()[0];
            cNewAgent.m_fThresholdTaskB = cOffSpringGenotype.GetValues()[1];
        }
        else if(m_sModelType.compare(MODEL_TYPE_SINGLE_THRESHOLD) == 0 && cOffSpringGenotype.GetSize() == 1){
            cNewAgent.m_fThresholdTaskA = cOffSpringGenotype.GetValues()[0];
        }
        else{
            LOGERR << "[ERROR] Model type and combination with genotype size non existent. Model type: " << m_sModelType << ". Genotype size: " << cOffSpringGenotype.GetSize() << ". Exiting ..." << std::endl;
            exit(-1);
        }
        //LOG << "Created agent with A: " << cNewAgent.m_fThresholdTaskA << " and B " << cNewAgent.m_fThresholdTaskB << std::endl;
        cNewAgent.m_unCurrentTask = 0; // IDLE
        cNewAgent.m_unSwitchingTimestep = 0;
        cNewAgent.m_bSwitchingTask = false;
        cNewAgent.m_unNonSwitchingTaskCounter = 0;
        cNewAgent.m_unTotalActionsPerAgent = 0;
        agents.push_back(cNewAgent);
        
    }
}

/****************************************/
/****************************************/

Real CSimulator::ComputeFitnessWeak(UInt32 u_actions_A, UInt32 u_actions_B){
    return (pow((Real)u_actions_A,m_fBetaFitnessWeightFactor) * pow((Real)u_actions_B,1.0 - m_fBetaFitnessWeightFactor));
}

/****************************************/
/****************************************/

Real CSimulator::ComputeFitnessStrong(UInt32 u_actions_A, UInt32 u_actions_B, bool logActions){
    Real fTotalActions    = ((Real)u_actions_A + (Real)u_actions_B);
    Real fProportionTaskA = 0.0;
    Real fProportionTaskB = 0.0;
    if( fTotalActions > 0.0){
        fProportionTaskA = (Real)u_actions_A / fTotalActions;
        fProportionTaskB = (Real)u_actions_B / fTotalActions;
    }
    Real fFitness = 0.0;
    //LOGERR << "Total actions " << fTotalActions << std::endl;
    if(!logActions){
        fFitness = fTotalActions        * exp(- ((fProportionTaskA - m_fBetaFitnessWeightFactor) * (fProportionTaskA - m_fBetaFitnessWeightFactor) / (2.0 * m_fSigmaFitness2 * m_fSigmaFitness2 )));
    }
    else {
        // The following is what Duarte actually did in her code
        fFitness = log(fTotalActions)   * exp(- ((fProportionTaskA - m_fBetaFitnessWeightFactor) * (fProportionTaskA - m_fBetaFitnessWeightFactor) / (2.0 * m_fSigmaFitness2 * m_fSigmaFitness2 )));
    }
    return fFitness;
}

/****************************************/
/****************************************/

void CSimulator::UpdateFitness3and4OverallActions(UInt32 u_initial_timestep, UInt32 u_end_timestep){
    m_unOverallTotalActionsA = 0;
    m_unOverallTotalActionsB = 0;
    
    for(UInt32 i = u_initial_timestep; i < u_end_timestep; ++i){
        m_unOverallTotalActionsA += actionsOverTime[i].m_unTaskA;
        m_unOverallTotalActionsB += actionsOverTime[i].m_unTaskB;
    }
    
    //LOGERR << "Computing overall fitness" << std::endl;
    m_fFitnessWeakOverall = ComputeFitnessWeak(m_unOverallTotalActionsA, m_unOverallTotalActionsB);
    m_fFitnessStrongOverall = ComputeFitnessStrong(m_unOverallTotalActionsA, m_unOverallTotalActionsB, false);
    m_fFitnessStrongOverallLogActs = ComputeFitnessStrong(m_unOverallTotalActionsA, m_unOverallTotalActionsB, true);
}

/****************************************/
/****************************************/

Real CSimulator::ComputeSpecializationUpToTimestep(UInt32 u_end_timestep){
    m_unOverallTotalActionsA = 0;
    m_unOverallTotalActionsB = 0;
    m_fOverallProportionTaskA = 0.0;
    m_fOverallProportionTaskB = 0.0;
    m_fOverallTotalActions = 0.0;
    m_fSpecialization = 0.0;
    for(UInt32 i = 0; i < u_end_timestep; ++i){
        m_unOverallTotalActionsA += actionsOverTime[i].m_unTaskA;
        m_unOverallTotalActionsB += actionsOverTime[i].m_unTaskB;
        m_fOverallTotalActions += (Real) actionsOverTime[i].m_unTaskA;
        m_fOverallTotalActions += (Real) actionsOverTime[i].m_unTaskB;
    }
    
    if(m_fOverallTotalActions > 0.0){
        m_fOverallProportionTaskA = (Real) m_unOverallTotalActionsA / m_fOverallTotalActions;
        m_fOverallProportionTaskB = (Real) m_unOverallTotalActionsB / m_fOverallTotalActions;    
    }
    
    Real fAveragedProbabilityQPerformingSameTask = 0.0;
    
    UInt32 m_unNumAgentsThatWorkedAtLeastTwice = 0;
    
    for(UInt32 i = 0 ; i < m_unColonySize ; ++i) {
        if(agents[i].m_unNonSwitchingTaskCounter > 0){
            fAveragedProbabilityQPerformingSameTask += (Real) agents[i].m_unNonSwitchingTaskCounter / (Real) agents[i].m_unTotalActionsPerAgent;
            m_unNumAgentsThatWorkedAtLeastTwice++;
        }
    }
    if(m_unNumAgentsThatWorkedAtLeastTwice > 0){
        fAveragedProbabilityQPerformingSameTask /= m_unNumAgentsThatWorkedAtLeastTwice;
    }
    
    //LOG << "fAveragedProbabilityQPerformingSameTask " << fAveragedProbabilityQPerformingSameTask << std::endl;
    
    if(m_fOverallProportionTaskA > 0.0 && m_fOverallProportionTaskB > 0.0){
        m_fSpecialization = (fAveragedProbabilityQPerformingSameTask / ( (m_fOverallProportionTaskA*m_fOverallProportionTaskA)+(m_fOverallProportionTaskB*m_fOverallProportionTaskB) )) - 1.0;
    }
    return m_fSpecialization;
}

/****************************************/
/****************************************/

void CSimulator::WriteResults(UInt32 u_timestep){
    
    if(m_bWriteResultsTime){
  
        Real fFitnessWeak = ComputeFitnessWeak(actionsOverTime[u_timestep].m_unTaskA, actionsOverTime[u_timestep].m_unTaskB);
        Real fFitnessStrong = ComputeFitnessStrong(actionsOverTime[u_timestep].m_unTaskA, actionsOverTime[u_timestep].m_unTaskB, false);
        Real fSpecialization = ComputeSpecializationUpToTimestep(u_timestep);
        
        outputResults << u_timestep                             << "\t";
        outputResults << m_fStimulusTaskA                       << "\t";
        outputResults << m_fStimulusTaskB                       << "\t";
        outputResults << actionsOverTime[u_timestep].m_unTaskA  << "\t";
        outputResults << actionsOverTime[u_timestep].m_unTaskB  << "\t";
        outputResults << actionsOverTime[u_timestep].m_unIdle   << "\t";
        outputResults << fFitnessWeak                              << "\t";
        outputResults << fFitnessStrong                              << "\t";
        outputResults << fSpecialization                        << std::endl;
    }
}

/****************************************/
/****************************************/

void CSimulator::StepOneThreshold(std::vector<Agent>::iterator it, Actions& c_actions_this_timestep){
    // Compute combined stimulus sA - sB
    Real fPerceivedCombinedStimulus = m_fStimulusTaskA - m_fStimulusTaskB + m_pcRNG->Gaussian(1.0,0.0);
    bool bPerformTaskA = false;
    bool bPerformTaskB = false;
    
    // If combined stimulus is very close to zero, select a task at random
    if (abs(fPerceivedCombinedStimulus - it->m_fThresholdTaskA) < 0.001 && !it->m_bSwitchingTask){
        Real fTaskChoiceRandom = m_pcRNG->Uniform(CRange<Real>(0.0,1.0));
        if(fTaskChoiceRandom < 0.5){
            bPerformTaskA = true;
        }
        else{
            bPerformTaskB = true;
        }
    }
    else if(fPerceivedCombinedStimulus > it->m_fThresholdTaskA && !it->m_bSwitchingTask){
        // Combined stimulus > thrA --> Do task A
        bPerformTaskA = true;
    }
    else if(fPerceivedCombinedStimulus < it->m_fThresholdTaskA && !it->m_bSwitchingTask){
        // Combined stimulus < thrA --> Do task B
        bPerformTaskB = true;
    }
    
    if(bPerformTaskA){
        if(it->m_unCurrentTask == 2 && m_unSwitchingCost > 0){
            it->m_bSwitchingTask = true; // Previous time agent was doing the other task. Switching!
            c_actions_this_timestep.m_unIdle++;
        }
        else{
            c_actions_this_timestep.m_unTaskA++;
            if(it->m_unCurrentTask == 1){
                it->m_unNonSwitchingTaskCounter ++; // Previous time agent was working, it did same task.
            }
            it->m_unCurrentTask = 1; // Set as Task A
            it->m_unTotalActionsPerAgent++;
            m_fStimulusTaskA -= m_fAlphaStimulusDecreaseTaskA;
        }
    }
    else if (bPerformTaskB){
        if(it->m_unCurrentTask == 1 && m_unSwitchingCost > 0){
            it->m_bSwitchingTask = true; // Previous time agent was doing the other task. Switching!
            c_actions_this_timestep.m_unIdle++;
        }
        else{
            c_actions_this_timestep.m_unTaskB++;
            if(it->m_unCurrentTask == 2){
                it->m_unNonSwitchingTaskCounter ++; // Previous time agent was working, it did same task.
            }
            it->m_unCurrentTask = 2; // Set as Task B
            it->m_unTotalActionsPerAgent++;
            m_fStimulusTaskB -= m_fAlphaStimulusDecreaseTaskB;
        }
    }
    else if(!it->m_bSwitchingTask){
        LOGERR << "[ERROR] One threshold model should have agents either do A or B. Idle allowed only when switching tasks." << std::endl;
        exit(-1);
    }
    
    if(it->m_bSwitchingTask){
        it->m_unSwitchingTimestep++;
    }
}

/****************************************/
/****************************************/

void CSimulator::StepTwoThresholdsDuarte(std::vector<Agent>::iterator it, Actions& c_actions_this_timestep){
    bool bEligibleTaskA = false;
    bool bEligibleTaskB = false;
    
    bool bPerformTaskA = false;
    bool bPerformTaskB = false;
    
    Real fPerceivedStimulusTaskA = m_fStimulusTaskA + m_pcRNG->Gaussian(1.0,0.0);
    Real fPerceivedStimulusTaskB = m_fStimulusTaskB + m_pcRNG->Gaussian(1.0,0.0);
    
    if(fPerceivedStimulusTaskA > it->m_fThresholdTaskA && !it->m_bSwitchingTask){
        bEligibleTaskA = true;
    }
    if(fPerceivedStimulusTaskB > it->m_fThresholdTaskB && !it->m_bSwitchingTask){
        bEligibleTaskB = true;
    }
    if(bEligibleTaskA && bEligibleTaskB){
        //LOGERR << "Both A and B because sA " << fPerceivedStimulusTaskA << " sB " << fPerceivedStimulusTaskB << " tA " << it->m_fThresholdTaskA << " tB " << it->m_fThresholdTaskB <<endl;
        Real fTaskChoiceRandom = m_pcRNG->Uniform(CRange<Real>(0.0,1.0));
        if(fTaskChoiceRandom < 0.5){
            bPerformTaskA = true;
            bPerformTaskB = false;
        }
        else{
            bPerformTaskA = false;
            bPerformTaskB = true;
        }
    }
    else if(bEligibleTaskA){
        bPerformTaskA = true;
        bPerformTaskB = false;
    }
    else if(bEligibleTaskB){
        bPerformTaskA = false;
        bPerformTaskB = true;
    }

    if(bPerformTaskA){
        //LOG << "Chose to do A " << std::endl;
        if(it->m_unCurrentTask == 2 && m_unSwitchingCost > 0){
            it->m_bSwitchingTask = true; // Previous time agent was doing the other task. Switching!
            c_actions_this_timestep.m_unIdle++;
            //it->m_unCurrentTask = 0; // Set as IDLE
        }
        else{
            c_actions_this_timestep.m_unTaskA++;
            if(it->m_unCurrentTask == 1){
                it->m_unNonSwitchingTaskCounter ++; // Previous time agent was working, it did same task.
            }
            it->m_unCurrentTask = 1; // Set as Task A
            it->m_unTotalActionsPerAgent++;
            m_fStimulusTaskA -= m_fAlphaStimulusDecreaseTaskA;
            //LOG << "Actually did A " << std::endl << std::endl;
            //LOG << "Total actions agent " << it->m_unTotalActionsPerAgent << std::endl;
            //LOG << "Times not switched agent " << it->m_unNonSwitchingTaskCounter << std::endl << std::endl;
        }
    }
    else if (bPerformTaskB){
        //LOG << "Chose to do B " << std::endl;
        
        if(it->m_unCurrentTask == 1 && m_unSwitchingCost > 0){
            it->m_bSwitchingTask = true; // Previous time agent was doing the other task. Switching!
            c_actions_this_timestep.m_unIdle++;
            //it->m_unCurrentTask = 0; // Set as IDLE
        }
        else{
            c_actions_this_timestep.m_unTaskB++;
            if(it->m_unCurrentTask == 2){
                it->m_unNonSwitchingTaskCounter ++; // Previous time agent was working, it did same task.
            }
            it->m_unCurrentTask = 2; // Set as Task B
            it->m_unTotalActionsPerAgent++;
            m_fStimulusTaskB -= m_fAlphaStimulusDecreaseTaskB;
            //LOG << "Total actions agent " << it->m_unTotalActionsPerAgent << std::endl;
            //LOG << "Times not switched agent " << it->m_unNonSwitchingTaskCounter << std::endl << std::endl;
            //LOG << "Actually did B " << std::endl << std::endl;
        }
    }
    else if (!bPerformTaskA && !bPerformTaskB){
        //LOG << "Entering here freaking fuck " << std::endl;
        c_actions_this_timestep.m_unIdle++;
        //it->m_unCurrentTask = 0; // Set as IDLE
    }
    
    if(it->m_bSwitchingTask){
        it->m_unSwitchingTimestep++;
    }
    
    //LOG << "Agent " << " has tA "<< it->m_fThresholdTaskA << " tB " << it->m_fThresholdTaskB << " and is doing " << it->m_unCurrentTask << std::endl;
    
}

/****************************************/
/****************************************/

void CSimulator::Execute(){
    UInt32 uTimestep = 0;
    
    while(uTimestep < m_unTotalDurationTimesteps){
        
        Actions actionsThisTimestep = {};
        std::random_shuffle(agents.begin(), agents.end());
        for(std::vector<Agent>::iterator it = agents.begin(); it != agents.end(); ++it) {
            
            if(it->m_unSwitchingTimestep >= m_unSwitchingCost && it->m_bSwitchingTask){
                it->m_bSwitchingTask = false;
                it->m_unSwitchingTimestep = 0;
                // The following code forces the agent to do the opposite task it was doing (switching). This must be done for one timestep.
                if(it->m_unCurrentTask == 2){ // Agent chose before going to sleep to switch from B to A
                    actionsThisTimestep.m_unTaskA++;
                    it->m_unCurrentTask = 1; // Set as Task A
                    it->m_unTotalActionsPerAgent++;
                    m_fStimulusTaskA -= m_fAlphaStimulusDecreaseTaskA;
                    //LOG << "[NEW] Total actions agent " << it->m_unTotalActionsPerAgent << std::endl;
                    //LOG << "[NEW] Times not switched agent " << it->m_unNonSwitchingTaskCounter << std::endl << std::endl;
                }
                else if(it->m_unCurrentTask == 1){ // Agent chose before going to sleep to switch from A to B
                    actionsThisTimestep.m_unTaskB++;
                    it->m_unCurrentTask = 2; // Set as Task B
                    it->m_unTotalActionsPerAgent++;
                    m_fStimulusTaskB -= m_fAlphaStimulusDecreaseTaskB;
                    //LOG << "[NEW] Total actions agent " << it->m_unTotalActionsPerAgent << std::endl;
                    //LOG << "[NEW] Times not switched agent " << it->m_unNonSwitchingTaskCounter << std::endl << std::endl;
                }
                continue; // Skip the rest as action has already been done
            }
            
            if(m_sModelType.compare(MODEL_TYPE_TWO_THRESHOLDS_DUARTE) == 0){
                StepTwoThresholdsDuarte(it,actionsThisTimestep);
            }
            else if(m_sModelType.compare(MODEL_TYPE_SINGLE_THRESHOLD) == 0){
                StepOneThreshold(it,actionsThisTimestep);
            }
            else{
                LOGERR << "[ERROR] Specified model " << m_sModelType << " unknown. Exiting." << std::endl;
                exit(-1);
            }
            if(m_fStimulusTaskA < 0.0){
                m_fStimulusTaskA = 0.0;
            }
            if(m_fStimulusTaskB < 0.0){
                m_fStimulusTaskB = 0.0;
            }
            
        }
        
        actionsOverTime.push_back(actionsThisTimestep);
        m_fStimulusTaskA += m_fDeltaStimulusIncreaseTaskA;
        m_fStimulusTaskB += m_fDeltaStimulusIncreaseTaskB;
        
        //m_fStimulusTaskA -= m_fAlphaStimulusDecreaseTaskA * (Real) actionsThisTimestep.m_unTaskA;
        //m_fStimulusTaskB -= m_fAlphaStimulusDecreaseTaskB * (Real) actionsThisTimestep.m_unTaskB;
        
        WriteResults(uTimestep);
        
        uTimestep++;
    }
    //LOGERR << endl;
}

/****************************************/
/****************************************/

CObjectives CSimulator::ComputePerformanceInExperiment(){
    CObjectives cResult;
    Real fFitnessWeakAri = 0.0;
    Real fFitnessStrongAri = 0.0;
    Real fFitnessWeakGeo = 0.0;
    Real fFitnessStrongGeo = 0.0;
    m_fFitnessWeakOverall = 0.0;
    m_fFitnessStrongOverall = 0.0;
    m_fFitnessStrongOverallLogActs = 0.0;
    
    UInt32 uTimestepsToEvaluate = m_unTotalDurationTimesteps - m_unTimestepsToSkipForFitness;
    
    double mantissaFit1 = 1.0;
    long long expFit1 = 0;
    double mantissaFit2 = 1.0;
    long long expFit2 = 0;
    double invN = 1.0 / uTimestepsToEvaluate;
    
    for(UInt32 i = m_unTotalDurationTimesteps - uTimestepsToEvaluate; i < m_unTotalDurationTimesteps; ++i){
        Real fFitnessWeakThisTimestep = ComputeFitnessWeak(actionsOverTime[i].m_unTaskA,actionsOverTime[i].m_unTaskB);
        
        fFitnessWeakAri += fFitnessWeakThisTimestep;
        
        // Method to deal with zeros in geometric mean: convert to 0.0 (method 3 in http://www.wwdmag.com/channel/casestudies/handling-zeros-geometric-mean-calculation)
        // if(abs(fFitnessWeakThisTimestep) < 0.001){
        //     fFitnessWeakThisTimestep = 1.0;
        // }
        
        int iFit1;
        //double f1 = std::frexp(fFitnessWeakThisTimestep,&iFit1);
        // Method to deal with zeros in geometric mean: add 1.0 always and remove 1.0 from results (method 1 in http://www.wwdmag.com/channel/casestudies/handling-zeros-geometric-mean-calculation)
        double f1 = std::frexp(fFitnessWeakThisTimestep + 0.000001,&iFit1);
        mantissaFit1*=f1;
        expFit1+=iFit1;
    
        
        Real fTotalActions    = ((Real)actionsOverTime[i].m_unTaskA + (Real)actionsOverTime[i].m_unTaskB);
        Real fProportionTaskA = 0.0;
        Real fProportionTaskB = 0.0;
        if( fTotalActions > 0.0){
            fProportionTaskA = (Real)actionsOverTime[i].m_unTaskA / fTotalActions;
            fProportionTaskB = (Real)actionsOverTime[i].m_unTaskB / fTotalActions;
        }
        
        Real fFitnessStrongThisTimestep = ComputeFitnessStrong(actionsOverTime[i].m_unTaskA,actionsOverTime[i].m_unTaskB, false);
        
        fFitnessStrongAri += fFitnessStrongThisTimestep;
        
        // Method to deal with zeros in geometric mean: convert to 0.0 (method 3 in http://www.wwdmag.com/channel/casestudies/handling-zeros-geometric-mean-calculation)
        // if(abs(fFitnessStrongThisTimestep) < 0.001){
        //     fFitnessStrongThisTimestep = 1.0;
        // }
        int iFit2;
        //double f2 = std::frexp(fFitnessStrongThisTimestep,&iFit2);
        // Method to deal with zeros in geometric mean: add 1.0 always and remove 1.0 from results (method 1 in http://www.wwdmag.com/channel/casestudies/handling-zeros-geometric-mean-calculation)
        double f2 = std::frexp(fFitnessStrongThisTimestep + 0.000001,&iFit2);
        mantissaFit2*=f2;
        expFit2+=iFit2;    
    
    }
    
    
    fFitnessWeakAri /= uTimestepsToEvaluate;
    fFitnessStrongAri /= uTimestepsToEvaluate;

    fFitnessWeakGeo = std::pow( std::numeric_limits<double>::radix,expFit1 * invN) * std::pow(mantissaFit1,invN);
    fFitnessStrongGeo = std::pow( std::numeric_limits<double>::radix,expFit2 * invN) * std::pow(mantissaFit2,invN);
    
    // Method to deal with zeros in geometric mean: add 1.0 always and remove 1.0 from results (method 1 in http://www.wwdmag.com/channel/casestudies/handling-zeros-geometric-mean-calculation)
    fFitnessWeakGeo -= 0.000001;
    fFitnessStrongGeo -= 0.000001;

    UpdateFitness3and4OverallActions(m_unTotalDurationTimesteps - uTimestepsToEvaluate, m_unTotalDurationTimesteps);
    Real fSpecialization = ComputeSpecializationUpToTimestep(m_unTotalDurationTimesteps);
    
    // LOGERR << "Fit1: " << fFitnessWeak;
    // LOGERR << " fit2: " << fFitnessStrong;
    // LOGERR << " q_dash: " << fAveragedProbabilityQPerformingSameTask;
    // LOGERR << " D: " << fSpecialization;
    // LOGERR << " p1: " << fOverallProportionTaskA;
    // LOGERR << " p2: " << fOverallProportionTaskB;
    // LOGERR << " A: " << fOverallTotalActions;
    // LOGERR << std::endl;
    
    if(m_sFitnessToUse.compare(FITNESS_TYPE_WEAK) == 0){
        if(m_sFitnessAveraging.compare(FITNESS_AVERAGING_ARITHMETIC) == 0){
            cResult.Insert(fFitnessWeakAri);
        }
        else if (m_sFitnessAveraging.compare(FITNESS_AVERAGING_GEOMETRIC) == 0){
            cResult.Insert(fFitnessWeakGeo);
        }
    }
    if(m_sFitnessToUse.compare(FITNESS_TYPE_STRONG) == 0){
        if(m_sFitnessAveraging.compare(FITNESS_AVERAGING_ARITHMETIC) == 0){
            cResult.Insert(fFitnessStrongAri);
        }
        else if (m_sFitnessAveraging.compare(FITNESS_AVERAGING_GEOMETRIC) == 0){
            cResult.Insert(fFitnessStrongGeo);
        }
    }
    if(m_sFitnessToUse.compare(FITNESS_TYPE_WEAK_OVERALL) == 0){
        cResult.Insert(m_fFitnessWeakOverall);
    }
    if(m_sFitnessToUse.compare(FITNESS_TYPE_STRONG_OVERALL) == 0){
        cResult.Insert(m_fFitnessStrongOverall);
    }
    if(m_sFitnessToUse.compare(FITNESS_TYPE_STRONG_OVERALL_LOG_ACTS) == 0){
        cResult.Insert(m_fFitnessStrongOverallLogActs);
    }
    
    cResult.Insert(fFitnessWeakAri);
    cResult.Insert(fFitnessStrongAri);
    // TODO: insert here the other fitnesses but remember to update the xml file
    cResult.Insert(fSpecialization);
    cResult.Insert(m_fOverallProportionTaskA);
    cResult.Insert(m_fOverallProportionTaskB);
    cResult.Insert(m_fOverallTotalActions);
    
    if(m_bWriteResults){
        ostringstream endRunFilename;
        endRunFilename.fill( '0' );
        endRunFilename.str("");
        endRunFilename << m_sEndrunResultsBasename;
        //endRunFilename << "_thr";
        //endRunFilename << m_fMonomorphicGenotype;
        endRunFilename << ".txt";
        
        outputResultsEndrun.open( endRunFilename.str().c_str(), ios::out );
        outputResultsEndrun << "FitnessWeakAri\tFitnessStrongAri\tFitnessWeakGeo\tFitnessStrongGeo\tFitnessWeakOverall\tFitnessStrongOverall\tFitnessStrongOverallLogActs\tSpec\tpA\tpB\tA" << std::endl;
        outputResultsEndrun << fFitnessWeakAri                  << "\t";
        outputResultsEndrun << fFitnessStrongAri                << "\t";
        outputResultsEndrun << fFitnessWeakGeo                  << "\t";
        outputResultsEndrun << fFitnessStrongGeo                << "\t";
        outputResultsEndrun << m_fFitnessWeakOverall            << "\t";
        outputResultsEndrun << m_fFitnessStrongOverall          << "\t";
        outputResultsEndrun << m_fFitnessStrongOverallLogActs   << "\t";
        outputResultsEndrun << fSpecialization                  << "\t";
        outputResultsEndrun << m_fOverallProportionTaskA        << "\t"; 
        outputResultsEndrun << m_fOverallProportionTaskB        << "\t";
        outputResultsEndrun << m_fOverallTotalActions           << std::endl;
        outputResultsEndrun.close();
    }
    
    return cResult;
}

/****************************************/
/****************************************/

void CSimulator::Reset(){
    m_fStimulusTaskA = 0.0;
    m_fStimulusTaskB = 0.0;
    
    m_unOverallTotalActionsA = 0;
    m_unOverallTotalActionsB = 0;
    m_fOverallProportionTaskA = 0.0;
    m_fOverallProportionTaskB = 0.0;
    m_fOverallTotalActions = 0.0;
    
    agents.clear();
    actionsOverTime.clear();
}


/****************************************/
/****************************************/

void CSimulator::Destroy(){
    outputResults.close();
}

/****************************************/
/****************************************/
