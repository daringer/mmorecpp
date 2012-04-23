#include "testing_suite.h"

using namespace std;
using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;

/// @todo PREPARE_WITH DOES NOT WORK!

Test::Test() : name(""), method(NULL), object(NULL) {}

Test::Test(const string& name, tMethod meth, TestSuite* obj)
  : name(name), method(meth), object(obj) {}

TestSuite::TestSuite() : check_count(0), active_test(NULL), show_good_details(false) { }

void TestSuite::add_check(bool expr, int line) {
  active_test->res.result &= expr;
  active_test->lineno = line;
  check_count++;

  if(show_good_details || !expr) {
    const string lineinfo = (expr) ? " [good]" : " [bad]";
    if(XString(active_test->res.details).startswith("Line(s):"))
      active_test->res.details.append(", " + str(line) + lineinfo);
    else
      active_test->res.details.append("Line(s): " + str(line) + lineinfo);
  }
}

void TestSuite::add_exc_check(bool res, const std::string& excname, int line) {
  add_check(res, line);
  if(!res)
    active_test->res.details.append("[exc: " + excname + " line: " + str(line) + "] ");
}

void TestSuite::add_iter_check(bool res, int iters, tIntList errs, int line) {
  add_check(res, line);
  stringstream out;
  for(tIntIter i=errs.begin(); i!=errs.end(); ++i) {
    out << *i;
    if(i+1 != errs.end())
      out << ",";
  }
  if(!res)
    active_test->res.details.append("[eq_iter: iterations: " +
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

void TestSuite::execute_tests(const string& suite_name) {
  for(tTestIter i=tests.begin(); i!=tests.end(); ++i) {
    i->second.res.id = suite_name + "::" + i->first;
    active_test = &i->second;
    setup();
    after_setup();
    try {
      (i->second.object->*i->second.method)(true);
    } catch(TOOLS::BaseException& e) {
      i->second.res.details.append(
          "[E] test failed - exception caught: " + e.output + " - ");
      i->second.res.result = false;
    } catch(exception& e) {
      i->second.res.details.append(
          "[E] test failed - std::exception caught: " + str(e.what()) + " - ");
      i->second.res.result = false;
    }
    tear_down();
    after_tear_down();
    active_test = NULL;
    i->second.res.show(false);
  }
}

TestResult::TestResult(const string& testid, bool res, const string& details)
  : id(testid), result(res), details(details) {}

TestResult::TestResult() : id(""), result(true) {}

void TestResult::show(bool show_details) {
  string icon, rating;
  if(result) {
    rating = "GOOD";
    icon = "+";
  } else {
    rating = "BAD!";
    icon = "-";
  }

  cout << "[" << icon << "] " << left << setw(45) << id << \
    setw(20) << right << rating << endl;

  if(details != "" && ((!show_details && !result) || show_details))
    cout << "[i]    " << details << endl;

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
    i->second->execute_tests(i->first);
}

void TestFramework::show_result_overview() {
  int good = 0;
  int bad = 0;
  int all_tests = 0;

  for(tTestSuiteIter i=test_suites.begin(); i!=test_suites.end(); ++i) {
    all_tests += i->second->check_count;
    for(tTestIter j=i->second->tests.begin(); j!=i->second->tests.end(); ++j) {
      (j->second.res.result) ? good++ : bad++;
    }
  }
  cout << endl << "[i] Finished TestRun (" <<
    all_tests << " checks done) - Tests: good: " << good;
  if(bad > 0)
    cout << " and bad: " << bad << endl;
  else
    cout << " and NO bad ones!" << endl;
}

void TestFramework::print_stacktrace(uint max_frames) {
    cerr << "[BT] ";

    void* addrlist[max_frames+1];
    int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

    if(addrlen == 0) {
        cerr << "[E] backtrace() returned 0 - Error!" << endl;
        return;
    }
    // resolve addresses to -> "filename(function+address)"
    // symlist must be free()-ed !
    char** symlist = backtrace_symbols(addrlist, addrlen);

    size_t funcnamesize = 256;
    char* funcname = (char*)malloc(funcnamesize);

    // demangle all functionnames
    for (int i=1; i<addrlen; ++i) {
        char* begin_name = 0;
        char* begin_offset = 0;
        char* end_offset = 0;

        // find parentheses and +address offset surrounding the mangled name:
        // ./module(function+0x15c) [0x8048a6d]
        for (char *p = symlist[i]; *p; ++p) {
            if (*p == '(')
                begin_name = p;
            else if (*p == '+')
                begin_offset = p;
            else if (*p == ')' && begin_offset) {
                end_offset = p;
                break;
            }
        }

        if (begin_name && begin_offset && end_offset
            && begin_name < begin_offset) {
            *begin_name++ = '\0';
            *begin_offset++ = '\0';
            *end_offset = '\0';

            // mangled name is now in [begin_name, begin_offset) and caller
            // offset in [begin_offset, end_offset). now apply
            // __cxa_demangle():

            int status;
            char* ret = abi::__cxa_demangle(begin_name,
                                            funcname, &funcnamesize, &status);
            if (status == 0) {
                funcname = ret; // use possibly realloc()-ed string
                cerr << "[BT] " << symlist[i] << " " \
                     << funcname << "+" << begin_offset << endl;
            } else { // demangle failes
                cerr << "[BT] " << symlist[i] << " " \
                     << begin_name << "+" << begin_offset << endl;
            }
        } else // parsing failed
            cerr << "[BT]" << symlist[i] << endl;
    }
    free(funcname);
    free(symlist);
}


