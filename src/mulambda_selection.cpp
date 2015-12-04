#include "mulambda_selection.h"
#include "population.h"

const string CONFIGURATION_SELECTION_MU = "mu";

/****************************************/
/****************************************/

CMuLambdaSelection::CMuLambdaSelection() :
   m_unMu(0),
   m_unCurrentIndex(0)
{
}


/****************************************/
/****************************************/

CMuLambdaSelection::~CMuLambdaSelection() {
}


/****************************************/
/****************************************/

void CMuLambdaSelection::Init(TConfigurationNode& t_tree) {
   CSelectionStrategy::Init(t_tree);
   GetNodeAttribute(t_tree, CONFIGURATION_SELECTION_MU, m_unMu );
}


/****************************************/
/****************************************/

void CMuLambdaSelection::Update( CPopulation& population ) {
   population.Sort();

   m_unCurrentIndex = 0;
}

/****************************************/
/****************************************/

UInt32 CMuLambdaSelection::GetNextIndividual( bool& elite ) {
   UInt32 nextIndividual;
   if( m_unCurrentIndex < m_unEliteSize ) {
      elite = true;
      nextIndividual = m_unCurrentIndex;
   }
   else {
      elite = false;
      nextIndividual = (m_unCurrentIndex-m_unEliteSize) % m_unMu;
   }
   m_unCurrentIndex += 1;
   return nextIndividual;
}


/****************************************/
/****************************************/



REGISTER_SELECTION_STRATEGY(CMuLambdaSelection,"mulambda");
