#include "testing_suite.h"

#include <cmath>
#include <limits>

using namespace std;
using namespace MM_NAMESPACE();
using namespace MM_NAMESPACE()::UNIT_TEST;

Test::Test() : name(""), method(NULL), object(NULL) {}

Test::Test(const string& name, tMethod meth, TestSuite* obj)
    : name(name), method(meth), object(obj) {}

TestSuite::TestSuite()
    : check_count(0), active_test(NULL), show_good_details(false) {}

TestSuite::~TestSuite() {}

void TestSuite::add_check(bool expr, int line) {
  active_test->res.result &= expr;
  active_test->lineno = line;
  check_count++;
  if (show_good_details || !expr) {
    if (!active_test->res.details.empty())
      active_test->res.details.append("\n[i]    ");
    const string lineinfo = (expr) ? " [good] " : " [bad] ";
    active_test->res.details.append("Line: " + str(line) + lineinfo);
  }
}

void TestSuite::add_details(const std::string& details) {
  active_test->res.details.append(details);
}

void TestSuite::add_exc_check(bool res, string e_name, int line) {
  add_check(res, line);
  if (!res) {
    if (!active_test->res.details.empty())
      active_test->res.details.append("\n[i] ");
    active_test->res.details.append("[exc: " + e_name + "Line: " + str(line) +
                                    "] ");
  }
}

void TestSuite::add_iter_check(bool res, int iters, tIntList errs, int line) {
  add_check(res, line);
  stringstream out;
  for (tIntIter i = errs.begin(); i != errs.end(); ++i) {
    out << *i;
    if (i + 1 != errs.end())
      out << ",";
  }
  if (!res)
    active_test->res.details.append("[eq_iter: iterations: " + str(iters) +
                                    " errors in: " + out.str() + "]");
}

void TestSuite::setup() {}
void TestSuite::tear_down() {}

void TestSuite::after_setup() {
  // active_test->details.append("Setup finished - ");
}

void TestSuite::after_tear_down() {
  // active_test->details.append("TearDown finished!");
}

template <typename T>
void TestSuite::handle_exception(T* e, Test* thetest, TestResult* res) {
  res->details.append(string("[E] test failed - exception caught: ") +
                      e->what() + " - ");
}

void TestSuite::execute_tests(const string& suite_name, const string& only_test,
                              const bool& return_on_fail,
                              const int& repeat_times) {

  // execute tests 'repeat_times' times
  for (int run = repeat_times; run; --run) {
    // for each test registered inside this TestSuite
    for (tTestIter i = tests.begin(); i != tests.end(); ++i) {

      // short-hand ref for current result-data
      TestResult& cur_res = i->second.res;

      // generate full-test-identification
      cur_res.id = suite_name + "::" + i->first;

      // if executing chosen test (set with '-t' cmdline option)
      if (only_test != "" && cur_res.id.find(only_test) == string::npos)
        continue;

      // set test as active and was run flag
      cur_res.run = true;
      active_test = &i->second;

      // fixture setup routines (setup() may be overridden)
      setup();
      after_setup();
      cur_res.timer.start();

      try {
        // actual test-method call/execution
        (i->second.object->*i->second.method)(true, return_on_fail);

        // catch (more-expressive) MM_NAMESPACE() exception
      } catch (const MM_NAMESPACE()::BaseException & e) {
        handle_exception(&e, active_test, &cur_res);

        // catch generic C++ exception
      } catch (const std::exception& e) {
        handle_exception(&e, active_test, &cur_res);

        // last chance catch...
      } catch (...) {
        std::exception e;
        handle_exception(&e, active_test, &cur_res);
      }

      cur_res.timer.stop();
      // tear down test-fixture (tear_down() may be overidden by the user)
      tear_down();
      after_tear_down();

      active_test = NULL;

      // save this specific time taken (instead of the overall duration)
      if (cur_res.runtimes.empty())
        cur_res.runtimes.push_back(cur_res.timer.diff_us());
      else
        cur_res.runtimes.push_back(cur_res.timer.diff_us() -
                                   cur_res.runtimes.back());

      // show results, but only on last 'repeat_times' round
      if (run == 1)
        cur_res.show(false);
    }
  }
}

