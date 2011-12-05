#include "stdlib.h"

#include "xregex.h"

#include "testing_suite.h"

using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;
using namespace std;

class XRegexToolsTestSuite : public TestSuite {
  public:
    string haystack, regexp, repl, replaced;

     XRegexToolsTestSuite() {
      REG_TEST(&XRegexToolsTestSuite::test_compile_pattern)
      REG_TEST(&XRegexToolsTestSuite::test_match)
      REG_TEST(&XRegexToolsTestSuite::test_search)
      REG_TEST(&XRegexToolsTestSuite::test_replace)
    }

    virtual void setup() {
      haystack = "xxxabcXxxxx123abcX321yyyyabcXyyyy";
      regexp = "(a(b)c)(.{2})";
      repl = "---\\2---";
      replaced = "xxx---b---xxx123---b---21yyyy---b---yyy";
    }

    void test_compile_pattern() {
      Regex pat(regexp);
      CHECK(true);
    }

    void test_match() {
      Regex pat(regexp);
      CHECK(pat.match(haystack));
    }

    void test_search() {
      Regex pat(regexp);
      Matchinglist m = pat.search(haystack);
      CHECK(m[0][0] == "abcXx" && m[0][1] == "abc" && \
            m[0][2] == "b" && m[0][3] == "Xx");
      CHECK(m[1][0] == "abcX3" && m[1][1] == "abc" && \
            m[1][2] == "b" && m[1][3] == "X3");
      CHECK(m[2][0] == "abcXy" && m[2][1] == "abc" && \
            m[2][2] == "b" && m[2][3] == "Xy");
      CHECK(m.size() == 3);
    }

    void test_replace() {
      Regex pat(regexp);
      CHECK(pat.replace(haystack, repl) == replaced);
    }
};
