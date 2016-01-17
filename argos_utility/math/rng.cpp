/**
 * @file <argos3/core/utility/math/rng.cpp>
 *
 * @brief This file provides the definition of
 *
 * @author Carlo Pinciroli - <ilpincy@gmail.com>
 */

#include "rng.h"
#include "../configuration/argos_exception.h"
#include "../logging/argos_log.h"
#include <cstring>
#include <limits>
#include <cmath>
#include <gsl/gsl_randist.h>

namespace argos {

   /****************************************/
   /****************************************/

   std::map<std::string, CRandom::CCategory*> CRandom::m_mapCategories;
   const gsl_rng_type** CRandom::m_pptRNGTypes = gsl_rng_types_setup();

   /* Checks that a category exists. It internally creates an iterator that points to the category, if found.  */
#define CHECK_CATEGORY(category)                                        \
   std::map<std::string, CCategory*>::iterator itCategory = m_mapCategories.find(category); \
   if(itCategory == m_mapCategories.end()) {                            \
      THROW_ARGOSEXCEPTION("CRandom:: can't find category \"" << category << "\"."); \
   }

   /****************************************/
   /****************************************/

   CRandom::CRNG::CRNG(UInt32 un_seed,
                       const std::string& str_type) :
      m_unSeed(un_seed),
      m_strType(str_type),
      m_ptRNG(NULL),
      m_pcIntegerRNGRange(NULL) {
      CreateRNG();
   }
   
   /****************************************/
   /****************************************/
   
   CRandom::CRNG::CRNG(const CRNG& c_rng) :
      m_unSeed(c_rng.m_unSeed),
      m_strType(c_rng.m_strType),
      m_ptRNG(NULL),
      m_pcIntegerRNGRange(new CRange<UInt32>(*c_rng.m_pcIntegerRNGRange)) {
      /* Clone RNG of original */
      m_ptRNG = gsl_rng_clone(c_rng.m_ptRNG);
   }

   /****************************************/
   /****************************************/

   CRandom::CRNG::~CRNG() {
      DisposeRNG();
   }

   /****************************************/
   /****************************************/

   void CRandom::CRNG::CreateRNG() {
      /* Look for RNG type in the RNG type list */
      bool bFound = false;
      const gsl_rng_type** pptRNGType = GetRNGTypes();
      while((!bFound) && (pptRNGType != NULL)) {
         if(m_strType == (*pptRNGType)->name) {
            bFound = true;
         }
         else {
            ++pptRNGType;
         }
      }
      if(!bFound) {
         /* RNG type not found, error! */
         THROW_ARGOSEXCEPTION("Unknown random number generator type '" << m_strType << "'.");
      }
      /* We found the wanted RNG type, create the actual RNG */
      m_ptRNG = gsl_rng_alloc(*pptRNGType);
      gsl_rng_set(m_ptRNG, m_unSeed);
      /* Initialize RNG range */
      m_pcIntegerRNGRange = new CRange<UInt32>(gsl_rng_min(m_ptRNG),
                                               gsl_rng_max(m_ptRNG));
   }

   /****************************************/
   /****************************************/

   void CRandom::CRNG::DisposeRNG() {
      gsl_rng_free(m_ptRNG);
      delete m_pcIntegerRNGRange;
   }

   /****************************************/
   /****************************************/

   void CRandom::CRNG::Reset() {
      gsl_rng_set(m_ptRNG, m_unSeed);
   }
   
   /****************************************/
   /****************************************/

   bool CRandom::CRNG::Bernoulli(Real f_true) {
      return gsl_rng_uniform(m_ptRNG) < f_true;
   }

   /****************************************/
   /****************************************/

   CRadians CRandom::CRNG::Uniform(const CRange<CRadians>& c_range) {
      return c_range.GetMin() + gsl_rng_uniform(m_ptRNG) * c_range.GetSpan();
   }
   
   /****************************************/
   /****************************************/

   Real CRandom::CRNG::Uniform(const CRange<Real>& c_range) {
      return c_range.GetMin() + gsl_rng_uniform(m_ptRNG) * c_range.GetSpan();
   }
   
   /****************************************/
   /****************************************/

   SInt32 CRandom::CRNG::Uniform(const CRange<SInt32>& c_range) {
      SInt32 nRetVal;
      do {
         m_pcIntegerRNGRange->MapValueIntoRange(nRetVal, gsl_rng_get(m_ptRNG), c_range);
      } while(nRetVal == c_range.GetMax());
      return nRetVal;
   }
   
   /****************************************/
   /****************************************/

   UInt32 CRandom::CRNG::Uniform(const CRange<UInt32>& c_range) {
      UInt32 unRetVal;
      do {
         m_pcIntegerRNGRange->MapValueIntoRange(unRetVal, gsl_rng_get(m_ptRNG), c_range);
      } while(unRetVal == c_range.GetMax());
      return unRetVal;
   }
   
   /****************************************/
   /****************************************/

