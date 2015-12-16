#include "mating_population.h"
#include <algorithm>

const string CONFIGURATION_EVOLUTION_OBJECTIVES                = "num_objectives";
const string CONFIGURATION_EVOLUTION_FOUNDING_TEAM_SIZE        = "founding_team_size";
const string CONFIGURATION_EVOLUTION_RECOMBINATION_FACTOR      = "recombination_factor";

/****************************************/
/****************************************/

CMatingPopulation::CMatingPopulation() :
   CPopulation(),
   m_unFoundingTeamSize(0),
   m_fRecombinationFactor(0.0),
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
   
   GetNodeAttribute(t_configuration_tree, CONFIGURATION_EVOLUTION_RECOMBINATION_FACTOR, m_fRecombinationFactor );
   if( m_fRecombinationFactor < 0.0 || m_fRecombinationFactor > 1.0 ) {
      THROW_ARGOSEXCEPTION("[REVOLVER] the recombination factor must be in [0,1]");
   }
   
   UInt32 nGenotypeUniqueID = 0;
   
   // initialise the genotypes within the population
   for( UInt32 i = 0; i < m_unPopulationSize; ++i ) {
      CEvaluationConfig* cSingleTeamEC = new CEvaluationConfig( 1, m_unFoundingTeamSize );
      cSingleTeamEC->SetIndividualIndex(i); // This is the ID of the mother. There are M ids.
      
      for(UInt32 j = 0; j < m_unFoundingTeamSize; ++j){
         CGenotype cTeamMemberGenotype(m_pcRNG,m_unGenotypeSize, m_cGenotypeValueRange);
         cTeamMemberGenotype.SetID(nGenotypeUniqueID); // Each genotype is indexed in [0,M*m]
         
         cSingleTeamEC->InsertControlParameters(j,cTeamMemberGenotype);
         
         //cSingleTeamEC->InsertControlParameters(nGenotypeUniqueID,cTeamMemberGenotype);
         //cSingleTeamEC->InsertTeamMember(0,nGenotypeUniqueID); // Each genotype is indexed in [0,M*m]
      }
      
      m_vecTeams.push_back(cSingleTeamEC);
      
      nGenotypeUniqueID++;
   }
}


/****************************************/
/****************************************/

void CMatingPopulation::Sort() { 
   if( m_bSorted ) return;

   if( Maximise() ) {
      std::sort(m_vecTeams.begin(), m_vecTeams.end(), std::greater<CEvaluationConfig*>());
   }
   else {
      std::sort(m_vecTeams.begin(), m_vecTeams.end(), std::less<CEvaluationConfig*>());
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
      cOffSpringEC->SetIndividualIndex(i); // This is the ID of the mother. There are M ids.
      for( UInt32 j = 0; j < m_unFoundingTeamSize; ++j ) {
         bool elite = false;
         UInt32 index = m_pcSelectionStrategy->GetNextIndividual(elite); // This should get the ID of the mother/funding team
         CGenotype cOffSpringGenotype = GetOffspringGenotypeFromFoundingTeam(index);
         cOffSpringGenotype.SetID(nGenotypeUniqueID);
         if(!elite){
            cOffSpringGenotype.MutateNormal(m_fMutationVariance);
            cOffSpringGenotype.CutOffMin();
         }
         cOffSpringEC->InsertControlParameters(j,cOffSpringGenotype);
         //cOffSpringEC->InsertTeamMember(0,nGenotypeUniqueID); // Each genotype is indexed in [0,M*m]
         nGenotypeUniqueID++;
      }
      m_vecTeamsOffsprings.push_back(cOffSpringEC);
   }
   
   // fitness proportional selection with elitism
   m_vecTeams.swap(m_vecTeamsOffsprings);
   
   for( UInt32 i = 0; i < m_unPopulationSize; ++i ) {
      delete m_vecTeamsOffsprings[i];
   }
   
   // TVecIndividuals vecOffsprings;
   // for( UInt32 i = 0; i < m_unPopulationSize; ++i ) {
   //    bool elite = false;
   //    UInt32 index = m_pcSelectionStrategy->GetNextIndividual(elite);
   //    vecOffsprings.push_back(m_vecIndividuals[index]);
   //    vecOffsprings[i].SetID(i);
   //    vecOffsprings[i].Reset();
   //    vecOffsprings[i].InsertAncestor(m_vecIndividuals[index].GetID());
   //    if( !elite ) {
	//      vecOffsprings[i].MutateNormal( m_fMutationVariance );
	//      vecOffsprings[i].CutOff();
   //    }

   // }

   // // fitness proportional selection with elitism
   // m_vecIndividuals.swap(vecOffsprings);

   m_bSorted = false;
}

/****************************************/
/****************************************/

