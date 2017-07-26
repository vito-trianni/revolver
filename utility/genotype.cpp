#include "genotype.h"

const string DIPLOID_DOMINANCE_TYPE_DOMINANCE                  = "dominance";
const string DIPLOID_DOMINANCE_TYPE_CODOMINANCE                = "codominance";

/****************************************/
/****************************************/
   
CGenotype::CGenotype() :
   m_pcRNG(NULL),
   m_unID(0),
   m_bIsDiploid(false)
{}


/****************************************/
/****************************************/

CGenotype::CGenotype( const CGenotype& params ) :
   CControlParameters( params ),
   m_pcRNG(params.m_pcRNG),
   m_unID(params.m_unID),
   m_bIsDiploid(params.m_bIsDiploid),
   m_cAlleles1(params.m_cAlleles1),
   m_cAlleles2(params.m_cAlleles2)
{
   if( this != &params ) {
      m_cPerformance = params.m_cPerformance;
      m_cAncestors = params.m_cAncestors;
   }
   
}

/****************************************/
/****************************************/
   
CGenotype::CGenotype( UInt32 un_num_values, const Real* pf_values_alleles1, const Real* pf_values_alleles2, const CRange<Real>& param_range ) : 
    CControlParameters(un_num_values, pf_values_alleles1, param_range),
    m_pcRNG(NULL),
    m_cAlleles1(CControlParameters(un_num_values, pf_values_alleles1, param_range)),
    m_cAlleles2(CControlParameters(un_num_values, pf_values_alleles2, param_range)),
    m_bIsDiploid(false),
    m_unID(0)
{
}

/****************************************/
/****************************************/

CGenotype::CGenotype( const vector<Real>& params_allele1, const vector<Real>& params_allele2, const CRange<Real>& param_range ) :
   CControlParameters( params_allele1, param_range ),
   m_cAlleles1(CControlParameters(params_allele1, param_range)),
   m_cAlleles2(CControlParameters(params_allele2, param_range)),
   m_pcRNG(NULL),
   m_bIsDiploid(false),
   m_unID(0)
{
}


/****************************************/
/****************************************/

CGenotype::CGenotype( CRandom::CRNG* pc_RNG, UInt32 un_num_values, const CRange<Real>& param_range ) :
   m_pcRNG(pc_RNG),
   m_bIsDiploid(false),
   m_unID(0)
{
   for( UInt32 i = 0; i < un_num_values; i++ ) {
      m_cAlleles1.Insert(m_pcRNG->Uniform(param_range));
      m_cAlleles2.Insert(m_pcRNG->Uniform(param_range));
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
      //m_vecElements[i] += m_pcRNG->Gaussian(f_mutation_variance);
      
      Real fValue1 = m_cAlleles1.GetElement(i);
      fValue1 += m_pcRNG->Gaussian(f_mutation_variance);
      m_cAlleles1.Insert(i,fValue1);
      
      Real fValue2 = m_cAlleles2.GetElement(i);
      fValue2 += m_pcRNG->Gaussian(f_mutation_variance);
      m_cAlleles2.Insert(i,fValue2);
      
   }
}

/****************************************/
/****************************************/

void CGenotype::MutateNormalWithProbability( const Real f_mutation_variance, const Real f_mutation_probability ) {   
   for( UInt32 i = 0; i < GetSize(); ++i ) {
      Real fMutateRandom = m_pcRNG->Uniform(CRange<Real>(0.0,1.0));
      if(fMutateRandom < f_mutation_probability){
         //m_vecElements[i] += m_pcRNG->Gaussian(f_mutation_variance);
         Real fValue1 = m_cAlleles1.GetElement(i);
         fValue1 += m_pcRNG->Gaussian(f_mutation_variance);
         m_cAlleles1.Insert(i,fValue1);
      }
      fMutateRandom = m_pcRNG->Uniform(CRange<Real>(0.0,1.0));
      if(fMutateRandom < f_mutation_probability){
         Real fValue2 = m_cAlleles2.GetElement(i);
         fValue2 += m_pcRNG->Gaussian(f_mutation_variance);
         m_cAlleles2.Insert(i,fValue2); 
      }
   }
}


/****************************************/
/****************************************/

void CGenotype::CutOff() {
   for( UInt32 i = 0; i < GetSize(); ++i ) {
      //m_cParametersRange.TruncValue( m_vecElements[i] );
      
      Real fValue1 = m_cAlleles1.GetElement(i);
      m_cParametersRange.TruncValue( fValue1 );
      m_cAlleles1.Insert(i,fValue1);
      
      Real fValue2 = m_cAlleles2.GetElement(i);
      m_cParametersRange.TruncValue( fValue2 );
      m_cAlleles2.Insert(i,fValue2);
   }
}

