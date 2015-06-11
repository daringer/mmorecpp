// have to globally activate it,
// FIXME: testing suite should separate and compile each TestSuite on its own
#define MEMORY_TRACKER_ACTIVE 1

#define XLOG_MIN_LOG_LVL 1

#include "../src/testing_suite.h"

#include "fs.cc"
#include "xtime.cc"
#include "template_parser.cc"
#include "xregex.cc"
#include "executor.cc"
#include "xstring.cc"
#include "xlogger.cc"

using namespace std;
using namespace TOOLS;

int main(int argc, char* argv[]) {
  // so in die richtung
  struct sigaction act;
  act.sa_handler = TOOLS::signal_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  sigaction(SIGSEGV, &act, 0);
  // act.sa_flags = SA_SIGINFO;
  // sa.sa_sigaction = segfault_sigaction;

  // set_terminate(TOOLS::tools_lib_exception_handler);

  UNIT_TEST::TestFramework f(argc, argv);
  f.add_suite<FSToolsTestSuite>("TOOLS::FS::Path");
  f.add_suite<XTimeToolsTestSuite>("TOOLS::XTime");
  f.add_suite<TemplateParserToolsTestSuite>("TOOLS::TemplateParser");
  f.add_suite<XRegexToolsTestSuite>("TOOLS::XRegex");
  f.add_suite<ExecutorToolsTestSuite>("TOOLS::Executor");
  f.add_suite<XStringToolsTestSuite>("TOOLS::XString");
  f.add_suite<XLoggerToolsTestSuite>("TOOLS::XLogger");
  f.run();
  f.show_result_overview();

  return 0;
}
