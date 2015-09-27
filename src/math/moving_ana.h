#pragma once

#include <unistd.h>

#include "../core/general.h"
#include "../core/exception.h"

#include "basic.h"

#include <vector>
#include <algorithm>
#include <iostream>

namespace MM_NAMESPACE() {
  namespace MATH {

  DEFINE_EXCEPTION(AnalysisWindowIllegalError, TOOLS::BaseException)
  DEFINE_EXCEPTION(NoValueToInterpolateError, TOOLS::BaseException)

  template <typename T, typename C = std::vector<T> >
  class BaseMovingAnalyzer {
   public:
    // typical STL-stuff
    typedef T value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef C cont_type;

   private:
    // this is me
    typedef BaseMovingAnalyzer<T, C> class_type;

   public:
    int max_size;
    value_type next;
    cont_type hist;

    BaseMovingAnalyzer(const int& window_size = 10);

    virtual void set_window_size(const int& window_size);

    virtual const_reference analyze() = 0;

    virtual const_reference get_next(const_reference val);
    virtual const_reference get_next_interpolated() const;
    virtual const_reference get_current() const;
  };

  template <typename T, typename C>
  BaseMovingAnalyzer<T, C>::BaseMovingAnalyzer(const int& window_size) {
    set_window_size(window_size);
  }

  template <typename T, typename C>
  const T& BaseMovingAnalyzer<T, C>::get_current() const {
    return next;
  }

  template <typename T, typename C>
  void BaseMovingAnalyzer<T, C>::set_window_size(const int& window_size) {
    if (window_size < 1)
      throw AnalysisWindowIllegalError(window_size);
    max_size = window_size;
  }
  template <typename T, typename C>
  const T& BaseMovingAnalyzer<T, C>::get_next(const_reference val) {
    hist.push_back(val);
    if (hist.size() > max_size)
      hist.erase(hist.begin());
    return analyze();
  }

  template <typename T, typename C>
  const T& BaseMovingAnalyzer<T, C>::get_next_interpolated() const {
    if (hist.empty())
      throw NoValueToInterpolateError();
    return next;
  }

  template <typename T, typename C = std::vector<T> >
  class MovingAverage : public BaseMovingAnalyzer<T, C> {
   public:
    typedef T value_type;
    typedef const value_type& const_reference;
    typedef BaseMovingAnalyzer<T, C> base_type;

    MovingAverage(const int& window_size) : base_type(window_size) {}
    const_reference analyze() {
      base_type::next = MM_NAMESPACE()::MATH::mean(base_type::hist);
      return base_type::next;
    }
  };

  template <typename T, typename C = std::vector<T> >
  class MovingMedian : public BaseMovingAnalyzer<T, C> {
   public:
    typedef T value_type;
    typedef const value_type& const_reference;
    typedef BaseMovingAnalyzer<T, C> base_type;

    MovingMedian(const int& window_size) : base_type(window_size) {}
    const_reference analyze() {
      base_type::next = MM_NAMESPACE()::MATH::median(base_type::hist);
      return base_type::next;
    }
  };
  }
}
