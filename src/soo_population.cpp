#include "soo_population.h"
#include <algorithm>

const string CONFIGURATION_EVOLUTION_OBJECTIVES      = "num_objectives";

/****************************************/
/****************************************/

CSOOPopulation::CSOOPopulation() :
   CPopulation(),
   m_bSorted( false )
{}


/****************************************/
/****************************************/

CSOOPopulation::~CSOOPopulation() {
}


/****************************************/
/****************************************/

void CSOOPopulation::Init( TConfigurationNode& t_configuration_tree ) {
   CPopulation::Init(t_configuration_tree);
   
   // initialise the genotypes within the population
   for( UInt32 i = 0; i < m_unPopulationSize; ++i ) {
      m_vecIndividuals.push_back(CGenotype(m_pcRNG,m_unGenotypeSize, m_cGenotypeValueRange));
      m_vecIndividuals[i].SetID(i);
   }
}


/****************************************/
/****************************************/

void CSOOPopulation::Sort() { 
   if( m_bSorted ) return;

   if( Maximise() ) {
      std::sort(m_vecIndividuals.begin(), m_vecIndividuals.end(), std::greater<CGenotype>());
   }
   else {
      std::sort(m_vecIndividuals.begin(), m_vecIndividuals.end(), std::less<CGenotype>());
   }
   m_bSorted = true;
};

/****************************************/
/****************************************/

void CSOOPopulation::Update() {
   // reset/update the selection strategy
   m_pcSelectionStrategy->Update( *this );
   
   // reproduction with mutation only
   TVecIndividuals vecOffsprings;
   for( UInt32 i = 0; i < m_unPopulationSize; ++i ) {
      bool elite = false;
      UInt32 index = m_pcSelectionStrategy->GetNextIndividual(elite);
      vecOffsprings.push_back(m_vecIndividuals[index]);
      vecOffsprings[i].SetID(i);
      vecOffsprings[i].Reset();
      vecOffsprings[i].InsertAncestor(m_vecIndividuals[index].GetID());
      if( !elite ) {
	 vecOffsprings[i].MutateNormal( m_fMutationVariance );
	 vecOffsprings[i].CutOff();
      }

   }

   // fitness proportional selection with eltiism
   m_vecIndividuals.swap(vecOffsprings);

   m_bSorted = false;
}


/****************************************/
/****************************************/

CControlParameters CSOOPopulation::GetIndividualParameters( const UInt32& un_individual_number ) const {
   return m_vecIndividuals[un_individual_number];
}


/****************************************/
/****************************************/

void CSOOPopulation::SetPerformance( const UInt32& un_individual_number, CObjectives& c_objectives ) {
   m_vecIndividuals[un_individual_number].SetPerformance( c_objectives );
}


/****************************************/
/****************************************/

Real CSOOPopulation::GetIndividualFitness( const UInt32& un_individual_number ) {
   return m_vecIndividuals[un_individual_number].GetPerformance()[0];
};


/****************************************/
/****************************************/

Real CSOOPopulation::GetBestIndividualFitness() {
   Sort();
   return m_vecIndividuals[0].GetPerformance()[0];
};

/****************************************/
/****************************************/

Real CSOOPopulation::GetWorstIndividualFitness() {
   Sort();
   return m_vecIndividuals[m_unPopulationSize-1].GetPerformance()[0];
}

/****************************************/
/****************************************/

void CSOOPopulation::Dump( const string& filename ) {
   // open the output stream
   ofstream out( filename.c_str(), ios::out );
   if( !out ) {
      THROW_ARGOSEXCEPTION( "Could not open file" << filename << " for dumping population" );
   }

   for( UInt32 i = 0; i < m_unPopulationSize; ++i ) {
      out << m_vecIndividuals[i] << endl;
   }

   out.close();
}



/****************************************/
/****************************************/

void CSOOPopulation::DumpIndividual( const UInt32& un_individual_number, const string& filename ) {
   // open the output stream
   ofstream out( filename.c_str(), ios::out );
   if( !out ) {
      THROW_ARGOSEXCEPTION( "Could not open file" << filename << " for dumping population" );
   }
   out << m_vecIndividuals[un_individual_number] << endl;

   out.close();
}


/****************************************/
/****************************************/

void CSOOPopulation::DumpBestIndividuals( const string& filename ) {
   DumpIndividual(0, filename);
}


REGISTER_POPULATION(CSOOPopulation, "soo_population")
