#ifndef _CDIPLOIDGENOTYPE_H_
#define _CDIPLOIDGENOTYPE_H_

#include <utility/genotype.h>

using namespace std;
using namespace argos;

// class that stores two copies of control parameters

class CDiploidGenotype : public CGenotype {
    protected:
    CControlParameters m_cAlleles1;
    CControlParameters m_cAlleles2;
    
    string m_sDominanceType;
    
    public:
    CDiploidGenotype();
       
    CDiploidGenotype( CDiploidGenotype& c_genotype );
    CDiploidGenotype( UInt32 un_num_values, const Real* pf_values_alleles1, const Real* pf_values_alleles2, const CRange<Real>& param_range = CRange<Real>(0,1) );
    CDiploidGenotype( const vector<Real>& params_alleles1, vector<Real>& params_alleles2, const CRange<Real>& param_range = CRange<Real>(0,1) );
    CDiploidGenotype( CRandom::CRNG* pc_RNG, UInt32 un_num_values, const CRange<Real>& param_range );
    ~CDiploidGenotype();
    
    inline CControlParameters& GetAlleles1() { return m_cAlleles1;  };
    inline CControlParameters& GetAlleles2() { return m_cAlleles2;  };
    
    inline const UInt32 GetSize() const {
        if(m_cAlleles1.GetSize() != m_cAlleles2.GetSize()) {
            LOGERR << "[ERROR] Very weird diploid alleles with different sizes. ";
            exit(-1);
        } 
        return m_cAlleles1.GetSize();
    };
    
    inline void SetAlleles1(CControlParameters& c_alleles_1){m_cAlleles1 = c_alleles_1;};
    inline void SetAlleles2(CControlParameters& c_alleles_2){m_cAlleles2 = c_alleles_2;};
    
    inline void SetDominanceType(const string s_dominance_type){m_sDominanceType = s_dominance_type;};
    
    void GenotypeToPhenotypeMapping();
    
    void MutateNormal( const Real f_mutation_variance );
    void MutateNormalWithProbability(const Real f_mutation_variance, const Real f_mutation_probability);
   
    void CutOff();
    void CutOffMin();
    void CutOffMax();
    
    CDiploidGenotype& operator=( CDiploidGenotype& c_genotype );
    
    friend ostream& operator <<( ostream& os, CGenotype& c_genotype );
    friend istream& operator >>( istream& is, CGenotype& c_genotype );
    
};


#endif