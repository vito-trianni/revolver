/* -*- Mode:C++ -*- */

#ifndef PARALLEL_EVOLUTION_H
#define PARALLEL_EVOLUTION_H

#include "evolution.h"

#include <mpi.h>

using namespace std;
using namespace argos;

typedef map<SInt32,SInt32> TMapProcessToIndividual;
typedef map<SInt32,SInt32>::iterator TMapProcessToIndividualIterator;

class CParallelEvolution : public CEvolution {
 private:
   UInt32 m_unNumProcesses;
   UInt64 m_unMsgTag;
   TMapProcessToIndividual m_mapProcessToIndividual;
   MPI::Intercomm m_cEvaluatorComm;

   UInt32* m_punEvaluationSeeds;
   
   string m_sInvalidName;

 public:
   CParallelEvolution();
   ~CParallelEvolution();

   virtual void Init( TConfigurationNode& t_configuration_tree );

   virtual void StartEvaluationProcess();
   virtual void StopEvaluationProcess();

   virtual void PostEvaluation() { cerr << "ParallelEvolution::PostEvaluation()" << endl;};
   virtual void EvaluatePopulation();

   // function for sending parameters to the fitness evaluation process
   virtual void SendIndividualParameters( UInt32 individualNumber );

   // function for receiveing fitness values from the fitness evaluation process
   virtual void ReceiveIndividualFitness();

   // virtual void LoadGeneration() { cerr << "ParallelEvolution::LoadGeneration()" << endl;};
   // virtual void LoadBestIndividuals() { cerr << "ParallelEvolution::LoadBestIndividuals()" << endl;};

   // get/set number of parallel processes
   inline const UInt32 GetNumProcesses() { return m_unNumProcesses; };
   inline void SetNumProcesses( const UInt32& un_num_processes ) { m_unNumProcesses = un_num_processes; };

   // another semi-porcata: since we cannot get rid of evaluation configs, we override this
   virtual void ClearLoggingStructures(){};

};

#endif


