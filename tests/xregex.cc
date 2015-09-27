#include "stdlib.h"

#include "tools.h"

using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;
using namespace std;

START_SUITE(XRegexToolsTestSuite) {
  REG_TEST(compile_pattern)
  REG_TEST(match)
  REG_TEST(search)
  REG_TEST(replace)
}

string haystack, regexp, repl, replaced;

virtual void setup() {
  haystack = "xxxabcXxxxx123abcX321yyyyabcXyyyy";
  regexp = "(a(b)c)(.{2})";
  repl = "---\\2---";
  replaced = "xxx---b---xxx123---b---21yyyy---b---yyy";
}

MAKE_TEST(compile_pattern) {
  Regex pat(regexp);
  CHECK(true);
}

MAKE_TEST(match) {
  Regex pat(regexp);
  CHECK(pat.match(haystack));
}

MAKE_TEST(search) {
  Regex pat(regexp);
  Matchinglist m = pat.search(haystack);
  CHECK(m[0][0] == "abcXx" && m[0][1] == "abc" && m[0][2] == "b" &&
        m[0][3] == "Xx");
  CHECK(m[1][0] == "abcX3" && m[1][1] == "abc" && m[1][2] == "b" &&
        m[1][3] == "X3");
  CHECK(m[2][0] == "abcXy" && m[2][1] == "abc" && m[2][2] == "b" &&
        m[2][3] == "Xy");
  CHECK(m.size() == 3);
}

MAKE_TEST(replace) {
  Regex pat(regexp);
  CHECK(pat.replace(haystack, repl) == replaced);
}
END_SUITE()
