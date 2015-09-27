#pragma once

#include "../core/general.h"

#include "basic.h"
#include "../core/exception.h"

#include <vector>
#include <algorithm>
#include <iostream>

namespace MM_NAMESPACE() {
  namespace MATH {

DEFINE_EXCEPTION(AnalysisWindowIllegalError, BaseException)
DEFINE_EXCEPTION(NoValueToInterpolateError, BaseException)

// TODO, FIXME: WHY WHY WHY IS THIS NOT POSSIBLE IN A BETTER WAY (without c++11)
#define GET_TYPE_ANALYZER_HISTORY(item_type) std::vector<item_type>

template<typename T>
class BaseMovingAnalyzer {
  protected:
  public:
    int max_size;
    T next;
    GET_TYPE_ANALYZER_HISTORY(T) hist;

    BaseMovingAnalyzer(const int& window_size=10) ;

    virtual void set_window_size(const int& window_size) ;
    
    virtual const T& analyze() = 0;

    virtual const T& get_next(const T& val);
    virtual const T& get_next_interpolated();
    virtual const T& get_current();
};

template<typename T>
BaseMovingAnalyzer<T>::BaseMovingAnalyzer(const int& window_size)  { 
  set_window_size(window_size); 
}

template<typename T>
const T& BaseMovingAnalyzer<T>::get_current() {
  return next;
}

template<typename T>
void BaseMovingAnalyzer<T>::set_window_size(const int& window_size)  {
    if (window_size < 1)
      throw AnalysisWindowIllegalError(window_size);
    max_size = window_size;
}
template<typename T>
const T& BaseMovingAnalyzer<T>::get_next(const T& val) {
  hist.push_back(val);
  if (hist.size() > max_size)
    hist.erase(hist.begin());
  return analyze();
}

template<typename T>
const T& BaseMovingAnalyzer<T>::get_next_interpolated()  {
  if (hist.empty())
    throw NoValueToInterpolateError();
  next = hist.back();
  return next;
}

template<typename T>
class MovingAverage : public BaseMovingAnalyzer<T> {
  public:
    MovingAverage(const int& window_size) : BaseMovingAnalyzer<T>(window_size) {}
    const T& analyze() {
      BaseMovingAnalyzer<T>::next = MM_NAMESPACE()::MATH::mean(BaseMovingAnalyzer<T>::hist);
      return BaseMovingAnalyzer<T>::next;
    }
};

template<typename T>
class MovingMedian : public BaseMovingAnalyzer<T> {
  public:
    MovingMedian(const int& window_size) : BaseMovingAnalyzer<T>(window_size) {}
    const T& analyze() {
      BaseMovingAnalyzer<T>::next = MM_NAMESPACE()::MATH::median(BaseMovingAnalyzer<T>::hist);
      return BaseMovingAnalyzer<T>::next;
    }
};

}}
