
#include "testing_suite.h"


#include "fs.cc"
#include "xstring.cc"
#include "xregex.cc"
#include "executor.cc"

using namespace std;
using namespace TOOLS;

int main(int argc, char* argv[]) {

  UNIT_TEST::TestFramework f(argc, argv);
  f.add_suite<FSToolsTestSuite>("TOOLS::FS::Path");
  f.add_suite<XStringToolsTestSuite>("TOOLS::XString");
  f.add_suite<XRegexToolsTestSuite>("TOOLS::XRegex");
  f.add_suite<ExecutorToolsTestSuite>("TOOLS::Executor");
  f.run();
  f.show_results();


}
