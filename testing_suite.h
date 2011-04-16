#ifndef TESTING_SUITE_H
#define TESTING_SUITE_H

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <iomanip>

#include "exception.h"

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


class Test {
  public:
    std::string name;
    bool result;
    std::string details;
    tMethod method;
    TestSuite* object;

    Test();
    Test(const std::string& name, bool res, tMethod meth, TestSuite* object);

    TestResult get_result();
};

class TestSuite {
  public:
    tTestMap tests;

    TestSuite();

    virtual void tear_down();
    void after_tear_down();

    virtual void setup();
    void after_setup();

    void execute_tests();

  protected:
    Test* active_test;

    void CHECK(bool expr);
    void CHECK(bool expr, const std::string& desc);

    template<class T>
    void add_test(const std::string& name, void (T::*f)()) {
      tests[name] = Test(name, false, static_cast<tMethod>(f), this);
    }
};

class TestResult {
  public:
    std::string id;
    bool result;
    std::string details;

    TestResult();
    TestResult(std::string& testid, bool res, std::string& details);
};

class TestFramework {
  public:
    TestFramework();
    ~TestFramework();

    template<class T>
    void add_suite(const std::string& desc) {
      test_suites[desc] = new T();
    };

    void run();

    tTestResultMap get_results();
    void show_results();

  private:
    tTestSuiteMap test_suites;
};

}
}

#endif
