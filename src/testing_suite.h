#ifndef TESTING_SUITE_H
#define TESTING_SUITE_H

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <iomanip>

#include "general.h"
#include "exception.h"
#include "xstring.h"
#include "config_manager.h"
#include "xtime.h"

namespace TOOLS {
namespace UNIT_TEST {

class TestSuite;
class TestResult;
class Test;

typedef void (TestSuite::*tMethod)(bool, bool);

typedef std::map<std::string, TestResult> tTestResultMap;
typedef std::map<std::string, Test> tTestMap;
typedef std::map<std::string, TestSuite*> tTestSuiteMap;

typedef tTestMap::iterator tTestIter;
typedef tTestResultMap::iterator tTestResultIter;
typedef tTestSuiteMap::iterator tTestSuiteIter;

// selects either .h file generation or .cc
//#define GENERATE_HEADER

#define CHECK(expr)              \
  if (do_checks)                 \
    add_check(expr, __LINE__);   \
  if (!(expr) && return_on_fail) \
    return;

#define CHECK_EXC(exc, func)             \
  do {                                   \
    if (!do_checks)                      \
      break;                             \
    bool _res = false;                   \
    try {                                \
      func;                              \
    }                                    \
    catch (exc e) {                      \
      _res = true;                       \
    }                                    \
    add_exc_check(_res, #exc, __LINE__); \
  } while (0)

#define CHECK_DUAL_ITER(iter, lbox, rbox, expr)                \
  do {                                                         \
    if (!do_checks)                                            \
      break;                                                   \
    int _count = 0;                                            \
    tIntList errs;                                             \
    for (iter i = lbox.begin(), j = rbox.begin();              \
         i != lbox.end(), j != rbox.end(); ++i, ++j, ++_count) \
      if (!(expr))                                             \
        errs.push_back(_count);                                \
    add_iter_check(errs.empty(), _count, errs, __LINE__);      \
  } while (0)

#define CHECK_ITER(iter, box, expr)                           \
  do {                                                        \
    if (!do_checks)                                           \
      break;                                                  \
    int _count = 0;                                           \
    tIntList errs;                                            \
    for (iter i = box.begin(); i != box.end(); ++i, ++_count) \
      if (!(expr))                                            \
        errs.push_back(_count);                               \
    add_iter_check(errs.empty(), _count, errs, __LINE__);     \
  } while (0)

// MACRO to start a test-suite
#define START_SUITE(name)         \
  class name : public TestSuite { \
   public:                        \
    typedef name tTestSuite;      \
  name()

// MACRO to define and implement a test
#define MAKE_TEST(name) \
  void test_##name(bool do_checks = true, bool return_on_fail = true)

// MACRO to register a test (obsolete?)
#define REG_TEST(method) add_test(#method, &tTestSuite::test_##method);

// MACRO to end the test-suite
#define END_SUITE() \
  }                 \
  ;

// MACRO to call other test function without evaluating its CHECKs
#define PREPARE_WITH(fnc) test_##fnc(false);

class TestResult {
 public:
  std::string id;
  bool result;
  bool run;
  std::string details;
  XTime timer;

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
  virtual ~TestSuite();

  virtual void tear_down();
  void after_tear_down();

  virtual void setup();
  void after_setup();

  void execute_tests(const std::string& suite_name,
                     const std::string& only_test, const bool& return_on_fail);

 protected:
  Test* active_test;

  void add_check(bool expr, int lineno);
  void add_exc_check(bool res, const std::string& excname, int lineno);
  void add_iter_check(bool res, int iters, tIntList errs, int lineno);

  template <class T>
  void add_test(XString name, void (T::*f)(bool, bool)) {
    XString desc = name;  // XString(name).subs("test_", "").split("::")[1];
    tests[desc] = Test(desc, static_cast<tMethod>(f), this);
  }

 private:
  bool show_good_details;
};

class TestFramework {
 public:
  TestFramework(int argc = 0, char* argv[] = NULL);
  virtual ~TestFramework();

  template <class T>
  void add_suite(const std::string& desc) {
    test_suites[desc] = new T();
  }

  void run();

  void show_result_overview();

 private:
  tTestSuiteMap test_suites;
  bool show_details;
  std::string execute_test;
  bool return_on_fail;
};
}
}

#endif
