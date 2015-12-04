#include "roulette_wheel_selection.h"
#include "population.h"

/****************************************/
/****************************************/

CRouletteWheelSelection::CRouletteWheelSelection() :
   m_unCurrentIndex(0),
   m_fTotalFitness(0)
{
}


/****************************************/
/****************************************/

CRouletteWheelSelection::~CRouletteWheelSelection() {
}



/****************************************/
/****************************************/

void CRouletteWheelSelection::Update( CPopulation& population ) {
   population.Sort();
   m_unCurrentIndex = 0;
   m_fTotalFitness = 0;
   for( UInt32 i = 0; i < population.GetSize(); ++i ) {
      Real fitness = population.GetIndividualFitness(i);
      m_mapIndividualFitness[i] = fitness;
      m_fTotalFitness += fitness;
   }
}

/****************************************/
/****************************************/

UInt32 CRouletteWheelSelection::GetNextIndividual( bool& elite ) {
   UInt32 nextIndividual;
   if( m_unCurrentIndex < m_unEliteSize ) {
      elite = true;
      nextIndividual = m_unCurrentIndex;
   }
   else if( m_fTotalFitness == 0 ) {
      elite = false;
      nextIndividual = m_pcRNG->Uniform(CRange<UInt32>(0,m_mapIndividualFitness.size()));
   }
   else {
      elite = false;
      Real roulette = m_pcRNG->Uniform(CRange<Real>(0,m_fTotalFitness));
      for( nextIndividual = 0; nextIndividual < m_mapIndividualFitness.size(); ++nextIndividual ) {
	 roulette -= m_mapIndividualFitness[nextIndividual];
	 if( roulette <= 0 ) break;
      }

      if( nextIndividual == m_mapIndividualFitness.size() ) {
	 nextIndividual = m_pcRNG->Uniform(CRange<UInt32>(0,m_mapIndividualFitness.size()));
      }
   }
   
   m_unCurrentIndex += 1;
   return nextIndividual;
}


/****************************************/
/****************************************/



REGISTER_SELECTION_STRATEGY(CRouletteWheelSelection,"roulette");
