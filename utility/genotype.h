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
 private:
   // variables for the random number generation
   CRandom::CRNG*  m_pcRNG;

   UInt32 m_unID;
   CObjectives m_cPerformance;
   CVector<UInt32> m_cAncestors;
   
 public:
   CGenotype();
   CGenotype( const CGenotype& c_genotype );
   CGenotype( UInt32 un_num_values, const Real* pf_values, const CRange<Real>& param_range = CRange<Real>(0,1) );
   CGenotype( const vector<Real>& params, const CRange<Real>& param_range = CRange<Real>(0,1) );
   CGenotype( CRandom::CRNG* pc_RNG, UInt32 un_num_values, const CRange<Real>& param_range );
   ~CGenotype();

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

   CGenotype& operator=(const CGenotype& c_genotype );

   friend ostream& operator <<( ostream& os, const CGenotype& c_genotype );
   friend istream& operator >>( istream& is, CGenotype& c_genotype );

   friend bool operator <( const CGenotype& c_g1, const CGenotype& c_g2 ) {return c_g1.m_cPerformance[0] < c_g2.m_cPerformance[0];}
   friend bool operator >( const CGenotype& c_g1, const CGenotype& c_g2 ) {return c_g1.m_cPerformance[0] > c_g2.m_cPerformance[0];}

};


typedef vector<CGenotype> TVecIndividuals;



#endif
