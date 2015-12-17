/* -*- Mode:C++ -*- */

#ifndef EVALUATION_CONFIG_H
#define EVALUATION_CONFIG_H

#include <limits.h>

#include "../argos_utility/configuration/base_configurable_resource.h"
#include "../argos_utility/datatypes/datatypes.h"
#include "../argos_utility/logging/argos_log.h"
#include "../argos_utility/math/rng.h"

#include "vector.h"
#include "objectives.h"
#include "genotype.h"

using namespace argos;
using namespace std;

typedef CVector<UInt32> TTeam;
typedef vector<TTeam>  TVecTeams;
typedef map<UInt32, CGenotype> TMapParamters;
typedef map<UInt32, CGenotype>::iterator TMapParamtersIterator;

class CEvaluationConfig : public CBaseConfigurableResource {
 private:
   UInt32 m_unNumTeams;
   UInt32 m_unTeamSize;
   
   UInt32 m_unIndividualIndex;

   TMapParamters m_mapControlParameters;
   TVecTeams m_vecTeams;
   CVector<UInt32> m_vecSampleSeeds;
   TVecObjectives m_vecResults;

 public:
   CEvaluationConfig();
   CEvaluationConfig( UInt32 un_num_teams, UInt32 un_team_size );
   ~CEvaluationConfig();

   virtual void Init( TConfigurationNode& t_configuration_tree );
   inline virtual void Destroy() {};
   inline virtual void Reset() {};

   
   inline const UInt32 GetIndividualIndex() const { return m_unIndividualIndex; };
   inline void SetIndividualIndex( const UInt32 un_individual_index ) { m_unIndividualIndex = un_individual_index; };

   inline const UInt32 GetNumControllers() const { return m_mapControlParameters.size(); };

   inline const TMapParamters& GetMapControlParameters() {return m_mapControlParameters;};
   //inline const CGenotype& GetControlParameters( const UInt32 un_index ) { return m_mapControlParameters[un_index]; };
   // TODO: I had to do this because the mating population needs to write the performance in the control parameters
   // and to do so it needs to cast them into a Genotype
   inline CGenotype& GetControlParameters( const UInt32 un_index ) { return m_mapControlParameters[un_index]; };
   void InsertControlParameters( const UInt32 un_index, CGenotype c_control_parameters );
   inline void ClearControlParameters() { m_mapControlParameters.clear(); };
   void LoadControlParameters( const string& s_population_file );
   void LoadControlParameters( const UInt32 un_index, const string& s_parameters_file );

   inline const UInt32 GetNumTeams() const { return m_unNumTeams; };
   inline void SetNumTeams( const UInt32 un_num_teams ) { m_unNumTeams = un_num_teams; };

   inline const UInt32 GetTeamSize() const { return m_unTeamSize; };
   inline void SetTeamSize( const UInt32 un_team_size ) { m_unTeamSize = un_team_size; };
   inline const TTeam& GetTeam( UInt32 un_index ) const { return m_vecTeams[un_index]; };
   void InsertTeam( const UInt32 un_team_index, TTeam vec_team );
   void InsertTeamMember( const UInt32 un_team_index, const UInt32 un_team_member );
   void SetTeams( const UInt32 un_num_values, const UInt32* pun_teams );
   inline void ClearTeams() { m_vecTeams.clear(); };

   inline void SetNumSampleSeeds(const UInt32 un_num_samples ) {m_vecSampleSeeds.SetSize(un_num_samples);m_vecResults.resize(un_num_samples);};
   inline void SetSampleSeed(const UInt32 un_num_sample, const UInt32 random_seed ) {m_vecSampleSeeds[un_num_sample] = random_seed;};
   inline void SetSampleSeeds(const CVector<UInt32> vec_random_seeds ) {m_vecSampleSeeds = vec_random_seeds;};
   inline const UInt32 GetSampleSeed(const UInt32 un_num_sample) const { return m_vecSampleSeeds[un_num_sample]; };

   inline void SetEvaluationResults( TVecObjectives vec_results ) {m_vecResults = vec_results;};
   inline const TVecObjectives& GetEvaluationResults()const { return m_vecResults;};

   friend ostream& operator <<( ostream& os, const CEvaluationConfig& c_ec );


};


typedef vector<CEvaluationConfig*> TVecEvaluationConigurations;


#endif


