/* -*- Mode:C++ -*- */

#ifndef _REVOLVER_POPULATION_H
#define _REVOLVER_POPULATION_H

#include <argos_utility/plugins/factory.h>
#include <argos_utility/configuration/base_configurable_resource.h>
#include <argos_utility/datatypes/datatypes.h>
#include <argos_utility/logging/argos_log.h>

#include <utility/genotype.h>
#include <utility/objectives.h>

#include "revolver_general.h"
#include "selection_strategy.h"

using namespace std;
using namespace argos;

class CPopulation : public CBaseConfigurableResource  {
 protected:
   // variables for the random number generation
   CRandom::CRNG*  m_pcRNG;

   // flag for maximisation/minimisation of fitness
   bool m_bMaximise;

   // size of the evolving population
   UInt32 m_unPopulationSize;

   // size of the genotype
   UInt32 m_unGenotypeSize;
      
   // range of the genotype
   CRange<Real> m_cGenotypeValueRange;

   // mutation variance
   Real m_fMutationVariance;

   // number of objectives to optimise
   UInt32 m_unNumObjectives;

   // selection strategy
   CSelectionStrategy* m_pcSelectionStrategy;
   
   // recombination factor used for experiments with mating (Duarte-like)
   Real m_fRecombinationFactor;
   
   // Specifies whether it's haploid, haplo-diploid, or diploid, and the type of dominance
   string m_sGenotypeType;
   string m_sDominanceType;
   
 public:
   CPopulation();
   ~CPopulation();

   // function to set and get the RNG pointer
   inline void SetRNG( CRandom::CRNG* pc_rng ) { m_pcRNG = pc_rng; };
   inline CRandom::CRNG* GetRNG(){return m_pcRNG;};
   

   // generic initialisation from the configuration tree, loaded from the xml file
   virtual void Init( TConfigurationNode& t_configuration_tree );
   virtual void InitSelectionStrategy( TConfigurationNode& t_configuration_tree );
   
   // reset function
   inline virtual void Reset() {};
      
   // destroy function
   inline virtual void Destroy() {};
      
   // sort population according to individual performance
   virtual void Sort() = 0;

   // update the population according to the reproduction strategy
   virtual void Update() = 0;

   // get the parameters of a specific individual
   virtual CGenotype GetIndividualParameters( const UInt32& un_individual_number ) const = 0;

   // set and store the computed performance of an individual
   virtual void SetPerformance( const UInt32& un_individual_number, CObjectives& c_objetives ) = 0;

   // functions to retrieve best/worst fitness values in the population
   virtual Real GetAveragePopulationFitness();
   virtual Real GetIndividualFitness( const UInt32& un_individual_number ) = 0;
   virtual Real GetBestIndividualFitness() = 0;
   virtual Real GetWorstIndividualFitness() = 0;

   // get the recombination factor
   virtual Real GetRecombinationFactor()const {return m_fRecombinationFactor;};
   
      // get the genotype type
   virtual string GetGenotypeType()const {return m_sGenotypeType;};
   
   // get the dominance type
   virtual UInt32 GetDominanceType()const {
      if(m_sDominanceType.compare("dominance") == 0){
         return 0;
      }
      else if(m_sDominanceType.compare("codominance") == 0){
         return 1;
      }
      else if(m_sDominanceType.compare("dominance_pos") == 0){
         return 2;
      }
      else if(m_sDominanceType.compare("dominance_neg") == 0){
         return 3;
      }
      else{
         LOGERR << "[MATING POPULATION] Unimplemented dominance type." << std::endl;
         exit(-1);
      }
   };

   // function to save the population and specific individuals
   inline virtual void Dump( const string& filename ) {LOG << "Population::Dump()" << endl;};
   inline virtual void DumpPerformance( const string& filename ) {LOG << "Population::DumpPerformance" << endl;};
   inline virtual void DumpIndividual( const UInt32& un_individual_number, const string& filename ) {LOG << "Population::DumpIndividual()" << endl;};
   inline virtual void DumpBestIndividuals( const string& filename ) {LOG << "Population::DumpBestIndividual" << endl;};

   // Get/Set functions
   inline virtual const bool   Maximise() const {return m_bMaximise; };
   inline virtual const UInt32 GetSize() const { return m_unPopulationSize; };
   inline virtual const UInt32 GetNumObjectives() const {return m_unNumObjectives;};
   inline virtual const UInt32 GetGenotypeSize() const {return m_unGenotypeSize;};

};

typedef CFactory<CPopulation> TFactoryPopulation;
#define REGISTER_POPULATION(CLASSNAME, LABEL)	  \
   REGISTER_SYMBOL(CPopulation,			  \
                   CLASSNAME,                     \
                   LABEL,                         \
                   "undefined",                   \
                   "undefined",                   \
                   "undefined",                   \
                   "undefined",                   \
                   "undefined")

#endif

