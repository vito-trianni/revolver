#include "selection_strategy.h"

#include <revolver/population.h>

const string CONFIGURATION_SELECTION_ELITE     = "elite_size";


/****************************************/
/****************************************/

CSelectionStrategy::CSelectionStrategy() :
   m_unEliteSize( 0 )
{}


/****************************************/
/****************************************/

CSelectionStrategy::~CSelectionStrategy()
{}


/****************************************/
/****************************************/

void CSelectionStrategy::Init( TConfigurationNode& t_configuration_tree ) {
   ////////////////////////////////////////////////////////////////////////////////
   // parameters of the population of individuals
   ////////////////////////////////////////////////////////////////////////////////
   GetNodeAttributeOrDefault(t_configuration_tree, CONFIGURATION_SELECTION_ELITE, m_unEliteSize, m_unEliteSize );
}
