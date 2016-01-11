/* -*- Mode:C++ -*- */

#ifndef _SOO_POPULATION_H
#define _SOO_POPULATION_H

#include "population.h"

using namespace std;
using namespace argos;


class CSOOPopulation : public CPopulation  {
 protected:
   // the actual population
   TVecIndividuals m_vecIndividuals;

   bool m_bSorted;
   UInt32 m_unBestIndividual;
   UInt32 m_unWorstIndividual;
   

 public:
   CSOOPopulation();
   ~CSOOPopulation();

   // generic initialisation from the configuration tree, loaded from the xml file
   virtual void Init( TConfigurationNode& t_configuration_tree );
   
   // sort population according to individual performance
   virtual void Sort();
   
   // update the population according to the reproduction strategy
   virtual void Update();

   // get the parameters of a specific individual
   virtual CGenotype GetIndividualParameters( const UInt32& un_individual_number ) const;
   
   // set and store the computed performance of an individual
   virtual void SetPerformance( const UInt32& un_individual_number, CObjectives& c_objetives );

   // functions to retrieve best/worst fitness values in the population
   virtual Real GetIndividualFitness( const UInt32& un_individual_number );
   virtual Real GetBestIndividualFitness();
   virtual Real GetWorstIndividualFitness();

   // functions to save the population and specific individuals
   virtual void Dump( const string& filename );
   virtual void DumpIndividual( const UInt32& un_individual_number, const string& filename );
   virtual void DumpBestIndividuals( const string& filename );

};


#endif

