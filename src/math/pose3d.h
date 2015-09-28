#pragma once

#include "../core/general.h"

#include "vector3d.h"

namespace MM_NAMESPACE() {
  namespace MATH {

  template <typename T = float, typename C = tVector3D<T> >
  class tPose3D {
   public:
    // STL-ize
    typedef T value_type;
    typedef const T& const_reference;
    typedef C cont_type;

   private:
    // me!
    typedef tPose3D<T> class_type;

   public:
    cont_type pos;
    cont_type dir;

    tPose3D(const cont_type& p, const cont_type& d) : pos(p), dir(d) {}
  };
  }
}
