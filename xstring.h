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

DEFINE_EXCEPTION(ConvertValueError, BaseException)

/* convert any given parameter to a string
 * @val any parameter, which has a string representation
 * @return the converted string
 */
template <class T>
std::string str(const T& val, int precision=0) {
  std::stringstream out;
  if(precision == 0)
    out << val;
  else
    out << std::setprecision(precision) << val;
  return out.str();
}

/* convert any input to an integer, if possible
 * @val any parameter, which has a integer representation
 * @return the converted integer
 */
template <class T>
int integer(const T& val) throw(ConvertValueError) {
  std::istringstream inp(val);
  int out;
  if(!(inp >> out))
    throw ConvertValueError("Could not convert: '" + str(val) + "' to integer");
  return out;
}

/* convert any input to an double, if possible
 * @val any parameter, which has a double representation
 * @return the converted integer
 */
template <class T>
double real(const T& val) throw(ConvertValueError) {
  std::istringstream inp(val);
  double out;
  if(!(inp >> out))
    throw ConvertValueError("Could not convert: '" + str(val) + "' to double");
  return out;
}

/* A string class fully compatible with std::string.
 * Featuring some often used methods, to bring a little comfort into string handling
 * designed to be memory-friendly, thus all operations are done on
 * the <XString> itself and simply returning a reference to the prior.
 */
class XString : public std::string {
  public:
    XString();
    // just a "forwarding-constructor" to maintain string compatibility
    virtual ~XString();
    /** @brief just a "forwarding-constructor" to maintain string compatibility */
    template<class T>
    XString(const T& val) : std::string(val) {}

    /* just a "forwarding-constructor" to maintain string compatibility 
     * @val1 forwarded to std::string
     * @val2 forwarded to std::string
     */
    template<class T, class S>
    XString(const T& val1, const S& val2) : std::string(val1, val2) {}

    XString& strip(const std::string& s=" ");

    tStringList split(const std::string& s=" ");

    XString& join(const tStringList& list);

    XString& subs(const std::string& what, const std::string& with, int max_replaces=1);
    XString& subs_all(const std::string& what, const std::string& with);

    XString& lower();
    XString& upper();

    bool startswith(const std::string& what);
    bool endswith(const std::string& what);
};
}

#endif
