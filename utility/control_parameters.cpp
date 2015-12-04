#include "control_parameters.h"


/****************************************/
/****************************************/
   
CControlParameters::CControlParameters() :
   CVector<Real>(),
   m_cParametersRange(0,1)
{}


/****************************************/
/****************************************/

CControlParameters::CControlParameters( const CControlParameters& params ) :
   CVector<Real>( params ),
   m_cParametersRange(params.m_cParametersRange.GetMin(),params.m_cParametersRange.GetMax())
{}

/****************************************/
/****************************************/
   
CControlParameters::CControlParameters( UInt32 un_num_values, const Real* pf_values, const CRange<Real>& c_range ) :
   CVector<Real>(un_num_values, pf_values),
   m_cParametersRange(c_range.GetMin(),c_range.GetMax())
{}


/****************************************/
/****************************************/

CControlParameters::CControlParameters( const vector<Real>& params, const CRange<Real>& c_range ) :
   CVector<Real>(params),
   m_cParametersRange(c_range.GetMin(),c_range.GetMax())
{}


/****************************************/
/****************************************/

CControlParameters& CControlParameters::operator=( const CControlParameters& params ) {
   m_cParametersRange.Set(params.m_cParametersRange.GetMin(),params.m_cParametersRange.GetMax());
   dynamic_cast<CVector<Real>&>(*this) = params;
   return *this;
}


/****************************************/
/****************************************/

ostream& operator <<( ostream& os, const CControlParameters& c_control_parameters ) {
   os << dynamic_cast<const CVector<Real>&>(c_control_parameters);
   os << "(" << c_control_parameters.m_cParametersRange << ")";
   return os;
}

/****************************************/
/****************************************/

istream& operator >>( istream& is, CControlParameters& c_control_parameters ) {
   is >> dynamic_cast<CVector<Real>&>(c_control_parameters);

   is.ignore(); // ignore the string "("
   is >> c_control_parameters.m_cParametersRange;
   is.ignore(); // ignore the string ")"
   return is;
}

