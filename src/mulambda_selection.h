/* -*- Mode:C++ -*- */

#ifndef MULAMBDA_SELECTION_H
#define MULAMBDA_SELECTION_H

#include <src/selection_strategy.h>

using namespace std;
using namespace argos;

class CMuLambdaSelection : public CSelectionStrategy {
 private:
   UInt32 m_unMu;
   UInt32 m_unCurrentIndex;

 public:
   CMuLambdaSelection();
   ~CMuLambdaSelection();
   
   virtual void Init(TConfigurationNode& t_tree);

   virtual void Update( CPopulation& population );
   virtual UInt32 GetNextIndividual( bool& elite );

};


#endif




