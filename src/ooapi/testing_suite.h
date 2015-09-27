#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <exception>

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <iomanip>

#include "../core/general.h"
#include "../core/exception.h"
#include "../core/xstring.h"
#include "../core/xtime.h"
#include "../core/xunit.h"

#include "config_manager.h"


namespace MM_NAMESPACE() {
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

#define CHECK(__expr)                                          \
  if (do_checks)                                               \
    add_check(__expr, __LINE__);                               \
  if (!(__expr))                                               \
     add_details(#__expr);                                     \
  if (!(__expr) && return_on_fail)                             \
    return;

#define CHECK_EQ(__val1, __val2)                               \
  if (do_checks)                                               \
    add_check((__val1 == __val2), __LINE__);                   \
  if(__val1 != __val2)                                         \
      add_details(#__val1  " != "  #__val2);                   \
  if ((__val1 != __val2) && return_on_fail)                    \
    return;

#define CHECK_APPROX(__val1, __val2, __eps)                    \
  do {                                                         \
  if (!do_checks)                                              \
    break;                                                     \
  auto __val = __val1 - __val2;                                \
  bool __res = (__val < __eps && __val > -__eps);              \
    add_check(__res, __LINE__);                                \
  if (!__res)                                                  \
    add_details("-eps < " #__val1 "-" #__val2                  \
        " < eps - diff: " + str(__val));                       \
  if (!__res && return_on_fail)                                \
    return;                                                    \
  } while (0)

#define CHECK_EXC(exc, func)                                   \
  do {                                                         \
    if (!do_checks)                                            \
      break;                                                   \
    bool _res = true;                                          \
    try {                                                      \
      func;                                                    \
    } catch (exc& e) {                                         \
    add_exc_check(true, #exc, __LINE__);                       \
      _res = true;                                             \
    } catch (...) {                                            \
    add_exc_check(false, #exc, __LINE__);                      \
      _res = false;                                            \
      if(return_on_fail)                                       \
         return;                                               \
    }                                                          \
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
#define START_SUITE(name)                                     \
  class name : public TestSuite {                             \
   public:                                                    \
    typedef name tTestSuite;                                  \
  name()

// MACRO to define and implement a test
#define MAKE_TEST(name) \
  void test_##name(bool do_checks = true, bool return_on_fail = true)

// MACRO to register a test 
#define REG_TEST(method) add_test(#method, &tTestSuite::test_##method);

// MACRO to end the test-suite
#define END_SUITE() \
  }                 \
  ;

// Call test function without evaluating its CHECKs (not working?)
#define PREPARE_WITH(fnc) test_##fnc(false);

class TestResult {
 public:
  std::string id;
  bool result;
  bool run;
  std::string details;
  XTime timer;
  std::vector<tMicroTime> runtimes;

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
  std::string exc_name;

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

  void execute_tests(const std::string& suite_name, const std::string& only_test,
                     const bool& return_on_fail, const int& repeat_times);

  template<typename T>
  void handle_exception(T* e, Test* thetest, TestResult* res);

 protected:
  Test* active_test;

  void add_details(const std::string& details);
  void add_check(bool expr, int line);
  void add_exc_check(bool res, std::string e_name, int lineno);
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
  int repeat_times;
};
}
}