CGenotype CMatingPopulation::GetOffspringGenotypeFromFoundingTeam(UInt32 c_founding_team_id){
   
   CGenotype offSpringGenotype;
   
   //TTeam cFoundingTeam = m_vecTeams[c_founding_team_id]->GetTeam(0);
   
   //CControlParameters& cMotherParameters = m_vecTeams[c_founding_team_id]->GetControlParameters(cFoundingTeam[0]);
   CControlParameters& cMotherParameters = m_vecTeams[c_founding_team_id]->GetControlParameters(0);
   CGenotype& cMotherGenotype = dynamic_cast<CGenotype&>(cMotherParameters);
   
   UInt32 nFatherID = m_pcRNG->Uniform(CRange<UInt32>(1,m_unFoundingTeamSize)); // The father is between 1 and m-1
   
   //CControlParameters& cFatherParameters = m_vecTeams[c_founding_team_id]->GetControlParameters(cFoundingTeam[nFatherID]);
   CControlParameters& cFatherParameters = m_vecTeams[c_founding_team_id]->GetControlParameters(nFatherID);
   CGenotype& cFatherGenotype = dynamic_cast<CGenotype&>(cFatherParameters);
   
   Real fRecombineRandom = m_pcRNG->Uniform(CRange<Real>(0.0,1.0));
   if(fRecombineRandom < m_fRecombinationFactor){
      // Single (random) point crossover
      UInt32 nCutoffPoint = m_pcRNG->Uniform(CRange<UInt32>(1,m_unGenotypeSize)); // First (last) element always in first (second) chunk
      Real pf_control_parameters[m_unGenotypeSize];
      Real fParentChoiceRandom = m_pcRNG->Uniform(CRange<Real>(0.0,1.0)); // From which parent we chose first and second block
      vector<Real> vecMotherValues = cMotherGenotype.GetValues();
      vector<Real> vecFatherValues = cFatherGenotype.GetValues();
      // First chunk until cutoff
      for(UInt32 i = 0; i < nCutoffPoint ; ++i){
         if(fParentChoiceRandom < 0.5){
            pf_control_parameters[i] = vecMotherValues[i];
         }
         else{
            pf_control_parameters[i] = vecFatherValues[i];
         }
      }
      // Second chunk from cutoff to the end. We use the same random number so if before we took from mom now
      // we take from dad and viceversa
      for(UInt32 i = nCutoffPoint; i < m_unGenotypeSize ; ++i){
         if(fParentChoiceRandom < 0.5){
            pf_control_parameters[i] = vecFatherValues[i];
         }
         else{
            pf_control_parameters[i] = vecMotherValues[i];
         }
      }
      CGenotype offSpringGenotype(m_unGenotypeSize,pf_control_parameters,m_cGenotypeValueRange);
      offSpringGenotype.InsertAncestor(cMotherGenotype.GetID());
      offSpringGenotype.InsertAncestor(cFatherGenotype.GetID());
      return offSpringGenotype;
   }
   else{
      Real fParentChoiceRandom = m_pcRNG->Uniform(CRange<Real>(0.0,1.0));
      if(fParentChoiceRandom < 0.5){
         CGenotype offSpringGenotype(cMotherGenotype);
         offSpringGenotype.InsertAncestor(cMotherGenotype.GetID());
         offSpringGenotype.InsertAncestor(cFatherGenotype.GetID()); // Should this be here?
         return offSpringGenotype;
      }
      else{
         CGenotype offSpringGenotype(cFatherGenotype);
         offSpringGenotype.InsertAncestor(cMotherGenotype.GetID()); // Should this be here?
         offSpringGenotype.InsertAncestor(cFatherGenotype.GetID());
         return offSpringGenotype;
      }
   }
   
}

/****************************************/
/****************************************/

CControlParameters CMatingPopulation::GetIndividualParameters( const UInt32& un_individual_number ) const {
   //return m_vecIndividuals[un_individual_number];
   //TTeam cTeam = m_vecTeams[un_individual_number]->GetTeam(0);
   THROW_ARGOSEXCEPTION( "The function CMatingPopulation::GetIndividualParameters should never ever be called, as evaluation config in mating does not need it." );
   return m_vecTeams[un_individual_number]->GetControlParameters(0);
}


/****************************************/
/****************************************/

void CMatingPopulation::SetPerformance( const UInt32& un_individual_number, CObjectives& c_objectives ) {
   //m_vecIndividuals[un_individual_number].SetPerformance( c_objectives );
   
   //TTeam cTeam = m_vecTeams[un_individual_number]->GetTeam(0);
   for (UInt32 i = 0; i < m_unFoundingTeamSize; ++i){
      CControlParameters& cParameters = m_vecTeams[un_individual_number]->GetControlParameters(i);
      CGenotype& cTeamMemberGenotype = dynamic_cast<CGenotype&>(cParameters);
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
         CGenotype& cGenotype = dynamic_cast<CGenotype&>(m_vecTeams[i]->GetControlParameters(j));
         out << cGenotype << "\t";
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
      CGenotype& cGenotype = dynamic_cast<CGenotype&>(m_vecTeams[un_individual_number]->GetControlParameters(j));
      out << cGenotype << "\t";
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