/****************************************/
/****************************************/

void CGenotype::CutOffMin() {
   for( UInt32 i = 0; i < GetSize(); ++i ) {
      //if (m_vecElements[i] < m_cParametersRange.GetMin()) m_vecElements[i] = m_cParametersRange.GetMin();
      if (m_cAlleles1.GetElement(i) < m_cParametersRange.GetMin()) m_cAlleles1.Insert(i,m_cParametersRange.GetMin());
      if (m_cAlleles2.GetElement(i) < m_cParametersRange.GetMin()) m_cAlleles2.Insert(i,m_cParametersRange.GetMin());
   }
}

/****************************************/
/****************************************/

void CGenotype::CutOffMax() {
   for( UInt32 i = 0; i < GetSize(); ++i ) {
      //if (m_vecElements[i] > m_cParametersRange.GetMax()) m_vecElements[i] = m_cParametersRange.GetMax();
      if (m_cAlleles1.GetElement(i) > m_cParametersRange.GetMax()) m_cAlleles1.Insert(i,m_cParametersRange.GetMax());
      if (m_cAlleles2.GetElement(i) > m_cParametersRange.GetMax()) m_cAlleles2.Insert(i,m_cParametersRange.GetMax());
   }
}

/****************************************/
/****************************************/

void CGenotype::GenotypeToPhenotypeMapping(){
   
   for(int i = 0; i < m_vecElements.size() ; ++i){
      if(m_bIsDiploid){
         if (m_sDominanceType.compare(DIPLOID_DOMINANCE_TYPE_DOMINANCE) == 0){
            // Largest trait in absolute value
            if(Abs(GetAlleles1().GetElement(i)) > Abs(GetAlleles2().GetElement(i)) ){
               Insert(i,GetAlleles1().GetElement(i));
            }
            else if (Abs(GetAlleles1().GetElement(i)) < Abs(GetAlleles2().GetElement(i)) ){
               Insert(i,GetAlleles2().GetElement(i));
            }
            else{
               Real fRandomAlleleChoice = m_pcRNG->Uniform(CRange<Real>(0.0,1.0));
               if(fRandomAlleleChoice < 0.5){
                  Insert(i,GetAlleles1().GetElement(i));
               }
               else{
                  Insert(i,GetAlleles2().GetElement(i));
               }
            }
         }
         else if (m_sDominanceType.compare(DIPLOID_DOMINANCE_TYPE_CODOMINANCE) == 0){
            Insert(i,(GetAlleles1().GetElement(i) + GetAlleles2().GetElement(i)) / 2.0); // Average trait
            
         }
      }
      else{
         Insert(i,GetAlleles1().GetElement(i)); // Copy the trait for haploid genotypes
      }
   }
   
   
}

/****************************************/
/****************************************/

CGenotype& CGenotype::operator=( const CGenotype& params ) {
   if( this != &params ) {
      m_cPerformance = params.m_cPerformance;
      m_cAncestors = params.m_cAncestors;
      m_cParametersRange.Set(params.m_cParametersRange.GetMin(),params.m_cParametersRange.GetMax());
      m_cAlleles1 = params.m_cAlleles1;
      m_cAlleles2 = params.m_cAlleles2;
      m_bIsDiploid = params.m_bIsDiploid;
      dynamic_cast<CVector<Real>&>(*this) = params;
   }

   m_pcRNG = params.m_pcRNG;
   return *this;
}


/****************************************/
/****************************************/

ostream& operator <<( ostream& os, const CGenotype& c_genotype ) {
   //os << dynamic_cast<const CControlParameters&>(c_genotype);
   os << dynamic_cast<const CVector<Real>&>(c_genotype.m_cAlleles1);
   if(c_genotype.m_bIsDiploid){
      os << dynamic_cast<const CVector<Real>&>(c_genotype.m_cAlleles2);
   }
   os << "G(" << c_genotype.m_unID << "+" << c_genotype.m_cAncestors << "+";
   os << c_genotype.m_cPerformance;
   os << ")";
   return os;
}

/****************************************/
/****************************************/

istream& operator >>( istream& is, CGenotype& c_genotype ) {
   //is >> dynamic_cast<CControlParameters&>(c_genotype);
   is >> dynamic_cast<CVector<Real>&>(c_genotype.m_cAlleles1);
   if(c_genotype.m_bIsDiploid){
      is >> dynamic_cast<CVector<Real>&>(c_genotype.m_cAlleles2);
   }
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
