#pragma once

#include <math.h>

#include "../core/general.h"

#include "basic.h"


namespace MM_NAMESPACE() {
  namespace MATH {

template<typename T = float>
class tVector3D {
public:
  // STL-ize
  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference;

private:
  // me!
  typedef tVector3D<T> class_type;
  
  // some used numbers
  //const value_type zero;
  //const value_type one;

   public:
    value_type x;
    value_type y;
    value_type z;

    tVector3D(const class_type& obj) : x(obj.x), y(obj.y), z(obj.z) {}
    tVector3D(const_reference x, const_reference y, const_reference z)
        : x(x), y(y), z(z) {}

    /* angle to another Vector3D - TODO: NEON!*/
    value_type angle_to(const class_type& other) const {
      return Vector3D(acos(x*other.x+y*other.y+z*other.z) /
          (sqrt(x*x + y*y + z*z) * 
           sqrt(other.x*other.x + other.y*other.y + other.z*other.z)));
    }
    value_type angle_to_deg(const class_type& other) const {
      return angle_to(other) * TO_DEG_F;
    }

    /* cross/(outer)-product and... */
    class_type cross(const class_type& other) const {
      // TODO: mmh NEON! ?
      return class_type(
          y*other.z - z*other.y, 
          z*other.x - x*other.z,
          x*other.y - y*other.x);
    }
    /* dot/(inner)-product - first vector */
    value_type dot(const class_type& other) const {
      return x*other.x + y*other.y + z*other.z;
    }

    /* will lead to problems with floatingpoints! */
    bool operator==(const class_type& other) const {
      return x==other.x && y==other.y && z==other.z;
    }
    bool operator!=(const class_type& other) const {
      return (!(*this == other));
    }

    /* arithmetrical stuff (scalar and Vector3D) */
    class_type operator+(const class_type& other) const {
      return class_type(x+other.x, y+other.y, z+other.z);
    }
    class_type operator+(const value_type& scalar) const {
      return class_type(x+scalar, y+scalar, z+scalar);
    }
    class_type operator-(const class_type& other) const {
      return class_type(x-other.x, y-other.y, z-other.z);
    }
    class_type operator-(const value_type& scalar) const {
      return class_type(x-scalar, y-scalar, z-scalar);
    }
    class_type operator*(const value_type& scalar) const {
      return class_type(x*scalar, y*scalar, z*scalar);
    }
    class_type operator/(const value_type& scalar) const {
      return class_type(x/scalar, y/scalar, z/scalar);
    }
};


}}
