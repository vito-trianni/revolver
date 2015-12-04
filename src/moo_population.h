/* -*- Mode:C++ -*- */

#ifndef _MOO_POPULATION_H
#define _MOO_POPULATION_H

#include "population.h"
#include <MOO-EALib/PopulationMOO.h>

using namespace std;
using namespace argos;


class CMOOPopulation : public CPopulation  {
 protected:
   bool m_bSorted;

   // the actual population
   PopulationMOO* m_pcPopulation;
   PopulationMOO* m_pcOffsprings;
   
 public:
   CMOOPopulation();
   ~CMOOPopulation();

   // generic initialisation from the configuration tree, loaded from the xml file
   virtual void Init( TConfigurationNode& t_configuration_tree );
   
   // sort population according to individual performance
   virtual void Sort();

   // update the population according to the reproduction strategy
   virtual void Update();

   // get the parameters of a specific individual
   virtual CControlParameters GetIndividualParameters( const UInt32& un_individual_number ) const;
   
   // set and store the computed performance of an individual
   virtual void SetPerformance( const UInt32& un_individual_number, CObjectives& c_objetives );

   // functions to retrieve best/worst fitness values in the population
   inline virtual Real GetIndividualFitness( const UInt32& un_individual_number ) {return m_pcPopulation[un_individual_number][0].fitnessValue();};
   inline virtual Real GetBestIndividualFitness() {return m_pcPopulation->best().fitnessValue();};
   inline virtual Real GetWorstIndividualFitness() {return m_pcPopulation->worst().fitnessValue();};

   // functions to save the population and specific individuals
   virtual void Dump( const string& filename );
   virtual void DumpIndividual( const UInt32& un_individual_number, const string& filename );
   virtual void DumpBestIndividuals( const string& filename );

};


#endif

