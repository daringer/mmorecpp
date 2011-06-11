#ifndef XSTRING_H
#define XSTRING_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>

#include "general.h"
#include "exception.h"

namespace TOOLS {

/**
 * @brief the exception to be thrown if a conversion error occurs
 */
class ConvertValueError : public BaseException {
  public:
    ConvertValueError(const std::string& msg);
};
/**
 * @brief convert any given parameter to a string
 * @param any parameter, which has a string representation
 * @return the converted string
 */
template <class T>
std::string str(const T& val, int precision=0) {
  std::stringstream out;
  if (precision == 0)
    out << val;
  else
    out << std::setprecision(precision) << val;
  return out.str();
}
/**
 * @brief convert any input to an integer, if possible
 * @param any parameter, which has a integer representation
 * @return the converted integer
 */
template <class T>
int integer(const T& val) {
  std::istringstream inp(val);
  int out;
  if(!(inp >> out))
    throw ConvertValueError("Could not convert: '" + str(val) + "' to integer");
  return out;
}

/**
 * @brief convert any input to an double, if possible
 * @param any parameter, which has a double representation
 * @return the converted integer
 */
template <class T>
double real(const T& val) {
  std::istringstream inp(val);
  double out;
  if(!(inp >> out))
    throw ConvertValueError("Could not convert: '" + str(val) + "' to double");
  return out;
}
/**
 * @brief A string class fully compatible with std::string featuring some
 *        often used methods, to bring a little comfort into string handling
 *        Designed to be memory-friendly, thus all operations are done on
 *        the XString itself and simply returning a reference to the prior.
 */
class XString : public std::string {
  public:
    XString();
    /** @brief just a "forwarding-constructor" to maintain string compatibility */
    template<class T>
    XString(const T& val) : std::string(val) {}

    /** @brief just a "forwarding-constructor" to maintain string compatibility */
    template<class T, class S>
    XString(const T& val1, const S& val2) : std::string(val1, val2) {}

    XString& strip();
    XString& strip(const std::string& s);

    Stringlist split();
    Stringlist split(const std::string& s);

    XString& join(const Stringlist& list);

    XString& subs(const std::string& what, const std::string& with, int max_replaces);
    XString& subs(const std::string& what, const std::string& with);
    XString& subs_all(const std::string& what, const std::string& with);

    XString& lower();
    XString& upper();
};
}

#endif
