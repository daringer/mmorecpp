#include "stdlib.h"

#include "xstring.h"

#include "testing_suite.h"


using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;
using namespace std;

class XStringToolsTestSuite : public TestSuite {
  public:
    XStringToolsTestSuite() {
      REG_TEST(&XStringToolsTestSuite::test_constructor);
      REG_TEST(&XStringToolsTestSuite::test_subs);
      REG_TEST(&XStringToolsTestSuite::test_split);
      REG_TEST(&XStringToolsTestSuite::test_strip);
      REG_TEST(&XStringToolsTestSuite::test_real);
      REG_TEST(&XStringToolsTestSuite::test_real_error);
      REG_TEST(&XStringToolsTestSuite::test_integer);
      REG_TEST(&XStringToolsTestSuite::test_integer_error);
      REG_TEST(&XStringToolsTestSuite::test_lower_case);
      REG_TEST(&XStringToolsTestSuite::test_upper_case);
    };

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
    };

    virtual void tear_down() { };

    void test_constructor() {
      XString xs1("abc");
      CHECK(xs1 == "abc");
      XString xs2(s1);
      CHECK(xs2 == s1);
    };

    void test_subs() {
      XString src("abc--[placeholder]---[two]--[two]-[two]---[two][two][two][one][one][one]");
      XString step("abc--yyy---xxx--xxx-[two]---[two][two][two][one][one][one]");
      XString final("abc--yyy---xxx--xxx-xxx---xxxxxxxxxzzzzzzzzz");

      XString xs1 = src.subs("[placeholder]", "yyy").subs("[two]", "xxx", 2);
      CHECK(xs1 == step);

      XString xs2 = xs1.subs_all("[two]","xxx").subs_all("[one]", "zzz");
      CHECK(xs2 == final);
    };

    void test_split() {
      tStringList l = s2.split();
      CHECK(l.size() == 2 && l[0].length() == 6 && l[1].length() == 10);


      tStringList m = XString("ab|cd|foo||a|xxxx").split("|");
      CHECK(m.size() == 6 && m[0].length() == 2 && m[1].length() == 2 &&
            m[2].length() == 3 && m[3].length() == 0 && m[4].length() == 1 &&
            m[5].length() == 4);
    };

    void test_strip() {
      XString xs1("  abc   ");
      CHECK(xs1.strip() == "abc");
      XString xs2("--//--xxx-/-");
      CHECK(xs2.strip("-").strip("/").strip("-") == "xxx");
    };

    void test_lower_case() {
      CHECK(s3.lower() == s3_small);
    };

    void test_upper_case() {
      CHECK(s3.upper() == s3_big);
    };

    void test_real() {
      CHECK(real(realstr) == decimal);
    };

    void test_real_error() {
      EXC_CHECK(ConvertValueError, real(foo));
    };

    void test_integer() {
      CHECK(integer(intstr) == number);
    };

    void test_integer_error() {
      EXC_CHECK(ConvertValueError, integer(foo));
    };


};
