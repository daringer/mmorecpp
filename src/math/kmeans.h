#pragma once

#include <vector>
#include <map>

#include "../core/general.h"
#include "basic.h"

namespace MM_NAMESPACE() {
  namespace MATH {

  template<typename T = float,
           typename L = std::vector<T>,
           typename M = std::map<T, L> >
  class StreamingKMeans {
   public:
    // typical STL-stuff
    typedef T value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef L list_type;
    typedef M map_type;

   private:
    // this is me
    typedef StreamingKMeans<T, L, M> class_type;



  };

}}
