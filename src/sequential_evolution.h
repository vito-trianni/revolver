/* -*- Mode:C++ -*- */

#ifndef SEQUENTIAL_EVOLUTION_H
#define SEQUENTIAL_EVOLUTION_H

// TODO: This should not be needed, we don't need plugins
// #include <argos3/core/utility/plugins/dynamic_loading.h>

// #include <argos3/core/simulator/simulator.h>
// #include <loop_functions/evolutionary_lf/evolutionary_lf.h>

#include "evolution.h"

using namespace std;
using namespace argos;

class CSequentialEvolution : public CEvolution {
 private:

 public:
   CSequentialEvolution();
   ~CSequentialEvolution();

   virtual void StartEvaluationProcess();
   virtual void StopEvaluationProcess();

   virtual void PostEvaluation() { LOGERR << "SequentialEvolution::PostEvaluation()" << endl;};
   virtual void EvaluatePopulation();

   // virtual void LoadGeneration() { cerr << "SequentialEvolution::LoadGeneration()" << endl;};
   // virtual void LoadBestIndividuals() { cerr << "SequentialEvolution::LoadBestIndividuals()" << endl;};


};

#endif


