/* -*- Mode:C++ -*- */

/**
 *
 * This class provides the implementation for the clonal evaluation
 * strategy.
 *
 * A clonal evaluation strategy is characterised by homogeneous
 * groups, that is, groups that are identical in their control
 * structure. Therefore, a single controller is sufficient to create
 * the group. As a consequence, the evaulation configuration is
 * generated simply including one controller into the available
 * parameters, and ignoring the team specifiaction
 *
 */

#ifndef CLONAL_EVALUATION_H
#define CLONAL_EVALUATION_H

#include "evaluation_strategy.h"

class CClonalEvaluation : public CEvaluationStrategy {
 private:

 public:
   CClonalEvaluation();
   ~CClonalEvaluation();

   virtual CEvaluationConfig* GetEvaluationConfig( const UInt32 un_individual_index, const CPopulation& c_population );
};




#endif
