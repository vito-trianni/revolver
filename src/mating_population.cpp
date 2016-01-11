#include "mating_population.h"
#include <algorithm>

const string CONFIGURATION_EVOLUTION_FOUNDING_TEAM_SIZE        = "founding_team_size";
const string CONFIGURATION_EVOLUTION_INIT_GENOTYPE             = "genotype_monomorphic_init_value";
const string CONFIGURATION_MUTATION_PROBABILITY                = "mutation_probability";

/****************************************/
/****************************************/

bool Less( CEvaluationConfig const *c_ec1, CEvaluationConfig const *c_ec2 ) {
  return c_ec1->GetEvaluationResults()[0][0] < c_ec2->GetEvaluationResults()[0][0];
}

bool More( CEvaluationConfig const *c_ec1, CEvaluationConfig const *c_ec2 ) {
  return c_ec1->GetEvaluationResults()[0][0] > c_ec2->GetEvaluationResults()[0][0];
}

/****************************************/
/****************************************/

CMatingPopulation::CMatingPopulation() :
   CPopulation(),
   m_unFoundingTeamSize(0),
   m_fMutationProbability(0.0),
   m_fMonomorphicInitGenotype(0.0),
   m_unBestIndividual(0),
   m_unWorstIndividual(0),
   m_bSorted( false )
{}


/****************************************/
/****************************************/

CMatingPopulation::~CMatingPopulation() {
   for(UInt32 i = 0; i < m_unPopulationSize; ++i){
      delete m_vecTeams[i];
   }
}


/****************************************/
/****************************************/

void CMatingPopulation::Init( TConfigurationNode& t_configuration_tree ) {
   CPopulation::Init(t_configuration_tree);
   
   GetNodeAttribute(t_configuration_tree, CONFIGURATION_EVOLUTION_FOUNDING_TEAM_SIZE, m_unFoundingTeamSize );
   if( m_unFoundingTeamSize <= 1 ) {
      THROW_ARGOSEXCEPTION("[REVOLVER] mating population requires founding team size m > 1");
   }
   
   GetNodeAttribute(t_configuration_tree, CONFIGURATION_EVOLUTION_INIT_GENOTYPE, m_fMonomorphicInitGenotype );
   GetNodeAttribute(t_configuration_tree, CONFIGURATION_MUTATION_PROBABILITY , m_fMutationProbability );
   
   UInt32 nGenotypeUniqueID = 0;
   
   // initialise the genotypes within the population
   for( UInt32 i = 0; i < m_unPopulationSize; ++i ) {
      CEvaluationConfig* cSingleTeamEC = new CEvaluationConfig( 1, m_unFoundingTeamSize );
      cSingleTeamEC->SetRecombinationFactor(m_fRecombinationFactor);
      cSingleTeamEC->SetIndividualIndex(i); // This is the ID of the mother. There are M ids.
      
      // build a fake team
      TTeam team;
      
      for(UInt32 j = 0; j < m_unFoundingTeamSize; ++j){
         //TODO: this is random initialization
         //CGenotype cTeamMemberGenotype(m_pcRNG,m_unGenotypeSize, m_cGenotypeValueRange);
         //Instead we want monomorphic
         Real pf_control_parameters[m_unGenotypeSize];
         for(UInt32 k = 0 ; k < m_unGenotypeSize ; ++k){
            pf_control_parameters[k] = m_fMonomorphicInitGenotype;
         }
         CGenotype cTeamMemberGenotype(m_unGenotypeSize,pf_control_parameters,m_cGenotypeValueRange);
         cTeamMemberGenotype.SetID(nGenotypeUniqueID); // Each genotype is indexed in [0,M*m]
         cTeamMemberGenotype.SetRNG(m_pcRNG);
         
         cSingleTeamEC->InsertControlParameters(j,cTeamMemberGenotype);
         
         // insert a fake team member
         team.Insert(j);
        
      }
      
      cSingleTeamEC->InsertTeam(0, team);
      
      m_vecTeams.push_back(cSingleTeamEC);
      
      nGenotypeUniqueID++;
   }
}


/****************************************/
/****************************************/

void CMatingPopulation::Sort() { 
   if( m_bSorted ) return;

   if( Maximise() ) {
      std::sort(m_vecTeams.begin(), m_vecTeams.end(), &More);
   }
   else {
      std::sort(m_vecTeams.begin(), m_vecTeams.end(), &Less);
   }
   m_bSorted = true;
};

/****************************************/
/****************************************/

