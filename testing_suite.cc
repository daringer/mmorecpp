#include "testing_suite.h"

using namespace std;
using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;

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

void TestSuite::execute_tests(const string& suite_name, const string& only_test, const bool& return_on_fail) {
  for(tTestIter i=tests.begin(); i!=tests.end(); ++i) {

    i->second.res.id = suite_name + "::" + i->first;

    // if executing choosen test
    if(only_test != "" && i->second.res.id.find(only_test) != 0)
        continue;

    i->second.res.run = true;

    active_test = &i->second;

    setup();
    after_setup();

    i->second.res.timer.start();

    try {
      (i->second.object->*i->second.method)(true, return_on_fail);
    } catch(TOOLS::BaseException& e) {
      i->second.res.details.append(
        "[E] test failed - exception caught: " + e.output + " - ");
      i->second.res.result = false;
    } catch(exception& e) {
      i->second.res.details.append(
        "[E] test failed - std::exception caught: " + str(e.what()) + " - ");
      i->second.res.result = false;
    }

    i->second.res.timer.stop();

    tear_down();
    after_tear_down();

    active_test = NULL;
    i->second.res.show(false);
  }
}

TestResult::TestResult(const string& testid, bool res, const string& details)
  : id(testid), result(res), details(details) {}

TestResult::TestResult() : id(""), result(true), run(false) {}

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
       setw(20) << right << rating;

  double diff = timer.diff_s() * 1000.0 + timer.diff_us() / 1000.0;
  cout << " ->" << setw(9) << right << diff << "ms"  << endl;

  if(details != "" && ((!show_details && !result) || show_details))
    cout << "[i]    " << details << endl;

}

TestFramework::TestFramework(int argc, char* argv[]) {
  ConfigManager conf(argv[0]);

  ConfigGroup& grp = conf.new_group("Main Options");
  grp.new_option<bool>("debug", "Show test details", "d"). \
  set_default(false);

  grp.new_option<string>("execute-test", "Execute test(s) by name", "t"). \
  set_default("");
  grp.new_option<bool>("stay-on-fail", "Stay in test on fail", "r"). \
  set_default(false);

  // ConfigManager init finished, start parsing file, then cmdline
  try {
    conf.parse_config_file("noname.conf");
    conf.parse_cmdline(argc, argv);
    cout << "[+] Parsing commandline complete" << endl;
  } catch(ConfigManagerException& e) {
    e.show();
    conf.usage(cout);
    exit(1);
  }

  show_details = conf.get<bool>("debug");
  execute_test = conf.get<string>("execute-test");
  return_on_fail = !conf.get<bool>("stay-on-fail");
}

TestFramework::~TestFramework() {
  for(tTestSuiteIter i=test_suites.begin(); i!=test_suites.end(); ++i)
    delete i->second;
}

void TestFramework::run() {
  for(tTestSuiteIter i=test_suites.begin(); i!=test_suites.end(); ++i)
    i->second->execute_tests(i->first, execute_test, return_on_fail);
}

void TestFramework::show_result_overview() {
  int good = 0;
  int bad = 0;
  int all_tests = 0;

  for(tTestSuiteIter i=test_suites.begin(); i!=test_suites.end(); ++i) {
    all_tests += i->second->check_count;
    for(tTestIter j=i->second->tests.begin(); j!=i->second->tests.end(); ++j) {
      if(j->second.res.run)
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

void print_stacktrace(uint max_frames) {
  cerr << "[BT] Showing stacktrace: " << endl;

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
  for(int i=1; i<addrlen; ++i) {
    char* begin_name = 0;
    char* begin_offset = 0;
    char* end_offset = 0;

    // find parentheses and +address offset surrounding the mangled name:
    // ./module(function+0x15c) [0x8048a6d]
    for(char* p = symlist[i]; *p; ++p) {
      if(*p == '(')
        begin_name = p;
      else if(*p == '+')
        begin_offset = p;
      else if(*p == ')' && begin_offset) {
        end_offset = p;
        break;
      }
    }

    if(begin_name && begin_offset && end_offset
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
      if(status == 0) {
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


