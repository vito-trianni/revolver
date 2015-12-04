/* -*- Mode:C++ -*- */

#ifndef ROULETTE_WHEEL_SELECTION_H
#define ROULETTE_WHEEL_SELECTION_H

#include <revolver/selection_strategy.h>

using namespace std;
using namespace argos;

class CRouletteWheelSelection : public CSelectionStrategy {
 private:
   UInt32 m_unCurrentIndex;
   map<UInt32,Real> m_mapIndividualFitness;
   Real m_fTotalFitness;


 public:
   CRouletteWheelSelection();
   ~CRouletteWheelSelection();
   
   virtual void Update( CPopulation& population );
   virtual UInt32 GetNextIndividual( bool& elite );

};


#endif




