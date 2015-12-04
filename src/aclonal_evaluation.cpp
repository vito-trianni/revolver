#include "aclonal_evaluation.h"

const string CONFIGURATION_EVOLUTION_TEAM_SIZE        = "team_size";
const string CONFIGURATION_EVOLUTION_SAMPLES_PER_TEAM = "samples_per_team";

/****************************************/
/****************************************/

CAclonalEvaluation::CAclonalEvaluation() :
   CEvaluationStrategy()
{
}


/****************************************/
/****************************************/

CAclonalEvaluation::~CAclonalEvaluation() {
};


/****************************************/
/****************************************/

void CAclonalEvaluation::Init(TConfigurationNode& t_tree) {
   CEvaluationStrategy::Init(t_tree);

   UInt32 un_samples_per_team = 0;
   GetNodeAttribute(t_tree, CONFIGURATION_EVOLUTION_SAMPLES_PER_TEAM, un_samples_per_team );
   if( un_samples_per_team == 0 ) {
      THROW_ARGOSEXCEPTION("[REVOLVER] aclonal evaluation strategy requires at least 1 sample per team");
   }
   
   GetNodeAttribute(t_tree, CONFIGURATION_EVOLUTION_TEAM_SIZE, m_unTeamSize );
   if( m_unTeamSize <= 1 ) {
      THROW_ARGOSEXCEPTION("[REVOLVER] aclonal evaluation strategy requires team size > 1");
   }

   if( m_unNumSamples%un_samples_per_team != 0 ) {
      THROW_ARGOSEXCEPTION("[REVOLVER] umbalanced number of samples per team: total number of samples is " << m_unNumSamples << ", but " << un_samples_per_team << " samples per team are specified");
   }
   
   // total number of controllers is given by the number of teams to
   // be formed (m_unNumSamples/un_samples_per_team)
   m_unNumTeams       = m_unNumSamples/un_samples_per_team;
}


/****************************************/
/****************************************/

CEvaluationConfig* CAclonalEvaluation::GetEvaluationConfig( const UInt32 un_individual_index, const CPopulation& c_population ) {
   // build a basic aclonal evaluation config
   CEvaluationConfig* pcEvaluationConfig = new CEvaluationConfig( m_unNumTeams, m_unTeamSize );

   // set the focal individual index
   pcEvaluationConfig->SetIndividualIndex(un_individual_index);

   // build the teams randomly, and insert the control parameters
   UInt32 controller_index = 0;
   for( UInt32 i = 0; i < m_unNumTeams; ++i ) {
      // insert the focal individaul into the the team specification
      pcEvaluationConfig->InsertTeamMember(i, un_individual_index );
      pcEvaluationConfig->InsertControlParameters(un_individual_index, c_population.GetIndividualParameters(un_individual_index));
      
      // draw controllers from the population at random
      for( UInt32 j = 1; j < m_unTeamSize; ++j ) {
	 UInt32 un_index = m_pcRNG->Uniform(CRange<UInt32>(0,c_population.GetSize()));
	 pcEvaluationConfig->InsertTeamMember(i, un_index);
	 pcEvaluationConfig->InsertControlParameters(un_index, c_population.GetIndividualParameters(un_index));
      }
   }

   // return the reference
   return pcEvaluationConfig;
};



REGISTER_EVALUATION_STRATEGY(CAclonalEvaluation,"aclonal");
