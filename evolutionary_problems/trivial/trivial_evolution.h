/* -*- Mode:C++ -*- */

#ifndef TRIVIAL_EVOLUTION_H
#define TRIVIAL_EVOLUTION_H

#include <src/evolution.h>

using namespace std;
using namespace argos;

class CTrivialEvolution : public CEvolution {
 private:

 public:
   CTrivialEvolution();
   ~CTrivialEvolution();

   virtual void StartEvaluationProcess();
   virtual void StopEvaluationProcess();

   virtual void PostEvaluation() { LOGERR << "TrivialEvolution::PostEvaluation()" << endl;};
   virtual void EvaluatePopulation();


};

#endif


