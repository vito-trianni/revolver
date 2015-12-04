#include "evaluation_strategy.h"

const string CONFIGURATION_EVOLUTION_NUMSAMPLES      = "num_samples";


/****************************************/
/****************************************/

CEvaluationStrategy::CEvaluationStrategy() :
   m_pcRNG( NULL),
   m_unNumSamples( 0 ),
   m_unNumTeams(0),
   m_unTeamSize(0),
   m_unNumControllers(0)
{}


/****************************************/
/****************************************/

CEvaluationStrategy::~CEvaluationStrategy()
{}


/****************************************/
/****************************************/

void CEvaluationStrategy::Init(TConfigurationNode& t_tree) {
   ////////////////////////////////////////////////////////////////////////////////
   // number of samples
   ////////////////////////////////////////////////////////////////////////////////
   GetNodeAttribute(t_tree, CONFIGURATION_EVOLUTION_NUMSAMPLES, m_unNumSamples );

}
