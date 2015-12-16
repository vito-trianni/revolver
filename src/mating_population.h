/* -*- Mode:C++ -*- */

#ifndef _MATING_POPULATION_H
#define _MATING_POPULATION_H

#include "population.h"
#include <utility/evaluation_config.h>

using namespace std;
using namespace argos;

typedef vector<CEvaluationConfig*> TVecFoundingTeams;

class CMatingPopulation : public CPopulation  {
 protected:
   // the actual population
   TVecFoundingTeams m_vecTeams;
   UInt32 m_unFoundingTeamSize;
   Real m_fRecombinationFactor;

   bool m_bSorted;
   UInt32 m_unBestIndividual;
   UInt32 m_unWorstIndividual;
   

 public:
   CMatingPopulation();
   ~CMatingPopulation();

   // generic initialisation from the configuration tree, loaded from the xml file
   virtual void Init( TConfigurationNode& t_configuration_tree );
   
   // sort population according to individual performance
   virtual void Sort();
   
   // update the population according to the reproduction strategy
   virtual void Update();

   // get the founding team associated to a specific mother/ID
   virtual inline CEvaluationConfig* GetFoundingTeam(const UInt32 un_individual_index) const {return m_vecTeams[un_individual_index];};

   virtual CGenotype GetOffspringGenotypeFromFoundingTeam(UInt32 c_founding_team_id);

   // get the parameters of a specific individual
   virtual CControlParameters GetIndividualParameters( const UInt32& un_individual_number ) const;
   
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

