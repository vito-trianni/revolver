#include "simulator.h"

const string CONFIGURATION_COLONY_SIZE                  = "colony_size";
const string CONFIGURATION_TOTAL_DURATION               = "total_duration_timesteps";

const string CONFIGURATION_DELTA_STIMULUS_INCREASE_A    = "delta_stimulus_increase_A";
const string CONFIGURATION_DELTA_STIMULUS_INCREASE_B    = "delta_stimulus_increase_B";

const string CONFIGURATION_ALPHA_STIMULUS_DECREASE_A    = "alpha_stimulus_decrease_A";
const string CONFIGURATION_ALPHA_STIMULUS_DECREASE_B    = "alpha_stimulus_decrease_B";

const string CONFIGURATION_SWITCHING_COST               = "switching_cost";
const string CONFIGURATION_BETA_FITNESS_WEIGHT_FACTOR   = "beta_fitness_weight_factor";
const string CONFIGURATION_SIGMA_FITNESS2               = "sigma_fitness2";

const string CONFIGURATION_WRITE_RESULTS                = "write_results";
const string CONFIGURATION_RESULTS_FILENAME             = "results_filename";
const string CONFIGURATION_ENDRUN_RESULTS_BASENAME      = "results_endrune_basename";

const string CONFIGURATION_FITNESS_TO_USE               = "fitness_to_use";

const string FITNESS_TYPE_WEAK                          = "weak";
const string FITNESS_TYPE_STRONG                        = "strong";

/****************************************/
/****************************************/

CSimulator::CSimulator():
    m_unRandomSeed(0),
    m_unTrialNumber(0),
    m_unColonySize(0),
    m_unTotalDurationTimesteps(0),
    m_fDeltaStimulusIncreaseTaskA(0.0),
    m_fDeltaStimulusIncreaseTaskB(0.0),
    m_fAlphaStimulusDecreaseTaskA(0.0),
    m_fAlphaStimulusDecreaseTaskB(0.0),
    m_unSwitchingCost(0),
    m_fBetaFitnessWeightFactor(0.0),
    m_fSigmaFitness2(0.0),
    m_fFitness(0.0),
    m_fStimulusTaskA(0.0),
    m_fStimulusTaskB(0.0),
    m_bWriteResults(false),
    m_sResultsFilename(""),
    m_sEndrunResultsBasename(""),
    m_sExperimentFilename(""),
    m_sFitnessToUse(""),
    m_fMonomorphicGenotype(0.0)
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
    
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_COLONY_SIZE, m_unColonySize );
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_TOTAL_DURATION, m_unTotalDurationTimesteps );
    
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_DELTA_STIMULUS_INCREASE_A, m_fDeltaStimulusIncreaseTaskA );
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_DELTA_STIMULUS_INCREASE_B, m_fDeltaStimulusIncreaseTaskB );
    
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_ALPHA_STIMULUS_DECREASE_A, m_fAlphaStimulusDecreaseTaskA );
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_ALPHA_STIMULUS_DECREASE_B, m_fAlphaStimulusDecreaseTaskB );
    
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_SWITCHING_COST, m_unSwitchingCost );
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_BETA_FITNESS_WEIGHT_FACTOR, m_fBetaFitnessWeightFactor );
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_SIGMA_FITNESS2, m_fSigmaFitness2 );
    
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_WRITE_RESULTS, m_bWriteResults );
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_RESULTS_FILENAME, m_sResultsFilename);
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_ENDRUN_RESULTS_BASENAME, m_sEndrunResultsBasename);
    
    GetNodeAttribute(t_simulator_configuration, CONFIGURATION_FITNESS_TO_USE, m_sFitnessToUse);
    
    if(m_bWriteResults){
        ostringstream filename;
        filename.fill( '0' );
        filename.str("");
        filename << m_sResultsFilename;
        outputResults.open( filename.str().c_str(), ios::out );
        outputResults << "Timestep\tStimA\tStimB\tRobotsA\tRobotsB\tRobotsIDLE\tFitness" << std::endl;

    }
}

/****************************************/
/****************************************/

void CSimulator::SetControlParameters(CEvaluationConfig* e_config){
    for(UInt32 i = 0 ; i < m_unColonySize ; ++i){
        Agent cNewAgent = {};
        CGenotype cOffSpringGenotype = e_config->GetOffspringGenotype(m_pcRNG);
        cNewAgent.m_fThresholdTaskA = cOffSpringGenotype.GetValues()[0];
        cNewAgent.m_fThresholdTaskB = cOffSpringGenotype.GetValues()[1];
        cNewAgent.m_unCurrentTask = 0; // IDLE
        cNewAgent.m_unSwitchingTimestep = 0;
        cNewAgent.m_unNonSwitchingTaskCounter = 0;
        cNewAgent.m_unTotalActionsPerAgent = 0;
        agents.push_back(cNewAgent);
    }
}


