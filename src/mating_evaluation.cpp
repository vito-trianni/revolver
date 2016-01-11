#include "mating_evaluation.h"

/****************************************/
/****************************************/

CMatingEvaluation::CMatingEvaluation() :
   CEvaluationStrategy()
{
   m_unNumTeams = 1;
}


/****************************************/
/****************************************/

CMatingEvaluation::~CMatingEvaluation() {
};

/****************************************/
/****************************************/

void CMatingEvaluation::GetInfoFromPopulation(const CPopulation* c_population)  {
   const CMatingPopulation* cMatingPopulation = dynamic_cast<const CMatingPopulation*>(c_population);
   if (cMatingPopulation) {
      m_unTeamSize = cMatingPopulation->GetFoundingTeamSize();
   }
   else{
      THROW_ARGOSEXCEPTION("[REVOLVER] mating evaluation strategy requires a mating population");
   }
}

/****************************************/
/****************************************/

CEvaluationConfig* CMatingEvaluation::GetEvaluationConfig( const UInt32 un_individual_index, const CPopulation& c_population ) {
   CEvaluationConfig* pcEvaluationConfig;
   
   const CMatingPopulation& cMatingPopulation = dynamic_cast<const CMatingPopulation&>(c_population);
   
   if (&cMatingPopulation) {
      pcEvaluationConfig = cMatingPopulation.GetFoundingTeam(un_individual_index);
   }
   else{
      THROW_ARGOSEXCEPTION("[REVOLVER] mating evaluation strategy requires a mating population");
   }

   // return the reference
   return pcEvaluationConfig;
};



REGISTER_EVALUATION_STRATEGY(CMatingEvaluation,"mating_evaluation_strategy");