TestResult::TestResult(const string& testid, bool res, const string& details)
    : id(testid), result(res), details(details) {}

TestResult::TestResult() : id(""), result(true), run(false) {}

void TestResult::show(bool show_details) {
  string icon, rating;
  if (result) {
    rating = "GOOD";
    icon = "+";
  } else {
    rating = "BAD!";
    icon = "-";
  }

  // test description and final rating...
  cout << "[" << icon << "] " << left << setw(45) << id << setw(20) << right
       << rating;

  // calc avg variance, simple squared dist to avg, additionally keep max/min
  double max_val = std::numeric_limits<double>::min();
  double min_val = std::numeric_limits<double>::max();
  double sum_val = 0.0;
  for (const tMicroTime& mt : runtimes) {
    sum_val += mt;
    max_val = std::max(max_val, static_cast<double>(mt));
    min_val = std::min(min_val, static_cast<double>(mt));
  }

  double mean_val = static_cast<double>(static_cast<double>(sum_val) /
                                        static_cast<double>(runtimes.size()));
  mean_val /= 1.e6;

  // show (total) measured time
  cout << " ->" << XUnit(sum_val, "s", 1, false, -6).lpad(12);

  // if repeated multiple times, provide avg-time (and variance) for test
  if (timer.cycles > 1) {
    cout << " [ Ø: " << XUnit(mean_val, "s", 1, false).lpad(10)
         << " | min: " << XUnit(min_val, "s", 1, false, -6).lpad(8)
         << " | max: " << XUnit(max_val, "s", 1, false, -6).lpad(8) << " ]";
  }
  cout << endl;

  // details shown here, if wanted....
  if (details != "" && ((!show_details && !result) || show_details))
    cout << "[i]    " << details << endl;
}

TestFramework::TestFramework(int argc, char* argv[]) {
  ConfigManager conf(argv[0]);

  ConfigGroup& grp = conf.new_group("Main Options");
  grp.new_option<bool>("debug", "Show test details", "d").set_default(false);

  grp.new_option<string>("execute-test", "Execute test(s) by name", "t")
        .set_default("");

  grp.new_option<bool>("stay-on-fail", "Stay in test on fail", "r")
        .set_default(false);

  grp.new_option<int>("repeat-times", "Repeat the tests X times", "x")
        .set_default(1);

  // ConfigManager init finished, start parsing file, then cmdline
  try {
    conf.parse_config_file("noname.conf");
    conf.parse_cmdline(argc, argv);
  } catch (ConfigManagerException& e) {
    e.dump();
    conf.usage(cout);
    exit(1);
  }

  // cmdline parsing done, save configured vars
  show_details = conf.get<bool>("debug");
  execute_test = conf.get<string>("execute-test");
  return_on_fail = !conf.get<bool>("stay-on-fail");
  repeat_times = conf.get<int>("repeat-times");

  cout << endl;
  cout << "[i] START - TestFramework" << endl;
}

TestFramework::~TestFramework() {
  for (tTestSuiteIter i = test_suites.begin(); i != test_suites.end(); ++i)
    delete i->second;
  cout << "[i] END - TestFramework" << endl;
}

void TestFramework::run() {
  for (tTestSuiteIter i = test_suites.begin(); i != test_suites.end(); ++i)
    i->second->execute_tests(i->first, execute_test, return_on_fail,
                             repeat_times);
}

void TestFramework::show_result_overview() {
  int good = 0;
  int bad = 0;
  int all_tests = 0;

  for (tTestSuiteIter i = test_suites.begin(); i != test_suites.end(); ++i) {
    all_tests += i->second->check_count;
    for (tTestIter j = i->second->tests.begin(); j != i->second->tests.end();
         ++j) {
      if (j->second.res.run)
        (j->second.res.result) ? good++ : bad++;
    }
  }
  cout << endl
       << "[i] Finished TestRun (" << all_tests
       << " checks done) - Tests: good: " << good;

  if (bad > 0)
    cout << " and bad: " << bad << endl;
  else
    cout << " and NO bad ones!" << endl;

  if (repeat_times > 1)
    cout << "[i] - repeated " << repeat_times << " times" << endl;
  if (!execute_test.empty())
    cout << "[i] - filtered tests by: '" << execute_test << "'" << endl;
}
