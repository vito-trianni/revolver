#include "moo_population.h"

/****************************************/
/****************************************/

CMOOPopulation::CMOOPopulation() :
   CPopulation(),
   m_bSorted( false ),
   m_pcPopulation( NULL ),
   m_pcOffsprings( NULL )
{}


/****************************************/
/****************************************/

CMOOPopulation::~CMOOPopulation() {
   if( m_pcPopulation ) delete m_pcPopulation;
   if( m_pcOffsprings ) delete m_pcOffsprings;
}


/****************************************/
/****************************************/

void CMOOPopulation::Init( TConfigurationNode& t_configuration_tree ) {
   CPopulation::Init(t_configuration_tree);
   
   ChromosomeT<double> chrom(m_unGenotypeSize);
   IndividualMOO prototype( chrom );
   m_pcPopulation = new PopulationMOO( m_unPopulationSize, prototype );
   m_pcOffsprings = new PopulationMOO( m_unPopulationSize, prototype );
   
   // set the number of objectives
   m_pcPopulation->setNoOfObj( m_unNumObjectives );
   m_pcOffsprings->setNoOfObj( m_unNumObjectives );

   m_pcPopulation->setMaximize();
   m_pcOffsprings->setMaximize();

   // initialise the genotypes within the population
   for( UInt32 i = 0; i < m_unPopulationSize; ++i ) {
      (dynamic_cast< ChromosomeT<double>& >((*m_pcPopulation)[i][0])).initialize(m_cGenotypeValueRange.GetMin(), m_cGenotypeValueRange.GetMax());
   }
}


/****************************************/
/****************************************/

void CMOOPopulation::Sort() { 
   if( m_bSorted ) return;

   m_pcPopulation->SMeasure(); 
   std::sort(m_pcPopulation->begin(), m_pcPopulation->end(), PopulationMOO::compareRankShare);

   m_bSorted = true;
}


/****************************************/
/****************************************/

void CMOOPopulation::Update() {
   // reset/update the selection strategy
   m_pcSelectionStrategy->Update( *this );
   
   // reproduction with mutation only
   for( UInt32 i = 0; i < m_unPopulationSize; ++i ) {
      vector<UInt32> ancestors;
      bool elite = false;
      UInt32 index = m_pcSelectionStrategy->GetNextIndividual(elite);
      (*m_pcOffsprings)[i] = (*m_pcPopulation)[index];
      ancestors.push_back(index);
      
      if( !elite ) {
	 (dynamic_cast< ChromosomeT<double>& >((*m_pcOffsprings)[i][0])).mutateNormal( m_fMutationVariance );
	 (dynamic_cast< ChromosomeT<double>& >((*m_pcOffsprings)[i][0])).cutOff( m_cGenotypeValueRange.GetMin(), m_cGenotypeValueRange.GetMax() );
      }
   }
   
   
   // fitness proportional selection with eltiism
   *m_pcPopulation = *m_pcOffsprings;

   m_bSorted = false;
}


/****************************************/
/****************************************/

CGenotype CMOOPopulation::GetIndividualParameters( const UInt32& un_individual_number ) const {
   return CControlParameters( dynamic_cast< ChromosomeT<double>& >((*m_pcPopulation)[un_individual_number][0]) );
}


/****************************************/
/****************************************/

void CMOOPopulation::SetPerformance( const UInt32& un_individual_number, CObjectives& c_objectives ) {
   // (*m_pcPopulation)[un_individual_number].setMOOFitnessValues( c_objectives.GetValues() );
}

/****************************************/
/****************************************/

void CMOOPopulation::Dump( const string& filename ) {
   // open the output stream
   ofstream out( filename.c_str(), ios::out );
   if( !out ) {
      THROW_ARGOSEXCEPTION( "Could not open file" << filename << " for dumping population" );
   }
   
   for( UInt32 i = 0; i < m_unPopulationSize; ++i ) {
      out << GetIndividualParameters(i) << " # ";
      for( UInt32 j = 0; j < m_unNumObjectives; ++j ) {
	 out << (*m_pcPopulation)[i].getMOOFitnessValues()[j] << " ";
      }
      out << " # ";
      out << endl;
   }

   out.close();
}



/****************************************/
/****************************************/

void CMOOPopulation::DumpIndividual( const UInt32& un_individual_number, const string& filename ) {
   // open the output stream
   ofstream out( filename.c_str(), ios::out );
   if( !out ) {
      THROW_ARGOSEXCEPTION( "Could not open file" << filename << " for dumping population" );
   }
   out << GetIndividualParameters(un_individual_number) << endl;

   out.close();
}


/****************************************/
/****************************************/

void CMOOPopulation::DumpBestIndividuals( const string& filename ) {
   vector<UInt32> non_dominated;// = m_pcPopulation->getNonDominatedIndices();
   for(UInt32 i = 0; i < non_dominated.size(); ++i ) {
      DumpIndividual(non_dominated[i], filename);
   }
}


REGISTER_POPULATION(CMOOPopulation, "moo_population")