   Real CRandom::CRNG::Exponential(Real f_mean) {
      return gsl_ran_exponential(m_ptRNG, f_mean);
   }
   
   /****************************************/
   /****************************************/

   Real CRandom::CRNG::Gaussian(Real f_std_dev,
                                Real f_mean) {
      return f_mean + gsl_ran_gaussian(m_ptRNG, f_std_dev);
   }

   /****************************************/
   /****************************************/

   Real CRandom::CRNG::Rayleigh(Real f_sigma) {
      return gsl_ran_rayleigh(m_ptRNG, f_sigma);
   }

/****************************************/
   /****************************************/

   Real CRandom::CRNG::Lognormal(Real f_sigma, Real f_mu) {
      return gsl_ran_lognormal(m_ptRNG, f_mu, f_sigma);
   }
   
   /****************************************/
   /****************************************/

   CRandom::CCategory::CCategory(const std::string& str_id,
                                 UInt32 un_seed) :
      m_strId(str_id),
      m_unSeed(un_seed),
      m_cSeeder(un_seed),
      m_cSeedRange(1, std::numeric_limits<UInt32>::max()) {}

   /****************************************/
   /****************************************/

   CRandom::CCategory::~CCategory() {
      while(! m_vecRNGList.empty()) {
         delete m_vecRNGList.back();
         m_vecRNGList.pop_back();
      }
   }

   /****************************************/
   /****************************************/

   void CRandom::CCategory::SetSeed(UInt32 un_seed) {
      m_unSeed = un_seed;
      m_cSeeder.SetSeed(m_unSeed);
   }

   /****************************************/
   /****************************************/

   CRandom::CRNG* CRandom::CCategory::CreateRNG(const std::string& str_type) {
      /* Get seed from internal RNG */
      UInt32 unSeed = m_cSeeder.Uniform(m_cSeedRange);
      /* Create new RNG */
      m_vecRNGList.push_back(new CRNG(unSeed, str_type));
      return m_vecRNGList.back();
   }

   /****************************************/
   /****************************************/

   void CRandom::CCategory::ResetRNGs() {
      /* Reset internal RNG */
      m_cSeeder.Reset();
      ReseedRNGs();
      /* Reset the RNGs */
      for(size_t i = 0; i < m_vecRNGList.size(); ++i) {
         m_vecRNGList[i]->Reset();
      }
   }

   /****************************************/
   /****************************************/

   void CRandom::CCategory::ReseedRNGs() {
      for(size_t i = 0; i < m_vecRNGList.size(); ++i) {
         /* Get seed from internal RNG */
         m_vecRNGList[i]->SetSeed(m_cSeeder.Uniform(m_cSeedRange));
      }
   }

   /****************************************/
   /****************************************/

   bool CRandom::CreateCategory(const std::string& str_category,
                                UInt32 un_seed) {
      /* Is there a category already? */
      std::map<std::string, CCategory*>::iterator itCategory = m_mapCategories.find(str_category);
      if(itCategory == m_mapCategories.end()) {
         /* No, create it */
         m_mapCategories.insert(
            std::pair<std::string,
            CRandom::CCategory*>(str_category,
                                 new CRandom::CCategory(str_category,
                                                        un_seed)));
         return true;
      }
      return false;
   }

   /****************************************/
   /****************************************/

   CRandom::CCategory& CRandom::GetCategory(const std::string& str_category) {
      CHECK_CATEGORY(str_category);
      return *(itCategory->second);
   }

   /****************************************/
   /****************************************/

   bool CRandom::ExistsCategory(const std::string& str_category) {
      try {
         CHECK_CATEGORY(str_category);
         return true;
      }
      catch(CARGoSException& ex) {
         return false;
      }
   }

   /****************************************/
   /****************************************/

   void CRandom::RemoveCategory(const std::string& str_category) {
      CHECK_CATEGORY(str_category);
      delete itCategory->second;
      m_mapCategories.erase(itCategory);
   }

   /****************************************/
   /****************************************/

   CRandom::CRNG* CRandom::CreateRNG(const std::string& str_category,
                                     const std::string& str_type) {
      CHECK_CATEGORY(str_category);
      return itCategory->second->CreateRNG(str_type);
   }
   
   /****************************************/
   /****************************************/

   UInt32 CRandom::GetSeedOf(const std::string& str_category) {
      CHECK_CATEGORY(str_category);
      return itCategory->second->GetSeed();
   }

   /****************************************/
   /****************************************/

   void CRandom::SetSeedOf(const std::string& str_category,
                           UInt32 un_seed) {
      CHECK_CATEGORY(str_category);
      itCategory->second->SetSeed(un_seed);
   }

   /****************************************/
   /****************************************/

   void CRandom::Reset() {
      for(std::map<std::string, CCategory*>::iterator itCategory = m_mapCategories.begin();
          itCategory != m_mapCategories.end();
          ++itCategory) {
         itCategory->second->ResetRNGs();
      }
   }

   /****************************************/
   /****************************************/

}
