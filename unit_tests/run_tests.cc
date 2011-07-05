
#include "testing_suite.h"


#include "fs.cc"
#include "xstring.cc"

using namespace std;
using namespace TOOLS;

int main() {

  UNIT_TEST::TestFramework f;
  f.add_suite<FSToolsTestSuite>("TOOLS::FS::Path");
  f.add_suite<XStringToolsTestSuite>("TOOLS::XString");
  f.run();
  f.show_results();


}
