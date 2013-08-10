#include "testing_suite.h"

#include "fs.cc"
#include "xregex.cc"
#include "executor.cc"
#include "xstring.cc"
#include "xtime.cc"
#include "xlogger.cc"
#include "mem_tracker.cc"
#include "template_parser.cc"

using namespace std;
using namespace TOOLS;

int main(int argc, char* argv[]) {

  set_terminate(TOOLS::tools_lib_exception_handler);

  UNIT_TEST::TestFramework f(argc, argv);
  f.add_suite<FSToolsTestSuite>("TOOLS::FS::Path");
  f.add_suite<XRegexToolsTestSuite>("TOOLS::XRegex");
  f.add_suite<ExecutorToolsTestSuite>("TOOLS::Executor");
  f.add_suite<XStringToolsTestSuite>("TOOLS::XString");
  f.add_suite<XTimeToolsTestSuite>("TOOLS::XTime");
  f.add_suite<XLoggerToolsTestSuite>("TOOLS::XLogger");
  f.add_suite<TemplateParserToolsTestSuite>("TOOLS::TemplateParser");
  f.add_suite<MemTrackerToolsTestSuite>("TOOLS::MemTracker");
  f.run();
  f.show_result_overview();

  return 0;
}
