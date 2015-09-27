#pragma

#include "../core/general.h"

#include "vector3d.h"

template<typename T=float. typename C=Vector3D<T> >
class tPose3D {
public:
  // STL-ize
  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference; 
  typedef C cont_type;

private:
  // me!
  typedef tPose3D<T> class_type;

  cont_type pos;
  cont_type dir;

  tPose3D(const cont_type& p, const cont_type& d) : pos(p), dir(d) { }
};

typedef tPose3D<> Pose3D; 


}}
