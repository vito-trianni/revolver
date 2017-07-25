#include "population.h"

const string CONFIGURATION_POPULATION_MAXIMISE                 = "maximise";
const string CONFIGURATION_POPULATION_SIZE                     = "population_size";
const string CONFIGURATION_GENOTYPE_SIZE                       = "genotype_size";
const string CONFIGURATION_GENOTYPE_VALUE_RANGE                = "gene_value_range";
const string CONFIGURATION_MUTATION_VARIANCE                   = "mutation_variance";
const string CONFIGURATION_EVOLUTION_OBJECTIVES                = "num_objectives";
const string CONFIGURATION_EVOLUTION_RECOMBINATION_FACTOR      = "recombination_factor";

const string CONFIGURATION_GENOTYPE_TYPE                       = "genotype_type";
const string CONFIGURATION_DIPLOID_DOMINANCE_TYPE              = "dominance_type";



/****************************************/
/****************************************/

CPopulation::CPopulation() :
   m_pcRNG( NULL ),
   m_bMaximise( true ),
   m_unPopulationSize( 0 ),
   m_unGenotypeSize( 0 ),
   m_cGenotypeValueRange(0,1),
   m_fMutationVariance( 0.1 ),
   m_fRecombinationFactor(0.0),
   m_sGenotypeType(""),
   m_sDominanceType(""),
   m_unNumObjectives( 0 )
{}


/****************************************/
/****************************************/

CPopulation::~CPopulation() {
   CFactory<CSelectionStrategy>::Destroy();

   delete m_pcSelectionStrategy;
}


/****************************************/
/****************************************/

void CPopulation::Init( TConfigurationNode& t_configuration_tree ) {
   ////////////////////////////////////////////////////////////////////////////////
   // flag for maximisation/minimisation
   ////////////////////////////////////////////////////////////////////////////////
   GetNodeAttributeOrDefault(t_configuration_tree, CONFIGURATION_POPULATION_MAXIMISE, m_bMaximise, m_bMaximise );

   ////////////////////////////////////////////////////////////////////////////////
   // parameters of the population of individuals
   ////////////////////////////////////////////////////////////////////////////////
   GetNodeAttribute(t_configuration_tree, CONFIGURATION_POPULATION_SIZE, m_unPopulationSize );

   ////////////////////////////////////////////////////////////////////////////////
   // parameters of the genotype
   ////////////////////////////////////////////////////////////////////////////////
   GetNodeAttribute(t_configuration_tree, CONFIGURATION_GENOTYPE_SIZE, m_unGenotypeSize );
   GetNodeAttributeOrDefault(t_configuration_tree, CONFIGURATION_GENOTYPE_VALUE_RANGE, m_cGenotypeValueRange, m_cGenotypeValueRange );
   GetNodeAttributeOrDefault(t_configuration_tree, CONFIGURATION_MUTATION_VARIANCE, m_fMutationVariance, m_fMutationVariance );

   ////////////////////////////////////////////////////////////////////////////////
   // number of objectives computed by the system
   ////////////////////////////////////////////////////////////////////////////////
   GetNodeAttribute(t_configuration_tree, CONFIGURATION_EVOLUTION_OBJECTIVES, m_unNumObjectives );

   ////////////////////////////////////////////////////////////////////////////////
   // recombination factor for mating type of populations. Ignored otherwise. Here because parellel needs to communicate it.
   ////////////////////////////////////////////////////////////////////////////////
   GetNodeAttribute(t_configuration_tree, CONFIGURATION_EVOLUTION_RECOMBINATION_FACTOR, m_fRecombinationFactor );
   if( m_fRecombinationFactor < 0.0 || m_fRecombinationFactor > 1.0 ) {
      THROW_ARGOSEXCEPTION("[REVOLVER] the recombination factor must be in [0,1]");
   }

   ////////////////////////////////////////////////////////////////////////////////
   // same problem as for the recombination factor for mating type of populations. 
   ////////////////////////////////////////////////////////////////////////////////

   GetNodeAttribute(t_configuration_tree, CONFIGURATION_GENOTYPE_TYPE , m_sGenotypeType );
   GetNodeAttribute(t_configuration_tree, CONFIGURATION_DIPLOID_DOMINANCE_TYPE , m_sDominanceType );


   ////////////////////////////////////////////////////////////////////////////////
   // initialise selection strategy
   ////////////////////////////////////////////////////////////////////////////////
   InitSelectionStrategy(GetNode(t_configuration_tree, "selection"));

}



/****************************************/
/****************************************/

void CPopulation::InitSelectionStrategy( TConfigurationNode& t_tree ) {
   try {
      string selection_type;
      GetNodeAttribute(t_tree, "type", selection_type);
      m_pcSelectionStrategy = CFactory<CSelectionStrategy>::New(selection_type);
      m_pcSelectionStrategy->SetRNG(m_pcRNG);
      m_pcSelectionStrategy->Init(t_tree);
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("[REVOLVER] Error initializing selection strategy", ex);
   }
}


/****************************************/
/****************************************/

Real CPopulation::GetAveragePopulationFitness() {
   Real f_average_fitness = 0;
   for( UInt32 i = 0; i < m_unPopulationSize; ++i ) {
      f_average_fitness += GetIndividualFitness(i);
   }
   return f_average_fitness/m_unPopulationSize;
}

