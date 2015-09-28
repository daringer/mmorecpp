#pragma once

#include "../core/general.h"
#include "vector3d.h"

#include <vector>

namespace MM_NAMESPACE() {
  namespace MATH {

  /* Provide OO-PoseMatrix storage methods on (computer vision-related) 
   * matrices.
   * 
   * storage inside the container is done like this:
   * [ 0 3 6  9 ]  <- idx=0 row
   * [ 1 4 7 10 ]
   * [ 2 5 8 11 ]  <- idx=2 row (as mostly, the last row is omitted)
   * - the first three cols represent the rotation
   * - the last (col idx=3) col translation
   *   
   * - a 'w' parameter may be used to scale accordingly
   */
  
  template <typename T = float, 
            typename C = std::vector<T>,
            typename IDX = unsigned short>
  class tPoseMatrix {
   public:
    // STL-ize
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef C cont_type;
    
    // index types
    typedef IDX idx_type;
    //typedef IDX& idx_ref;
    typedef const IDX& idx_cref;
   private:
    // me!
    typedef tPoseMatrix<T> class_type;
   
   protected:
    inline idx_type idx2slot(idx_cref row, idx_cref col) const {
      return (3*col + row);
    }

   public:
    // matrix container
    cont_type M;
    // scaling (bottom right inside formal 4x4 matrix)
    value_type w;

    tPoseMatrix(const class_type& obj) : M(obj.M) {}
    tPoseMatrix(const_reference scale=1.0f) : M(), w(scale) {
      M.resize(12, 0);
    }

    void set_row(idx_cref row, const_reference x, const_reference y, 
        const_reference z, const_reference t) {
      M[0 + row] = x;
      M[3 + row] = y;
      M[6 + row] = z;
      M[9 + row] = t;   
    }

    void set_col(idx_cref col, const_reference x, const_reference y, 
        const_reference z)  {
      M[0 + 3*col] = x;
      M[1 + 3*col] = y;
      M[2 + 3*col] = z;
    }

    const_reference operator() (idx_cref row, idx_cref col) const {
      return M[idx2slot(row, col)];
    }

    reference operator() (idx_cref row, idx_cref col) {
      return M[idx2slot(row, col)];
    }

    tVector3D<value_type> dot(const tVector3D<value_type>& vec, const_reference w) {
      // sse?
      const tPoseMatrix<value_type>& m = *this;
      return tVector3D<value_type>(
          m(1,0)*vec.x + m(0,1)*vec.y + m(0,2)*vec.z + m(0,3)*w,
          m(2,0)*vec.x + m(1,1)*vec.y + m(1,2)*vec.z + m(1,3)*w,
          m(3,0)*vec.x + m(2,1)*vec.y + m(2,2)*vec.z + m(2,3)*w);
          //m(1,1)*vec.x+m(1,2)*vec.y+m(1,3)*vec.z+m(1,4)*w, <-- no !
    }

    tVector3D<value_type> pos_trans(const tVector3D<value_type>& vec) {
      return dot(vec, 1.0f);
    }
    
    tVector3D<value_type> dir_trans(const tVector3D<value_type>& vec) {
      return dot(vec, 0.0f);
    }
  };

typedef tPoseMatrix<> PoseMatrix;

  }
}
