/* -*- Mode:C++ -*- */

#ifndef VECTOR_H
#define VECTOR_H

#include "../argos_utility/logging/argos_log.h"
#include "../argos_utility/configuration/argos_exception.h"

using namespace argos;
using namespace std;

#include <vector>

template<typename T> class CVector {
protected:
   vector<T> m_vecElements;
   
public:
   CVector() {};

   CVector( const UInt32 un_vector_size ) {m_vecElements.resize(un_vector_size);};
   
   CVector( const CVector& c_vector ) {
      if( this != &c_vector ) {
	 m_vecElements.resize(c_vector.m_vecElements.size());
	 std::copy( c_vector.m_vecElements.begin(), c_vector.m_vecElements.end(), m_vecElements.begin() );
      }
   };

   CVector( const vector<T>& t_vector ) {
      m_vecElements.resize(t_vector.size());
      std::copy( t_vector.begin(), t_vector.end(), m_vecElements.begin() );
   };

   CVector( const UInt32& un_vector_size, const T* pt_elements ) {
      m_vecElements.resize(un_vector_size);
      std::copy( pt_elements, pt_elements + un_vector_size, m_vecElements.begin() );
   }

   ~CVector() {
      m_vecElements.clear();
   };
   
   inline const UInt32 GetSize() const {return m_vecElements.size();};
   inline void SetSize( const UInt32 un_size ) {m_vecElements.resize(un_size);};

   inline void Insert( const T& un_member ) {m_vecElements.push_back(un_member);}
   inline void Insert( const UInt32 un_index, const T& t_element ) {m_vecElements[un_index] = t_element;};
   inline void Insert( const UInt32& un_vector_size, const T* pt_elements ) {
      m_vecElements.resize(un_vector_size);
      std::copy( pt_elements, pt_elements + un_vector_size, m_vecElements.begin() );
   }

   inline void Reset() {m_vecElements.clear();}
   inline vector<T>& GetValues() {return m_vecElements;};

   inline const T GetElement(const UInt32 un_index) const {return m_vecElements[un_index];};
   inline T operator[](const UInt32 un_index) const {return m_vecElements[un_index];};
   inline T& operator[](const UInt32 un_index) {return m_vecElements[un_index];};
   
   template <typename U> 
   CVector& operator +=( const CVector<U>& c_obj ) {
      ARGOS_ASSERT(m_vecElements.size() == c_obj.m_vecElements.size(), 
		   "CVector error: adding objects of different size: " << m_vecElements.size() << " and " << c_obj.m_vecElements.size() );
      for( UInt32 i = 0; i < m_vecElements.size(); ++i ) {
	 m_vecElements[i] += c_obj.m_vecElements[i];
      }
      return *this;
   };

   CVector& operator *=( const Real& factor ) {
      for( UInt32 i = 0; i < m_vecElements.size(); ++i ) {
	 m_vecElements[i] *= factor;
      }
      return *this;
   };

   CVector& operator /=( const Real& factor ) {
      (*this) *= 1/factor;
      return *this;
   }


   CVector& operator=(const CVector& c_vector ) {
      if( this != &c_vector ) {
	 m_vecElements.resize(c_vector.m_vecElements.size());
	 std::copy( c_vector.m_vecElements.begin(), c_vector.m_vecElements.end(), m_vecElements.begin() );
      }
      return *this;
   }

   friend ostream& operator <<( ostream& os, const CVector& c_vector ) {
      os << c_vector.GetSize() << "(";
      for( UInt32 i = 0; i < c_vector.GetSize(); i++ ) {
	 os << c_vector.m_vecElements[i];
	 if( i < c_vector.GetSize()-1 ) os << ",";
      }
      os << ")";
      return os;
   };

   
   void Mean( vector<CVector>& v_list ) {
      for( UInt32 i = 0; i < v_list.size(); ++i ) {
	 *this += v_list[i];
      }
      *this /= v_list.size();
   }



   friend std::istream& operator>>(std::istream& is, CVector& c_vector ) {
      // parse the number of elements and set the vector size
      UInt32 un_size;
      is >> un_size;
      c_vector.m_vecElements.resize(un_size);

      // parse the elements list
      is.ignore(); // ignore the string "("
      for( UInt32 i = 0; i < un_size; ++i ) {
	 is >> c_vector.m_vecElements[i];
	 is.ignore(); // ignore the character separator --- ',' or ')' for the last element
      }
      return is;
   };

};


#endif


