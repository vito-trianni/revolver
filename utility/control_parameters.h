#ifndef _CCONTROLPARAMETERS_H_
#define _CCONTROLPARAMETERS_H_

#include "../argos_utility/math/range.h"
#include "../argos_utility/logging/argos_log.h"
#include "../argos_utility/datatypes/datatypes.h"
#include "../argos_utility/configuration/argos_exception.h"

#include <fstream>
#include "vector.h"

using namespace std;
using namespace argos;

// simple class to store control parameters from real-valued genotypes
class CControlParameters : public CVector<Real> {
 protected:
   CRange<Real> m_cParametersRange;

 public:
   CControlParameters();
   CControlParameters( const CControlParameters& params );
   CControlParameters( UInt32 un_num_values, const Real* pf_values, const CRange<Real>& c_range = CRange<Real>(0,1) );
   CControlParameters( const vector<Real>& params, const CRange<Real>& c_range = CRange<Real>(0,1) );
   virtual ~CControlParameters() {};
   
   CControlParameters&  operator=(const CControlParameters& c_params );
   // inline void CopyParameters( double* params ) const {std::copy( m_vecParamters.begin(), m_vecParamters.end(), params );};

   inline const CRange<Real>& GetRange() const {return m_cParametersRange;};
   inline void SetRange( const CRange<Real>& c_range ) { m_cParametersRange.Set(c_range.GetMin(),c_range.GetMax()); };

   friend ostream& operator<<( ostream& os, const CControlParameters& c_control_parameters );
   friend istream& operator>>(istream& is, CControlParameters& c_control_parameters );
};




#endif
