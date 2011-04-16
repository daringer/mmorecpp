#ifndef REGEX_H
#define REGEX_H

#include <string>
#include <vector>

#include <regex.h>
#include <strings.h>

#include "general.h"
#include "exception.h"
#include "xstring.h"


namespace TOOLS {

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
    typedef std::vector<std::string> Grouplist;
    typedef std::vector<Grouplist> Matchinglist;
    typedef Matchinglist::iterator iterator;
    typedef Grouplist::iterator group_iterator;

    Matchinglist results;
    std::string raw_pattern;

    Regex(const std::string& pattern);
    ~Regex();

    iterator begin();
    iterator end();

    bool match(const std::string& s);
    Matchinglist& search(const std::string& s);
    std::string replace(const std::string& what, const std::string& with);

  protected:
    regex_t pattern;

    void apply_pattern(const std::string& s, int max_results);
};

}
#endif
