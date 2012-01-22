#include "testing_suite.h"

using namespace std;
using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;

Test::Test() : name(""), result(true), details(""), method(NULL), object(NULL) {}

Test::Test(const string& name, tMethod meth, TestSuite* obj)
  : name(name), result(true), details(""), method(meth), object(obj) {}

TestSuite::TestSuite() : active_test(NULL), show_good_details(false) { }

void TestSuite::add_check(bool expr, int line) {
  active_test->result &= expr;
  active_test->lineno = line;

  if(show_good_details || !expr) {
    const string lineinfo = (expr) ? " [good]" : " [bad]";
    if(XString(active_test->details).startswith("Line(s):"))
      active_test->details.append(", " + str(line) + lineinfo);
    else
      active_test->details.append("Line(s): " + str(line) + lineinfo);
  }
}

void TestSuite::add_exc_check(bool res, const std::string& excname, int line) {
  add_check(res, line);
  if(!res)
    active_test->details.append("[exc: " + excname + "] ");
}

void TestSuite::add_eq_iter_check(bool res, int iters, tIntList errs, int line) {
  add_check(res, line);
  stringstream out;
  for(tIntIter i=errs.begin(); i!=errs.end(); ++i) {
    out << *i;
    if(i+1 != errs.end())
      out << ",";
  }
  if(!res)
    active_test->details.append("[eq_iter: iterations: " + 
        str(iters) + " errors in: " + out.str() + "]");
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
    } catch(TOOLS::BaseException& e) {
      i->second.details.append("[E] test failed - exception caught: " + e.output + " - ");
      i->second.result = false;
    } catch(exception& e) {
      i->second.details.append("[E] test failed - std::exception caught: " + str(e.what()) + " - ");
      i->second.result = false;
    }
    tear_down();
    after_tear_down();
    active_test = NULL;
  }
}

TestResult::TestResult(const string& testid, bool res, const string& details)
  : id(testid), result(res), details(details) {}

TestResult::TestResult() : id(""), result(false) {}

TestResult Test::get_result() {
  return TestResult(name, result, details);
}

TestFramework::TestFramework(int argc, char* argv[]) {
  show_details = (argc == 2 && str(argv[1]) == "-d") ? true : false;
}

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

    if(i->second.details != "" && ((!show_details && !i->second.result) || show_details))
          cout << "[i]    " << i->second.details << endl;
  }
  cout << endl << "[i] Finished TestRun - good: " << good;
  if(bad > 0)
    cout << " and bad: " << bad << endl;
  else
    cout << " and NO bad tests!" << endl;
}