void CMatingPopulation::Update() {
   // reset/update the selection strategy
   m_pcSelectionStrategy->Update( *this );
   
   // reproduction with mutation and mating based on Duarte paper
   UInt32 nGenotypeUniqueID = 0;
   TVecFoundingTeams m_vecTeamsOffsprings;
   
   for( UInt32 i = 0; i < m_unPopulationSize; ++i ) {
      CEvaluationConfig* cOffSpringEC = new CEvaluationConfig( 1, m_unFoundingTeamSize );
      cOffSpringEC->SetRecombinationFactor(m_fRecombinationFactor);
      cOffSpringEC->SetIndividualIndex(i); // This is the ID of the mother. There are M ids.
      TTeam team;
      for( UInt32 j = 0; j < m_unFoundingTeamSize; ++j ) {
         bool elite = false;
         UInt32 index = m_pcSelectionStrategy->GetNextIndividual(elite); // This should get the ID of the mother/funding team
         CGenotype cOffSpringGenotype = m_vecTeams[index]->GetOffspringGenotype(m_pcRNG);
         //LOGERR << "Offspring: " << cOffSpringGenotype << endl ;
         cOffSpringGenotype.SetID(nGenotypeUniqueID);
         cOffSpringGenotype.SetRNG(m_pcRNG);
         if(!elite){
            cOffSpringGenotype.MutateNormalWithProbability(m_fMutationVariance,m_fMutationProbability);
            cOffSpringGenotype.CutOffMin();
         }
         cOffSpringEC->InsertControlParameters(j,cOffSpringGenotype);
         //LOGERR << "Mutated offspring: " << cOffSpringGenotype << endl ;
         
         team.Insert(j);
         
         nGenotypeUniqueID++;
      }
      cOffSpringEC->InsertTeam(0, team);
      m_vecTeamsOffsprings.push_back(cOffSpringEC);
      
      //LOGERR << "Finito generating team "  << endl << endl;
   }
   
   // fitness proportional selection with elitism
   m_vecTeams.swap(m_vecTeamsOffsprings);
   
   for( UInt32 i = 0; i < m_unPopulationSize; ++i ) {
      delete m_vecTeamsOffsprings[i];
   }
   
   m_bSorted = false;
}

/****************************************/
/****************************************/

CGenotype CMatingPopulation::GetIndividualParameters( const UInt32& un_individual_number ) const {
   //return m_vecIndividuals[un_individual_number];
   //TTeam cTeam = m_vecTeams[un_individual_number]->GetTeam(0);
   THROW_ARGOSEXCEPTION( "The function CMatingPopulation::GetIndividualParameters should never ever be called, as evaluation config in mating does not need it." );
   return m_vecTeams[un_individual_number]->GetControlParameters(0);
}


/****************************************/
/****************************************/

void CMatingPopulation::SetPerformance( const UInt32& un_individual_number, CObjectives& c_objectives ) {
   
   for (UInt32 i = 0; i < m_unFoundingTeamSize; ++i){
      CGenotype& cTeamMemberGenotype = m_vecTeams[un_individual_number]->GetControlParameters(i);
      cTeamMemberGenotype.SetPerformance( c_objectives );
   }

   TVecObjectives vec_objectives;
	vec_objectives.push_back( c_objectives );
	m_vecTeams[un_individual_number]->SetEvaluationResults(vec_objectives);
   
}


/****************************************/
/****************************************/

Real CMatingPopulation::GetIndividualFitness( const UInt32& un_individual_number ) {
   //return m_vecIndividuals[un_individual_number].GetPerformance()[0];
   return m_vecTeams[un_individual_number]->GetEvaluationResults()[0][0];
};


/****************************************/
/****************************************/

Real CMatingPopulation::GetBestIndividualFitness() {
   Sort();
   return m_vecTeams[0]->GetEvaluationResults()[0][0];
   //return m_vecIndividuals[0].GetPerformance()[0];
};

/****************************************/
/****************************************/

Real CMatingPopulation::GetWorstIndividualFitness() {
   Sort();
   return m_vecTeams[m_unPopulationSize-1]->GetEvaluationResults()[0][0];
   //return m_vecIndividuals[m_unPopulationSize-1].GetPerformance()[0];
}

/****************************************/
/****************************************/

void CMatingPopulation::Dump( const string& filename ) {
   // open the output stream
   ofstream out( filename.c_str(), ios::out );
   if( !out ) {
      THROW_ARGOSEXCEPTION( "Could not open file " << filename << " for dumping population" );
   }

   for( UInt32 i = 0; i < m_unPopulationSize; ++i ) {
      for( UInt32 j = 0; j < m_unFoundingTeamSize; ++j ) {
         out << m_vecTeams[i]->GetControlParameters(j) << "\t";
      }
      out << endl;
      // out << m_vecIndividuals[i] << endl;
   }

   out.close();
}



/****************************************/
/****************************************/

void CMatingPopulation::DumpIndividual( const UInt32& un_individual_number, const string& filename ) {
   // open the output stream
   ofstream out( filename.c_str(), ios::out );
   if( !out ) {
      THROW_ARGOSEXCEPTION( "Could not open file" << filename << " for dumping population" );
   }
   for( UInt32 j = 0; j < m_unFoundingTeamSize; ++j ) {
      out << m_vecTeams[un_individual_number]->GetControlParameters(j) << "\t";
   }
   out << endl;
   // out << m_vecIndividuals[un_individual_number] << endl;

   out.close();
}


/****************************************/
/****************************************/

void CMatingPopulation::DumpBestIndividuals( const string& filename ) {
   DumpIndividual(0, filename);
}


REGISTER_POPULATION(CMatingPopulation, "mating_population")
