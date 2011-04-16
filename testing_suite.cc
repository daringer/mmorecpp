#include "testing_suite.h"

using namespace std;
using namespace TOOLS::UNIT_TEST;

Test::Test() : name(""), result(false), details(""), method(NULL), object(NULL) {};

Test::Test(const string& name, bool res, tMethod meth, TestSuite* obj)
  : name(name), result(res), details(""), method(meth), object(obj) {}

TestSuite::TestSuite() : active_test(NULL) { }

void TestSuite::CHECK(bool expr) {
  active_test->result = expr;
}

void TestSuite::CHECK(bool expr, const string& desc) {
  active_test->result = expr;
  active_test->details.append("CHECK-debug [" + desc + "] - ");
}

void TestSuite::setup() {}
void TestSuite::tear_down() {}

void TestSuite::after_setup() {
  //active_test->details.append("Setup finished - ");
}

void TestSuite::after_tear_down() {
  //active_test->details.append("TearDown finished!");
}

void TestSuite::execute_tests() {
  for(tTestIter i=tests.begin(); i!=tests.end(); ++i) {
    active_test = &i->second;
    setup();
    after_setup();
    try {
      (i->second.object->*i->second.method)();
    } catch(TOOLS::BaseException e) {
      i->second.details.append("[E] run failed - exception caught: " + e.output + " - ");
      i->second.result = false;
    }
    tear_down();
    after_tear_down();
    active_test = NULL;
  }
}

TestResult::TestResult(string& testid, bool res, string& details)
  : id(testid), result(res), details(details) {}

TestResult::TestResult() : id(""), result(false) {}

TestResult Test::get_result() {
  return TestResult(name, result, details);
}

TestFramework::TestFramework() {}

TestFramework::~TestFramework() {
  for(tTestSuiteIter i=test_suites.begin(); i!=test_suites.end(); ++i)
    delete i->second;
}

void TestFramework::run() {
  for(tTestSuiteIter i=test_suites.begin(); i!=test_suites.end(); ++i)
    i->second->execute_tests();
}

tTestResultMap TestFramework::get_results() {
  tTestResultMap out;
  for(tTestSuiteIter i=test_suites.begin(); i!=test_suites.end(); ++i)
    for(tTestIter j=i->second->tests.begin(); j!=i->second->tests.end(); ++j)
      out[i->first + " | " + j->first] = j->second.get_result();
  return out;
}

void TestFramework::show_results() {
  tTestResultMap res = get_results();
  int good = 0, bad = 0;
  string rating, icon;

  cout << endl << "[i] Collecting results..." << endl;
  for(tTestResultIter i=res.begin(); i!=res.end(); ++i) {
    if(i->second.result) {
      rating = "GOOD";
      good++;
      icon = "+";
    } else {
      rating = "BAD!";
      bad++;
      icon = "-";
    }

    cout << "[" << icon << "] " << left << setw(45) << i->first << \
         setw(20) << right << rating << endl;
    if(i->second.details != "")
      cout << "[i]    Details: " << i->second.details << endl;
  }
  cout << endl << "[i] Finished TestRun - good: " << good;
  if(bad > 0)
    cout << " and bad: " << bad << endl;
  else
    cout << " and NO bad tests!" << endl;
}


