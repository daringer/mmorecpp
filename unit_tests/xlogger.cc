#include <stdlib.h>
#include <unistd.h>

#include "xlogger.h"

#include "testing_suite.h"


using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;
using namespace std;

#define LOGID "system"

class XLoggerToolsTestSuite : public TestSuite {
  public:
    XLogger* xlog;
    MemoryBackend* mb;

    XLoggerToolsTestSuite() {
      REG_TEST(&XLoggerToolsTestSuite::test_simple)
    }

    virtual void setup() {
      XLogger* xlog = new XLogger(LOGID);
      //xlog->add_backend(new ConsoleBackend());
      mb = new MemoryBackend("mem");
      xlog->add_backend(mb);
    }

    virtual void tear_down() {
      delete xlog;
    }

    MAKE_TEST(simple) {
      CHECK(mb->log_msgs.size() == 1);
      INFO << "WHHHOOOOT" << " MOREEEEE" << ": " << 1234 << "\n"; // not yet working ----> endl;
      CHECK(mb->log_msgs.size() == 2);
    }

    MAKE_TEST(endl) {

    }

};