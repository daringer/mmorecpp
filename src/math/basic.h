#pragma once

#include <cmath>

#include <iterator>
#include <algorithm>

#include "../core/general.h"
#include "../core/exception.h"



namespace MM_NAMESPACE() {
  namespace MATH {

DEFINE_EXCEPTION(EmptyDataContainerError, TOOLS::BaseException)

/*
// use this to infere template param based on class-constructor
template<typename T>
class wrapped_mean {
  public:

  typedef typename T::const_iterator tIter;
  typedef typename T::value_type tValue;

  const T& container;

  wrapped_mean(const T& box) : container(box) {}
  operator tValue() {
    tIter it = container.begin();
    tValue num = tValue(0);
    tValue out = *it;
    ++it;

    for(; it!=container.end(); ++it, ++num)
      out += *it;
    return out / num;
  }
  // wrap through function: 
  //template<typename T>
  //typename T::value_type foo(const T& box) {
  //  return wrapped_mean<T>(box);
  //}
};*/

/**
 * The following functions provide basic numerical operations on arbitrary 
 * numbers, i.e., containers of numbers! The realization is very flexible and 
 * adaptable, a passed container 'X' of numbers to be analyzed by the following 
 * functions, just has to fullfil the following requirements:
 * - provide a 'X.begin()' and 'X.end()' method returning a valid 'iterator'
 * - provide a 'X::value_type' member as 'typedef'
 *   (all STL containers do so!)
 * - the containing items of type 'X::value_type' should support basic math 
 *   operations like: /, *, <, >, ++, ==, +=
 * - for 'variance' and 'sigma' additionally a operator <type>(const X& x) is 
 *   required to enable sqrt() call 
 *
 * (See the unit-tests in '../tests/math.cc' for some examples)
 */

template<typename T>
typename T::value_type mean(const T& box) {
    if (box.empty())
      throw EmptyDataContainerError();

    typename T::const_iterator it = box.begin();
    typename T::value_type num = typename T::value_type(1);
    typename T::value_type out = *it;
    ++it;

    for(; it!=box.end(); ++it, ++num)
      out += *it;
    return out / num;
};

template<typename T>
typename T::value_type median(const T& box) {
    if (box.empty())
      throw EmptyDataContainerError();

    T target;
    typename T::const_iterator it = box.begin();
    for(typename T::value_type x : box) {
      target.push_back(*it);
      ++it;
    }
    std::sort(target.begin(), target.end());
    return target.at(target.size() / 2);
};

template<typename T>
typename T::value_type variance(const T& box) {
    if (box.empty())
      throw EmptyDataContainerError();

    typename T::value_type avg = mean(box);
    typename T::const_iterator it = box.begin();
    typename T::value_type x = *it;
    typename T::value_type out = (x - avg) * (x - avg);
    ++it;

    for(; it!=box.end(); ++it) {
      x = *it; 
      out += ( (x - avg) * (x - avg) );
    }
    return out;
};

template<typename T>
typename T::value_type sigma(const T& box) {
    if (box.empty())
      throw EmptyDataContainerError();

    return typename T::value_type(std::sqrt(variance(box)));
};

}}
