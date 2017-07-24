#include "diploid_genotype.h"

const string DIPLOID_DOMINANCE_TYPE_DOMINANCE                  = "dominance";
const string DIPLOID_DOMINANCE_TYPE_CODOMINANCE                = "codominance";


/****************************************/
/****************************************/

CDiploidGenotype::CDiploidGenotype(): CGenotype(){}

/****************************************/
/****************************************/

CDiploidGenotype::CDiploidGenotype( CDiploidGenotype& c_genotype ) :
   CGenotype( c_genotype )
{
   SetAlleles1(c_genotype.GetAlleles1());
   SetAlleles2(c_genotype.GetAlleles2());
}

/****************************************/
/****************************************/

CDiploidGenotype::CDiploidGenotype( UInt32 un_num_values, const Real* pf_values_alleles1, const Real* pf_values_alleles2, const CRange<Real>& param_range )
{
    SetRange(param_range);
    CControlParameters cAlleles1(un_num_values, pf_values_alleles1, param_range);
    SetAlleles1(cAlleles1);
    CControlParameters cAlleles2(un_num_values, pf_values_alleles2, param_range);
    SetAlleles2(cAlleles2);
}

/****************************************/
/****************************************/


CDiploidGenotype::CDiploidGenotype( const vector<Real>& params_alleles1, vector<Real>& params_alleles2, const CRange<Real>& param_range )
{
    SetRange(param_range);
    CControlParameters cAlleles1( params_alleles1, param_range );
    SetAlleles1(cAlleles1);
    CControlParameters cAlleles2( params_alleles2, param_range );
    SetAlleles2(cAlleles2);
}

/****************************************/
/****************************************/


CDiploidGenotype::CDiploidGenotype( CRandom::CRNG* pc_RNG, UInt32 un_num_values, const CRange<Real>& param_range )
{
   SetRNG(pc_RNG);
   SetID(0);
   
   for( UInt32 i = 0; i < un_num_values; i++ ) {
      m_cAlleles1.Insert(m_pcRNG->Uniform(param_range));
      m_cAlleles2.Insert(m_pcRNG->Uniform(param_range));
   }

   SetRange( param_range );
}

/****************************************/
/****************************************/


CDiploidGenotype::~CDiploidGenotype() {
}

/****************************************/
/****************************************/

void CDiploidGenotype::GenotypeToPhenotypeMapping(){
   for(int i = 0; i < m_vecElements.size() ; ++i){
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
   
}

/****************************************/
/****************************************/

void CDiploidGenotype::MutateNormal( const Real f_mutation_variance ) {
   for( UInt32 i = 0; i < GetSize(); ++i ) {
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

void CDiploidGenotype::MutateNormalWithProbability( const Real f_mutation_variance, const Real f_mutation_probability ) {
   for( UInt32 i = 0; i < GetSize(); ++i ) {
      Real fMutateRandom = m_pcRNG->Uniform(CRange<Real>(0.0,1.0));
      if(fMutateRandom < f_mutation_probability){
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

void CDiploidGenotype::CutOff() {
   for( UInt32 i = 0; i < GetSize(); ++i ) {
       
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

void CDiploidGenotype::CutOffMin() {
   for( UInt32 i = 0; i < GetSize(); ++i ) {
      if (m_cAlleles1.GetElement(i) < m_cParametersRange.GetMin()) m_cAlleles1.Insert(i,m_cParametersRange.GetMin());
      if (m_cAlleles2.GetElement(i) < m_cParametersRange.GetMin()) m_cAlleles2.Insert(i,m_cParametersRange.GetMin());
   }
}

/****************************************/
/****************************************/

void CDiploidGenotype::CutOffMax() {
   for( UInt32 i = 0; i < GetSize(); ++i ) {
      if (m_cAlleles1.GetElement(i) > m_cParametersRange.GetMax()) m_cAlleles1.Insert(i,m_cParametersRange.GetMax());
      if (m_cAlleles2.GetElement(i) > m_cParametersRange.GetMax()) m_cAlleles2.Insert(i,m_cParametersRange.GetMax());
   }
}

/****************************************/
/****************************************/

CDiploidGenotype& CDiploidGenotype::operator=( CDiploidGenotype& params ) {
   CGenotype::operator=(params);
   m_cAlleles1 = params.GetAlleles1();
   m_cAlleles2 = params.GetAlleles2();
   return *this;
}


/****************************************/
/****************************************/

ostream& operator <<( ostream& os, CDiploidGenotype& c_genotype ) {
   os << dynamic_cast<const CVector<Real>&>(c_genotype.GetAlleles1());
   os << dynamic_cast<const CVector<Real>&>(c_genotype.GetAlleles2());
   os << "(" << c_genotype.GetRange() << ")";
   
   os << "G(" << c_genotype.GetID() << "+" << c_genotype.GetAncestors() << "+";
   os << c_genotype.GetPerformance();
   os << ")";
   return os;
}

/****************************************/
/****************************************/

istream& operator >>( istream& is, CDiploidGenotype& c_genotype ) {
   is >> dynamic_cast<CVector<Real>&>(c_genotype.GetAlleles1());
   is >> dynamic_cast<CVector<Real>&>(c_genotype.GetAlleles2());
   is.ignore(); // ignore the string "("
   CRange<Real> cParametersRange;
   is >> cParametersRange;
   c_genotype.SetRange(cParametersRange);
   is.ignore(); // ignore the string ")"
   
   is.ignore(2); // ignoring string "G("
   UInt32 unID;
   if( !(is >> unID) ) {
      THROW_ARGOSEXCEPTION("Parse error: expected genotype ID");
   }
   c_genotype.SetID(unID);
   is.ignore(); // ignoring string "+"
   CVector<UInt32> cAncestors;
   is >> cAncestors;
   c_genotype.SetAncestors(cAncestors);
   
   is.ignore(); // ignoring string "+"
   CObjectives cPerformance;
   is >> cPerformance;
   c_genotype.SetPerformance(cPerformance);
   is.ignore(); // ignoring string ")"

   return is;
}