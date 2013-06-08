#include <stdlib.h>
#include <unistd.h>

#include "xtime.h"

#include "testing_suite.h"


using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;
using namespace std;

class XTimeToolsTestSuite : public TestSuite {
  public:
    string own_fmt;
    int sleep_duration;

    XTimeToolsTestSuite() {
      REG_TEST(&XTimeToolsTestSuite::test_interval)
      REG_TEST(&XTimeToolsTestSuite::test_interval_autostart)
      REG_TEST(&XTimeToolsTestSuite::test_date_default_format)
      REG_TEST(&XTimeToolsTestSuite::test_own_format)
    }

    virtual void setup() {
      own_fmt = "%R %T";
      sleep_duration = 1;
    }

    virtual void tear_down() {
    }

    MAKE_TEST(date_default_format) {
      XDateTime dt;
      CHECK(dt.format().length() > 0);
    }

    MAKE_TEST(own_format) {
      XDateTime dt;
      dt.set_format(own_fmt);
      CHECK(dt.format().length() > 0);
    }

    MAKE_TEST(interval) {
      XTime t;
      t.start();
      sleep(sleep_duration);
      t.stop();
      CHECK(t.diff_us() >= sleep_duration*1e6);
      CHECK(t.diff_s() >= sleep_duration);
    }

    MAKE_TEST(interval_autostart) {
      XTime t(true);
      sleep(sleep_duration);
      t.stop();
      CHECK(t.diff_us() >= sleep_duration*1e6);
      CHECK(t.diff_s() >= sleep_duration);
    }
};