/****************************************/
/****************************************/

void CSimulator::WriteResults(UInt32 u_timestep){
    
    if(m_bWriteResults){
  
        Real fFitness = (pow((Real)actionsOverTime[u_timestep].m_unTaskA,m_fBetaFitnessWeightFactor) + pow((Real)actionsOverTime[u_timestep].m_unTaskB,1.0 - m_fBetaFitnessWeightFactor));
        
        outputResults << u_timestep                             << "\t";
        outputResults << m_fStimulusTaskA                       << "\t";
        outputResults << m_fStimulusTaskB                       << "\t";
        outputResults << actionsOverTime[u_timestep].m_unTaskA  << "\t";
        outputResults << actionsOverTime[u_timestep].m_unTaskB  << "\t";
        outputResults << actionsOverTime[u_timestep].m_unIdle   << "\t";
        outputResults << fFitness                               << std::endl;
    }
}

/****************************************/
/****************************************/

void CSimulator::Execute(){
    UInt32 uTimestep = 0;
    
    while(uTimestep < m_unTotalDurationTimesteps){
        
        Actions actionsThisTimestep = {};
        std::random_shuffle(agents.begin(), agents.end());
        for(std::vector<Agent>::iterator it = agents.begin(); it != agents.end(); ++it) {
            bool bPerformTaskA = false;
            bool bPerformTaskB = false;
            
            Real fPerceivedStimulusTaskA = m_fStimulusTaskA + m_pcRNG->Gaussian(1.0,0.0);
            Real fPerceivedStimulusTaskB = m_fStimulusTaskB + m_pcRNG->Gaussian(1.0,0.0);
            // Real fPerceivedStimulusTaskA = m_fStimulusTaskA;
            // Real fPerceivedStimulusTaskB = m_fStimulusTaskB;
            if(fPerceivedStimulusTaskA > it->m_fThresholdTaskA){
                bPerformTaskA = true;
            }
            if(fPerceivedStimulusTaskB > it->m_fThresholdTaskB){
                bPerformTaskB = true;
            }
            if(bPerformTaskA && bPerformTaskB){
                //LOGERR << "Both A and B because sA " << fPerceivedStimulusTaskA << " sB " << fPerceivedStimulusTaskB << " tA " << it->m_fThresholdTaskA << " tB " << it->m_fThresholdTaskB <<endl;
                Real fTaskChoiceRandom = m_pcRNG->Uniform(CRange<Real>(0.0,1.0));
                if(fTaskChoiceRandom < 0.5){
                    actionsThisTimestep.m_unTaskA++;
                    if(it->m_unCurrentTask == 1){
                        it->m_unNonSwitchingTaskCounter ++; // Previous time agent was working, it did same task.
                    }
                    it->m_unCurrentTask = 1; // Set as Task A
                    it->m_unTotalActionsPerAgent++;
                    m_fStimulusTaskA -= m_fAlphaStimulusDecreaseTaskA;
                }
                else{
                    actionsThisTimestep.m_unTaskB++;
                    if(it->m_unCurrentTask == 2){
                        it->m_unNonSwitchingTaskCounter ++; // Previous time agent was working, it did same task.
                    }
                    it->m_unCurrentTask = 2; // Set as Task B
                    it->m_unTotalActionsPerAgent++;
                    m_fStimulusTaskB -= m_fAlphaStimulusDecreaseTaskB;
                }
            }
            else if(bPerformTaskA){
                actionsThisTimestep.m_unTaskA++;
                if(it->m_unCurrentTask == 1){
                    it->m_unNonSwitchingTaskCounter ++; // Previous time agent was working, it did same task.
                }
                it->m_unCurrentTask = 1; // Set as Task A
                it->m_unTotalActionsPerAgent++;
                m_fStimulusTaskA -= m_fAlphaStimulusDecreaseTaskA;
            }
            else if(bPerformTaskB){
                actionsThisTimestep.m_unTaskB++;
                if(it->m_unCurrentTask == 2){
                    it->m_unNonSwitchingTaskCounter ++; // Previous time agent was working, it did same task.
                }
                it->m_unCurrentTask = 2; // Set as Task B
                it->m_unTotalActionsPerAgent++;
                m_fStimulusTaskB -= m_fAlphaStimulusDecreaseTaskB;
            }
            else{
                actionsThisTimestep.m_unIdle++;
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
    Real fFitness1 = 0.0;
    Real fFitness2 = 0.0;
    UInt32 uOverallTotalActionsA = 0;
    UInt32 uOverallTotalActionsB = 0;
    Real fOverallProportionTaskA = 0.0;
    Real fOverallProportionTaskB = 0.0;
    Real fOverallTotalActions = 0.0;
    Real fSpecialization = 0.0;
    
    UInt32 uTimestepsToEvaluate = 100;
    for(UInt32 i = m_unTotalDurationTimesteps - uTimestepsToEvaluate; i < m_unTotalDurationTimesteps; ++i){
        
        uOverallTotalActionsA += actionsOverTime[i].m_unTaskA;
        uOverallTotalActionsB += actionsOverTime[i].m_unTaskB;
        fOverallTotalActions += (Real) actionsOverTime[i].m_unTaskA;
        fOverallTotalActions += (Real) actionsOverTime[i].m_unTaskB;
        
        fFitness1 += (pow((Real)actionsOverTime[i].m_unTaskA,m_fBetaFitnessWeightFactor) + pow((Real)actionsOverTime[i].m_unTaskB,1.0 - m_fBetaFitnessWeightFactor));
        
        Real fTotalActions    = ((Real)actionsOverTime[i].m_unTaskA + (Real)actionsOverTime[i].m_unTaskB);
        Real fProportionTaskA = 0.0;
        Real fProportionTaskB = 0.0;
        if( fTotalActions > 0.0){
            fProportionTaskA = (Real)actionsOverTime[i].m_unTaskA / fTotalActions;
            fProportionTaskB = (Real)actionsOverTime[i].m_unTaskB / fTotalActions;
        }
        
        //LOGERR << " p1: " << fProportionTaskA;
        //LOGERR << " p2: " << fProportionTaskB;
        //LOGERR << " A: " << fTotalActions;
        
        Real fFitness2ThisTimestep = fTotalActions * exp(- ((fProportionTaskA - m_fBetaFitnessWeightFactor) * (fProportionTaskA - m_fBetaFitnessWeightFactor) / (2.0 * m_fSigmaFitness2 * m_fSigmaFitness2 )));
        
        fFitness2 += fFitness2ThisTimestep;
        
    }
    
    fFitness1 /= uTimestepsToEvaluate;
    fFitness2 /= uTimestepsToEvaluate;
    
    fOverallProportionTaskA = (Real) uOverallTotalActionsA / fOverallTotalActions;
    fOverallProportionTaskB = (Real) uOverallTotalActionsB / fOverallTotalActions;
    
    Real fAveragedProbabilityQPerformingSameTask = 0.0;
    for(UInt32 i = 0 ; i < m_unColonySize ; ++i) {
        fAveragedProbabilityQPerformingSameTask += (Real) agents[i].m_unNonSwitchingTaskCounter / (Real) agents[i].m_unTotalActionsPerAgent;
    }
    fAveragedProbabilityQPerformingSameTask /= m_unColonySize;
    fSpecialization = (fAveragedProbabilityQPerformingSameTask / ( (fOverallProportionTaskA*fOverallProportionTaskA)+(fOverallProportionTaskB*fOverallProportionTaskB) )) - 1.0;
    
    // LOGERR << "Fit1: " << fFitness1;
    // LOGERR << " fit2: " << fFitness2;
    // LOGERR << " q_dash: " << fAveragedProbabilityQPerformingSameTask;
    // LOGERR << " D: " << fSpecialization;
    // LOGERR << " p1: " << fOverallProportionTaskA;
    // LOGERR << " p2: " << fOverallProportionTaskB;
    // LOGERR << " A: " << fOverallTotalActions;
    // LOGERR << std::endl;
    
    if(m_sFitnessToUse.compare(FITNESS_TYPE_WEAK) == 0){
        cResult.Insert(fFitness1);
    }
    if(m_sFitnessToUse.compare(FITNESS_TYPE_STRONG) == 0){
        cResult.Insert(fFitness2);
    }
    
    cResult.Insert(fFitness1);
    cResult.Insert(fFitness2);
    cResult.Insert(fSpecialization);
    cResult.Insert(fOverallProportionTaskA);
    cResult.Insert(fOverallProportionTaskB);
    cResult.Insert(fOverallTotalActions);
    
    if(m_bWriteResults){
        ostringstream endRunFilename;
        endRunFilename.fill( '0' );
        endRunFilename.str("");
        endRunFilename << m_sEndrunResultsBasename;
        endRunFilename << "_thr";
        endRunFilename << m_fMonomorphicGenotype;
        endRunFilename << ".txt";
        
        outputResultsEndrun.open( endRunFilename.str().c_str(), ios::out );
        outputResultsEndrun << "Fitness1\tFitness2\tSpec\tpA\tpB\tA" << std::endl;
        outputResultsEndrun << fFitness1                << "\t";
        outputResultsEndrun << fFitness2                << "\t";
        outputResultsEndrun << fSpecialization          << "\t";
        outputResultsEndrun << fOverallProportionTaskA  << "\t"; 
        outputResultsEndrun << fOverallProportionTaskB  << "\t";
        outputResultsEndrun << fOverallTotalActions     << std::endl;
        outputResultsEndrun.close();
    }
    
    return cResult;
}

/****************************************/
/****************************************/

void CSimulator::Reset(){
    m_fStimulusTaskA = 0.0;
    m_fStimulusTaskB = 0.0;
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