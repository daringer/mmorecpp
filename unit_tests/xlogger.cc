#include <stdlib.h>
#include <unistd.h>

#include "xlogger.h"

#include "testing_suite.h"


using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;
using namespace std;

#define LOGID "system"

#ifndef GENERATE_HEADER
void loglvl_action() {
  INFO << "Loglvl action successful!";
}
#endif

START_SUITE(XLoggerToolsTestSuite) {
      REG_TEST(simple)
      REG_TEST(loglvl_action)
    }
    
    XLogger* xlog;
    MemoryBackend* mb;

    virtual void setup() {
      xlog = new XLogger(LOGID);
      //xlog->add_backend(new ConsoleBackend(LOGID));
      mb = new MemoryBackend(LOGID);
      xlog->add_backend(mb);
    }

    virtual void tear_down() {
      delete xlog;
    }

    MAKE_TEST(simple) {
      CHECK(mb->log_msgs.size() == 1);
      int tmp = 123;
      DEBUG << "starting logger testing!";
      INFO << "more info logging, let's log an integer: " << tmp;
      WARN << "oh oh, this is a warning - this could end bad!";
      ERROR << "NOOOOO!!! ERRRORRRR!!!!";
      if(XLOG_MIN_LOG_LVL > 5)
        CHECK(mb->log_msgs.size() == 5);
    }

    MAKE_TEST(loglvl_action) {
      INFO << "STARTING";
      xlog->set_loglvl_action(10, loglvl_action);
      ERROR << "ERRRRORRRRR, should trigger action";
      CHECK(mb->log_msgs.size() == 3);
    }

END_SUITE()
