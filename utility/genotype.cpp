#include "genotype.h"


/****************************************/
/****************************************/
   
CGenotype::CGenotype() :
   m_pcRNG(NULL),
   m_unID(0)
{}


/****************************************/
/****************************************/

CGenotype::CGenotype( const CGenotype& params ) :
   CControlParameters( params ),
   m_pcRNG(params.m_pcRNG),
   m_unID(params.m_unID)
{
   if( this != &params ) {
      m_cPerformance = params.m_cPerformance;
      m_cAncestors = params.m_cAncestors;
   }
}

/****************************************/
/****************************************/
   
CGenotype::CGenotype( UInt32 un_num_values, const Real* pf_values, const CRange<Real>& param_range ) : 
    CControlParameters(un_num_values, pf_values, param_range),
    m_pcRNG(NULL),
    m_unID(0)
{
}

/****************************************/
/****************************************/

CGenotype::CGenotype( const vector<Real>& params, const CRange<Real>& param_range ) :
   CControlParameters( params, param_range ),
   m_pcRNG(NULL),
   m_unID(0)
{
}


/****************************************/
/****************************************/

CGenotype::CGenotype( CRandom::CRNG* pc_RNG, UInt32 un_num_values, const CRange<Real>& param_range ) :
   m_pcRNG(pc_RNG),
   m_unID(0)
{
   for( UInt32 i = 0; i < un_num_values; i++ ) {
      Insert(m_pcRNG->Uniform(param_range));
   }

   SetRange( param_range );
}


/****************************************/
/****************************************/

CGenotype::~CGenotype() {
}


/****************************************/
/****************************************/

void CGenotype::MutateNormal( const Real f_mutation_variance ) {   
   for( UInt32 i = 0; i < GetSize(); ++i ) {
      m_vecElements[i] += m_pcRNG->Gaussian(f_mutation_variance);
   }
}


/****************************************/
/****************************************/

void CGenotype::CutOff() {
   for( UInt32 i = 0; i < GetSize(); ++i ) {
      m_cParametersRange.TruncValue( m_vecElements[i] );
   }
}

/****************************************/
/****************************************/

void CGenotype::CutOffMin() {
   for( UInt32 i = 0; i < GetSize(); ++i ) {
      if (m_vecElements[i] < m_cParametersRange.GetMin()) m_vecElements[i] = m_cParametersRange.GetMin();
   }
}

/****************************************/
/****************************************/

void CGenotype::CutOffMax() {
   for( UInt32 i = 0; i < GetSize(); ++i ) {
      if (m_vecElements[i] > m_cParametersRange.GetMax()) m_vecElements[i] = m_cParametersRange.GetMax();
   }
}

/****************************************/
/****************************************/

CGenotype& CGenotype::operator=( const CGenotype& params ) {
   if( this != &params ) {
      m_cPerformance = params.m_cPerformance;
      m_cAncestors = params.m_cAncestors;
      m_cParametersRange.Set(params.m_cParametersRange.GetMin(),params.m_cParametersRange.GetMax());
      dynamic_cast<CVector<Real>&>(*this) = params;
   }

   m_pcRNG = params.m_pcRNG;
   return *this;
}


/****************************************/
/****************************************/

ostream& operator <<( ostream& os, const CGenotype& c_genotype ) {
   os << dynamic_cast<const CControlParameters&>(c_genotype);
   os << "G(" << c_genotype.m_unID << "+" << c_genotype.m_cAncestors << "+";
   os << c_genotype.m_cPerformance;
   os << ")";
   return os;
}

/****************************************/
/****************************************/

istream& operator >>( istream& is, CGenotype& c_genotype ) {
   is >> dynamic_cast<CControlParameters&>(c_genotype);
   is.ignore(2); // ignoring string "G("
   if( !(is >> c_genotype.m_unID) ) {
      THROW_ARGOSEXCEPTION("Parse error: expected genotype ID");
   }
   is.ignore(); // ignoring string "+"
   is >> c_genotype.m_cAncestors;
   is.ignore(); // ignoring string "+"
   is >> c_genotype.m_cPerformance;
   is.ignore(); // ignoring string ")"

   return is;
}
