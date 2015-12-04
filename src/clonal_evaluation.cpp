#include "clonal_evaluation.h"

/****************************************/
/****************************************/

CClonalEvaluation::CClonalEvaluation() :
   CEvaluationStrategy()
{
   m_unNumTeams = 1;
   m_unTeamSize = 1;
}


/****************************************/
/****************************************/

CClonalEvaluation::~CClonalEvaluation() {
};


/****************************************/
/****************************************/

CEvaluationConfig* CClonalEvaluation::GetEvaluationConfig( const UInt32 un_individual_index, const CPopulation& c_population ) {
   // build a basic clonal evaluation config (a single controller and no team specification results in a homogeneous group)
   CEvaluationConfig* pcEvaluationConfig = new CEvaluationConfig( m_unNumTeams, m_unTeamSize );
   
   // set the focal individual index
   pcEvaluationConfig->SetIndividualIndex(un_individual_index);

   // build and insert a fake team
   TTeam team;
   team.Insert(un_individual_index);
   pcEvaluationConfig->InsertTeam(0, team);
   // get the parameters of the reference individual, and insert 
   pcEvaluationConfig->InsertControlParameters(un_individual_index, c_population.GetIndividualParameters(un_individual_index));

   // return the reference
   return pcEvaluationConfig;
};



REGISTER_EVALUATION_STRATEGY(CClonalEvaluation,"clonal");
