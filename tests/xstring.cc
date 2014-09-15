#include "stdlib.h"

#include "xstring.h"

#include "testing_suite.h"

using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;
using namespace std;

START_SUITE(XStringToolsTestSuite) {
  REG_TEST(constructor)
  REG_TEST(subs)
  REG_TEST(split)
  REG_TEST(strip)
  REG_TEST(real)
  REG_TEST(real_error)
  REG_TEST(integer)
  REG_TEST(integer_error)
  REG_TEST(lower_case)
  REG_TEST(upper_case)
  REG_TEST(empty_string_split)
  REG_TEST(string_no_delim)
  REG_TEST(is_real)
  REG_TEST(is_no_real)
}

XString s1, s2, s3, foo, realstr, intstr, s3_big, s3_small;
double decimal;
int number;

virtual void setup() {
  s1 = "teststring";
  s2 = "longer teststring";
  s3 = "aNd aNoTHeR";
  s3_big = "AND ANOTHER";
  s3_small = "and another";
  foo = "non-integer or real value";
  realstr = "12.34";
  decimal = 12.34;
  intstr = "123";
  number = 123;
}

virtual void tear_down() {}

MAKE_TEST(constructor) {
  XString xs1("abc");
  CHECK(xs1 == "abc");
  XString xs2(s1);
  CHECK(xs2 == s1);
}

MAKE_TEST(subs) {
  XString src(
      "abc--[placeholder]---[two]--[two]-[two]---[two][two][two][one][one]["
      "one]");
  XString step("abc--yyy---xxx--xxx-[two]---[two][two][two][one][one][one]");
  XString final("abc--yyy---xxx--xxx-xxx---xxxxxxxxxzzzzzzzzz");

  XString xs1 = src.subs("[placeholder]", "yyy").subs("[two]", "xxx", 2);
  CHECK(xs1 == step);

  XString xs2 = xs1.subs_all("[two]", "xxx").subs_all("[one]", "zzz");
  CHECK(xs2 == final);
}
MAKE_TEST(split) {
  tStringList l = s2.split();
  CHECK(l.size() == 2 && l[0].length() == 6 && l[1].length() == 10);

  tStringList m = XString("ab|cd|foo||a|xxxx").split("|");
  CHECK(m.size() == 6 && m[0].length() == 2 && m[1].length() == 2 &&
        m[2].length() == 3 && m[3].length() == 0 && m[4].length() == 1 &&
        m[5].length() == 4);
}

MAKE_TEST(strip) {
  XString xs1("  abc   ");
  CHECK(xs1.strip() == "abc");
  XString xs2("--//--xxx-/-");
  CHECK(xs2.strip("-").strip("/").strip("-") == "xxx");
}

MAKE_TEST(lower_case) { CHECK(s3.lower() == s3_small); }

MAKE_TEST(upper_case) { CHECK(s3.upper() == s3_big); }

MAKE_TEST(real) { CHECK(real(realstr) == decimal); }

MAKE_TEST(real_error) { CHECK_EXC(ConvertValueError, real(foo)); }

MAKE_TEST(integer) { CHECK(integer(intstr) == number); }

MAKE_TEST(integer_error) { CHECK_EXC(ConvertValueError, integer(foo)); }

MAKE_TEST(empty_string_split) {
  XString xs("");
  tStringList sl = xs.split(" ");
  CHECK(sl.size() == 1 && sl[0] == "");
}

MAKE_TEST(string_no_delim) {
  tStringList sl = s1.split(" ");
  CHECK(sl.size() == 1 && sl[0] == s1);
}

MAKE_TEST(is_real) {
  CHECK(is_real("1"));
  CHECK(is_real("-1"));
  CHECK(is_real("0"));
  CHECK(is_real("1.1"));
  CHECK(is_real("-5.15"));
  CHECK(is_real("1.4325123"));
  CHECK(is_real("-94023.23442"));
  CHECK(is_real("23412"));
  CHECK(is_real("1.23e-45"));
  CHECK(is_real("10e3"));
  CHECK(is_real("-432e-123"));
  CHECK(is_real("-999E999"));
  CHECK(is_real("1.145e-05"));
  CHECK(is_real("1.5E-05"));
  CHECK(is_real("1e+07"));
  CHECK(is_real("1231.5E+005"));
  CHECK(is_real("1.5092383201382102"));

}

MAKE_TEST(is_no_real) {
  CHECK(!is_real("1.231.231"));
  CHECK(!is_real(""));
  CHECK(!is_real("oidsjf3242"));
  CHECK(!is_real("123.123e12331e"));
  CHECK(!is_real("a"));
  CHECK(!is_real("123.312w44"));
  CHECK(!is_real("-"));
}

END_SUITE()
