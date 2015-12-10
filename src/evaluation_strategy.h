/* -*- Mode:C++ -*- */

#ifndef EVALUATION_STRATEGY_H
#define EVALUATION_STRATEGY_H

#include <argos_utility/configuration/base_configurable_resource.h>
#include <argos_utility/datatypes/datatypes.h>
#include <argos_utility/logging/argos_log.h>

#include <utility/control_parameters.h>
#include <utility/evaluation_config.h>

#include <src/population.h>

class CEvaluationStrategy : public CBaseConfigurableResource {
protected:
   // variables for the random number generation
   CRandom::CRNG*  m_pcRNG;

   UInt32 m_unNumSamples;
   UInt32 m_unNumTeams;
   UInt32 m_unTeamSize;
   UInt32 m_unNumControllers;
   
public:
   CEvaluationStrategy();
   ~CEvaluationStrategy();
   
   // function to set the RNG pointer
   inline void SetRNG( CRandom::CRNG* pc_rng ) { m_pcRNG = pc_rng; };

   virtual void Init(TConfigurationNode& t_tree);
   virtual void Reset() {};
   virtual void Destroy() {};
   
   virtual CEvaluationConfig* GetEvaluationConfig( const UInt32 un_individual_index, const CPopulation& c_population ) = 0;

   // get/set number of samples
   inline const UInt32 GetNumSamples() { return m_unNumSamples; };
   inline void SetNumSamples( const UInt32& un_samples ) { m_unNumSamples = un_samples; };

   inline const UInt32 GetNumControllers() {return m_unNumControllers;};
   inline const UInt32 GetNumTeams() {return m_unNumTeams;};
   inline const UInt32 GetTeamSize() {return m_unTeamSize;};

};




typedef CFactory<CEvaluationStrategy> TFactoryEvaluationStrategy;
#define REGISTER_EVALUATION_STRATEGY(CLASSNAME, LABEL)	  \
   REGISTER_SYMBOL(CEvaluationStrategy,			  \
                   CLASSNAME,				  \
                   LABEL,				  \
                   "undefined",				  \
                   "undefined",				  \
                   "undefined",				  \
                   "undefined",				  \
                   "undefined")

#endif




