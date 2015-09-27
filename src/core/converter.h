#pragma once

#include <iomanip>

#include "exception.h"
#include "general.h"

namespace MM_NAMESPACE() {

  DEFINE_EXCEPTION(ConvertValueError, BaseException)

  /* convert any given parameter to a string
   * @val any parameter, which has a string representation
   * @return the converted string
   */
  template <class T>
  std::string str(const T& val, int precision = 0) {
    static std::stringstream inp1;
    inp1.str("");
    inp1.clear();
    if (precision == 0)
      inp1 << val;
    else
      inp1 << std::setprecision(precision) << val;
    return inp1.str();
  }

  /* convert any input to an integer, if possible
   * @val any parameter, which has a integer representation
   * @return the converted integer
   */
  template <class T>
  int integer(const T& val) throw(ConvertValueError) {
    static std::stringstream inp2;
    inp2.str("");
    inp2.clear();
    inp2 << val;
    int out;
    bool ret = (!(inp2 >> out));
    if (ret)
      throw ConvertValueError("Could not convert: '" + str(val) +
                              "' to integer");
    return out;
  }

  /* convert any input to an double, if possible
   * @val any parameter, which has a double representation
   * @return the converted integer
   */
  template <class T>
  double real(const T& val) throw(ConvertValueError) {
    static std::stringstream inp3;
    inp3.str("");
    inp3.clear();
    inp3 << val;
    double out;
    bool ret = (!(inp3 >> out));
    if (ret)
      throw ConvertValueError("Could not convert: '" + str(val) + "' to double");
    return out;
  }

  bool is_real(const std::string& s);
}
