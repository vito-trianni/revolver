/* -*- Mode:C++ -*- */

#ifndef SELECTION_STRATEGY_H
#define SELECTION_STRATEGY_H

#include <argos_utility/plugins/factory.h>
#include <argos_utility/configuration/base_configurable_resource.h>
#include <argos_utility/logging/argos_log.h>

#include <src/genotype.h>



using namespace std;
using namespace argos;

class CPopulation;

class CSelectionStrategy : public CBaseConfigurableResource {
 protected:
   CRandom::CRNG*  m_pcRNG;

   UInt32 m_unEliteSize;


 public:
   CSelectionStrategy();
   ~CSelectionStrategy();
   
   virtual void Init(TConfigurationNode& t_tree);
   virtual void Reset() {};
   virtual void Destroy() {};
   
   virtual void Update( CPopulation& population ) = 0;
   virtual UInt32 GetNextIndividual( bool& elite ) = 0;

   inline void SetRNG( CRandom::CRNG* pc_rng ) { m_pcRNG = pc_rng; };
};




typedef CFactory<CSelectionStrategy> TFactorySelectionStrategy;
#define REGISTER_SELECTION_STRATEGY(CLASSNAME, LABEL)	  \
   REGISTER_SYMBOL(CSelectionStrategy,			  \
                   CLASSNAME,				  \
                   LABEL,				  \
                   "undefined",				  \
                   "undefined",				  \
                   "undefined",				  \
                   "undefined",				  \
                   "undefined")

#endif




