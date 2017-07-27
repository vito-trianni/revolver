#ifndef _CGENOTYPE_H_
#define _CGENOTYPE_H_

#include <utility/control_parameters.h>
#include <utility/objectives.h>
#include <utility/vector.h>
#include <argos_utility/math/range.h>
#include <argos_utility/math/rng.h>

#include <utility>


using namespace std;
using namespace argos;



// simple class to store control parameters from real-valued genotypes
class CGenotype : public CControlParameters {
 protected:
   // variables for the random number generation 
   CRandom::CRNG*  m_pcRNG;
 
 private:
 
   UInt32 m_unID;
   CObjectives m_cPerformance;
   CVector<UInt32> m_cAncestors;
   
   CControlParameters m_cAlleles1;
   CControlParameters m_cAlleles2;
    
   UInt32 m_unDominanceType;
   
   bool m_bIsDiploid;
   
 public:
   CGenotype();
   CGenotype( const CGenotype& c_genotype );
   CGenotype( UInt32 un_num_values, const Real* pf_values_alleles1, const Real* pf_values_alleles2, const CRange<Real>& param_range = CRange<Real>(0,1) );
   CGenotype( const vector<Real>& params_allele1, const vector<Real>& params_allele2, const CRange<Real>& param_range = CRange<Real>(0,1) );
   CGenotype( CRandom::CRNG* pc_RNG, UInt32 un_num_values, const CRange<Real>& param_range );
   ~CGenotype();
   
   inline void SetDiploid(){m_bIsDiploid = true;};
   inline void SetHaploid(){m_bIsDiploid = false;};
   inline bool IsDiploid(){return m_bIsDiploid;};
   
   inline CControlParameters& GetAlleles1() { return m_cAlleles1;  };
   inline CControlParameters& GetAlleles2() { return m_cAlleles2;  };
   
   inline void SetAlleles1(CControlParameters& c_alleles_1){m_cAlleles1 = c_alleles_1;};
   inline void SetAlleles2(CControlParameters& c_alleles_2){m_cAlleles2 = c_alleles_2;};
    
   
   inline const UInt32 GetSize() const {
    if(m_bIsDiploid){
     if(m_cAlleles1.GetSize() != m_cAlleles2.GetSize()) {
      LOGERR << "[ERROR] Very weird diploid alleles with different sizes. ";
       exit(-1);
      }
     }
     return m_cAlleles1.GetSize();
   };
   
   inline vector<Real>& GetValues() {
    if(!m_bIsDiploid){
     return m_cAlleles1.GetValues();
    }
    else{
     LOGERR << "[GENOTYPE] The method GetValues() should not be called directly if one is dealing with a diploid genotype" << std::endl;
     exit(-1);
     return m_cAlleles1.GetValues();
    }
   };
   
   inline vector<Real>& GetPhenotype(){
	   return CVector::GetValues();
   }
   
   inline void SetDominanceType(const UInt32 un_dominance_type){m_unDominanceType = un_dominance_type;};
   
   void GenotypeToPhenotypeMapping();

   inline void SetRNG( CRandom::CRNG* pc_rng ) { m_pcRNG = pc_rng; };
   
   inline const UInt32 GetID() const {return m_unID;};
   inline void SetID( const UInt32 un_id ) {m_unID = un_id;};

   inline const CObjectives& GetPerformance() { return m_cPerformance;};
   inline void SetPerformance( const CObjectives& c_objs ) {m_cPerformance = c_objs;};
   
   void MutateNormal( const Real f_mutation_variance );
   void MutateNormalWithProbability(const Real f_mutation_variance, const Real f_mutation_probability);
   
   void CutOff();
   void CutOffMin();
   void CutOffMax();
   inline void Reset() {m_cPerformance.Reset(); m_cAncestors.Reset();};
   inline void InsertAncestor(UInt32 un_index) {m_cAncestors.Insert(un_index);};
   inline const CVector<UInt32>& GetAncestors() { return m_cAncestors;};
   inline void SetAncestors(const CVector<UInt32> c_ancestors) { m_cAncestors = c_ancestors;};

   CGenotype& operator=(const CGenotype& c_genotype );

   friend ostream& operator <<( ostream& os, const CGenotype& c_genotype );
   friend istream& operator >>( istream& is, CGenotype& c_genotype );

   friend bool operator <( const CGenotype& c_g1, const CGenotype& c_g2 ) {return c_g1.m_cPerformance[0] < c_g2.m_cPerformance[0];}
   friend bool operator >( const CGenotype& c_g1, const CGenotype& c_g2 ) {return c_g1.m_cPerformance[0] > c_g2.m_cPerformance[0];}

};


typedef vector<CGenotype> TVecIndividuals;



#endif
