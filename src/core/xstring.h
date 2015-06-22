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

/* A string class fully compatible with std::string.
 * Featuring some often used methods, to bring a little comfort into string
 * handling
 * designed to be memory-friendly, thus all operations are done on
 * the <XString> itself and simply returning a reference to the prior.
 */
class XString : public std::string {
 public:
  XString();
  // just a "forwarding-constructor" to maintain string compatibility
  // TODO USE C++11 here and do a using foo::foo();
  virtual ~XString();
  /** @brief just a "forwarding-constructor" to maintain string compatibility */

  //using std::string::string;

  template <class T>
  XString(const T& val)
      : std::string(val) {}

  /* just a "forwarding-constructor" to maintain string compatibility
   * @val1 forwarded to std::string
   * @val2 forwarded to std::string
   */
  template <class T, class S>
  XString(const T& val1, const S& val2)
      : std::string(val1, val2) {}

  XString& strip(const std::string& s = " ");

  tStringList split(const std::string& s = " ");

  XString& join(const tStringList& list);
  XString& join(const tStringSet& set);

  XString& subs(const std::string& what, const std::string& with,
                int max_replaces = 1);
  XString& subs_all(const std::string& what, const std::string& with);

  XString& lower();
  XString& upper();

  bool startswith(const std::string& what);
  bool endswith(const std::string& what);
};
}

#endif
