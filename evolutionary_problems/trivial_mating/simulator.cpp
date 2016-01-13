#include "simulator.h"

const string CONFIGURATION_COLONY_SIZE                  = "colony_size";
const string CONFIGURATION_TOTAL_DURATION               = "total_duration_timesteps";

const string CONFIGURATION_DELTA_STIMULUS_INCREASE_A    = "delta_stimulus_increase_A";
const string CONFIGURATION_DELTA_STIMULUS_INCREASE_B    = "delta_stimulus_increase_B";

const string CONFIGURATION_ALPHA_STIMULUS_DECREASE_A    = "alpha_stimulus_decrease_A";
const string CONFIGURATION_ALPHA_STIMULUS_DECREASE_B    = "alpha_stimulus_decrease_B";

const string CONFIGURATION_SWITCHING_COST               = "switching_cost";
const string CONFIGURATION_BETA_FITNESS_WEIGHT_FACTOR   = "beta_fitness_weight_factor";

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
    m_fFitness(0.0),
    m_fStimulusTaskA(0.0),
    m_fStimulusTaskB(0.0),
    m_sExperimentFilename("")
{
    if( !CRandom::ExistsCategory( "simulator" ) ) {
      CRandom::CreateCategory(  "simulator", 1 );
    }
    m_pcRNG = CRandom::CreateRNG(  "simulator" );   
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
    
}

/****************************************/
/****************************************/

void CSimulator::SetControlParameters(CEvaluationConfig* e_config){
    for(UInt32 i = 0 ; i < m_unColonySize ; ++i){
        Agent cNewAgent = {};
        CGenotype cOffSpringGenotype = e_config->GetOffspringGenotype(m_pcRNG);
        cNewAgent.m_fThresholdTaskA = cOffSpringGenotype.GetValues()[0];
        cNewAgent.m_fThresholdTaskB = cOffSpringGenotype.GetValues()[1];
        cNewAgent.m_unSwitchingTimestep = 0;
        agents.push_back(cNewAgent);
    }
}


/****************************************/
/****************************************/

void CSimulator::Execute(){
    UInt32 uTimestep = 0;
    
    //LOGERR << "Timestep\tStimA\tStimB\tRobotsA\tRobotsB\tRobotsIDLE\tFitness" << std::endl;
    
    while(uTimestep < m_unTotalDurationTimesteps){
        Actions actionsThisTimestep = {};
        std::random_shuffle(agents.begin(), agents.end());
        for(std::vector<Agent>::iterator it = agents.begin(); it != agents.end(); ++it) {
            bool bPerformTaskA = false;
            bool bPerformTaskB = false;
            // Real fPerceivedStimulusTaskA = m_fStimulusTaskA + m_pcRNG->Gaussian(1.0,0.0);
            // Real fPerceivedStimulusTaskB = m_fStimulusTaskB + m_pcRNG->Gaussian(1.0,0.0);
            Real fPerceivedStimulusTaskA = m_fStimulusTaskA;
            Real fPerceivedStimulusTaskB = m_fStimulusTaskB;
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
                    m_fStimulusTaskA -= m_fAlphaStimulusDecreaseTaskA;
                }
                else{
                    actionsThisTimestep.m_unTaskB++;
                    m_fStimulusTaskB -= m_fAlphaStimulusDecreaseTaskB;
                }
            }
            else if(bPerformTaskA){
                actionsThisTimestep.m_unTaskA++;
                m_fStimulusTaskA -= m_fAlphaStimulusDecreaseTaskA;
            }
            else if(bPerformTaskB){
                actionsThisTimestep.m_unTaskB++;
                m_fStimulusTaskB -= m_fAlphaStimulusDecreaseTaskB;
            }
            else{
                actionsThisTimestep.m_unIdle++;
            }
            
        }
        
        Real fFitness = (pow((Real)actionsThisTimestep.m_unTaskA,m_fBetaFitnessWeightFactor) + pow((Real)actionsThisTimestep.m_unTaskB,1.0 - m_fBetaFitnessWeightFactor));
        
        // LOGERR << uTimestep << "\t";
        // LOGERR << m_fStimulusTaskA << "\t";
        // LOGERR << m_fStimulusTaskB << "\t";
        // LOGERR << actionsThisTimestep.m_unTaskA << "\t";
        // LOGERR << actionsThisTimestep.m_unTaskB << "\t";
        // LOGERR << actionsThisTimestep.m_unIdle  << "\t";
        // LOGERR << fFitness  << std::endl;
        
        actionsOverTime.push_back(actionsThisTimestep);
        m_fStimulusTaskA += m_fDeltaStimulusIncreaseTaskA;
        m_fStimulusTaskB += m_fDeltaStimulusIncreaseTaskB;
        
        //m_fStimulusTaskA -= m_fAlphaStimulusDecreaseTaskA * (Real) actionsThisTimestep.m_unTaskA;
        //m_fStimulusTaskB -= m_fAlphaStimulusDecreaseTaskB * (Real) actionsThisTimestep.m_unTaskB;
        
        uTimestep++;
    }
    //LOGERR << endl;
}

/****************************************/
/****************************************/

Real CSimulator::ComputePerformanceInExperiment(){
    Real fFitness = 0.0;
    
    UInt32 uTimestepsToEvaluate = 0;
    for(UInt32 i = m_unTotalDurationTimesteps - uTimestepsToEvaluate; i < m_unTotalDurationTimesteps; ++i){
        fFitness += (pow((Real)actionsOverTime[i].m_unTaskA,m_fBetaFitnessWeightFactor) + pow((Real)actionsOverTime[i].m_unTaskB,1.0 - m_fBetaFitnessWeightFactor));
    }
    fFitness /= uTimestepsToEvaluate;
    
    return fFitness;
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
}

/****************************************/
/****************************************/