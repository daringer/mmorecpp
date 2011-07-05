#include "stdlib.h"

#include "xstring.h"

#include "testing_suite.h"


using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;
using namespace std;

class XStringToolsTestSuite : public TestSuite {
  public:
    XStringToolsTestSuite() {
      add_test("Constructor", &XStringToolsTestSuite::test_constructor);
      add_test("String substitution", &XStringToolsTestSuite::test_subs);
      add_test("String split", &XStringToolsTestSuite::test_split);
      add_test("String strip", &XStringToolsTestSuite::test_strip);
    };

    XString s1, s2, s3;

    virtual void setup() { 
      s1 = "teststring";
      s2 = "longer teststring";
      s3 = "and another";
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
};
