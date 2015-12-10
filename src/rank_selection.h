/* -*- Mode:C++ -*- */

#ifndef RANK_SELECTION_H
#define RANK_SELECTION_H

#include <src/selection_strategy.h>

using namespace std;
using namespace argos;

class CRankSelection : public CSelectionStrategy {
 private:
   UInt32 m_unCurrentIndex;
   UInt32 m_unNumIndividuals;

 public:
   CRankSelection();
   ~CRankSelection();
   
   virtual void Update( CPopulation& population );
   virtual UInt32 GetNextIndividual( bool& elite );

};


#endif




