#pragma once

#warning deprecated

#include <string>
#include <vector>

#include <regex.h>
#include <strings.h>

#include "../core/general.h"
#include "../core/exception.h"
#include "../core/converter.h"
#include "../core/xstring.h"

namespace MM_NAMESPACE() {

  typedef tStringList Grouplist;
  typedef std::vector<tStringList> Matchinglist;
  typedef Matchinglist::iterator MatchingIter;
  typedef Grouplist::iterator GroupIter;

  /**
   * @brief RegexException to be thrown on any regular expression error
   */
  class RegexException : public BaseException {
   public:
    RegexException(const std::string& msg);
    RegexException(const int errcode, const regex_t* pattern);
  };
  /**
   * @brief a (posix)wrapped regular expression class with easy usage:
   *        Regex pat("123(.+)");
   *        pat.search("my fancy 123 string ");
   *
   *        Supports regex replace!
   */
  class Regex {
   public:
    Matchinglist results;
    std::string raw_pattern;

    Regex(const std::string& pattern);
    virtual ~Regex();

    bool match(const std::string& s);
    Matchinglist& search(const std::string& s);
    std::string replace(const std::string& what, const std::string& with);

   protected:
    regex_t* pattern;

    void apply_pattern(const std::string& s, int max_results);
  };
}
