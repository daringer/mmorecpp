#ifndef TESTING_SUITE_H
#define TESTING_SUITE_H

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <iomanip>

#include "general.h"
#include "exception.h"
#include "xstring.h"

namespace TOOLS {
namespace UNIT_TEST {

class TestSuite;
class TestResult;
class Test;

typedef void (TestSuite::*tMethod)();

typedef std::map<std::string, TestResult> tTestResultMap;
typedef std::map<std::string, Test> tTestMap;
typedef std::map<std::string, TestSuite*> tTestSuiteMap;

typedef tTestMap::iterator tTestIter;
typedef tTestResultMap::iterator tTestResultIter;
typedef tTestSuiteMap::iterator tTestSuiteIter;


#define CHECK(expr) \
  add_check(expr, __LINE__);

#define EXC_CHECK(exc, func) do { \
    bool _res = false; \
    try { \
      func; \
    } catch(exc e) { \
      _res = true; \
    } \
    add_exc_check(_res, #exc, __LINE__); \
  } while(0)

#define CHECK_DUAL_ITER(iter, lbox, rbox, expr) do { \
  int _count = 0; \
  tIntList errs; \
  for(iter i=lbox.begin(), j=rbox.begin(); \
      i!=lbox.end(), j!=rbox.end(); ++i, ++j, ++_count) \
      if(!(expr)) \
        errs.push_back(_count); \
  add_eq_iter_check(errs.empty(), _count, errs, __LINE__); \
} while(0)

#define CHECK_ITER(iter, box, expr) do { \
  int _count = 0; \
  tIntList errs; \
  for(iter i=box.begin(); i!=box.end(); ++i, ++_count) \
      if(!(expr)) \
        errs.push_back(_count); \
  add_eq_iter_check(errs.empty(), _count, errs, __LINE__); \
} while(0)

#define REG_TEST(method) \
  add_test(#method, method);

class TestResult {
  public:
    std::string id;
    bool result;
    std::string details;

    TestResult();
    TestResult(const std::string& testid, bool res, const std::string& details);

    void show(bool show_details);
};

class Test {
  public:
    std::string name;
    int lineno;
    tMethod method;
    TestSuite* object;
    TestResult res;

    Test();
    Test(const std::string& name, tMethod meth, TestSuite* object);
};

class TestSuite {
  public:
    tTestMap tests;
    int check_count;

    TestSuite();

    virtual void tear_down();
    void after_tear_down();

    virtual void setup();
    void after_setup();

    void execute_tests();

  protected:
    Test* active_test;

    void add_check(bool expr, int lineno);
    void add_exc_check(bool res, const std::string& excname, int lineno);
    void add_iter_check(bool res, int iters, tIntList errs, int lineno);


    template<class T>
    void add_test(XString name, void (T::*f)()) {
      XString desc = XString(name).subs("test_", "").subs("&", "").subs("TestSuite", "");
      tests[desc] = Test(desc, static_cast<tMethod>(f), this);
    }
  private:
    bool show_good_details;
};


class TestFramework {
  public:
    TestFramework(int argc=0, char* argv[]=NULL);
    ~TestFramework();

    template<class T>
    void add_suite(const std::string& desc) {
      test_suites[desc] = new T();
    }

    void run();

    void show_result_overview();

  private:
    tTestSuiteMap test_suites;
    bool show_details;
};

}
}

#endif
