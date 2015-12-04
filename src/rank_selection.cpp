#include "rank_selection.h"
#include "population.h"


/****************************************/
/****************************************/

CRankSelection::CRankSelection() :
   m_unCurrentIndex(0),
   m_unNumIndividuals(0)
{}


/****************************************/
/****************************************/

CRankSelection::~CRankSelection() {
}


/****************************************/
/****************************************/

void CRankSelection::Update( CPopulation& population ) {
   population.Sort();
   m_unCurrentIndex = 0;
   m_unNumIndividuals = population.GetSize();
}

/****************************************/
/****************************************/

UInt32 CRankSelection::GetNextIndividual( bool& elite ) {
   UInt32 nextIndividual;
   if( m_unCurrentIndex < m_unEliteSize ) {
      elite = true;
      nextIndividual = m_unCurrentIndex;
   }
   else {
      Real roulette = m_pcRNG->Uniform(CRange<Real>(0,m_unNumIndividuals*(m_unNumIndividuals+1)/2));
      for( nextIndividual = 0; nextIndividual < m_unNumIndividuals; ++nextIndividual ) {
	 roulette -= (m_unNumIndividuals - nextIndividual);
	 if( roulette <= 0 ) break;
      }
      
      if( nextIndividual == m_unNumIndividuals ) {
	 nextIndividual = m_pcRNG->Uniform(CRange<UInt32>(0,m_unNumIndividuals));
      }
   }

   m_unCurrentIndex += 1;
   return nextIndividual;
}


/****************************************/
/****************************************/



REGISTER_SELECTION_STRATEGY(CRankSelection,"rank");
