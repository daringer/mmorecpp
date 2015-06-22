#include <stdlib.h>
#include <unistd.h>

#include "tools.h"

using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;
using namespace std;

START_SUITE(XTimeToolsTestSuite) {
  REG_TEST(interval)
  REG_TEST(interval_autostart)
  REG_TEST(date_default_format)
  REG_TEST(own_format)
}

string own_fmt;
int sleep_duration;

virtual void setup() {
  own_fmt = "%R %T";
  sleep_duration = 1;
}

virtual void tear_down() {}

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
  CHECK(t.diff_us() >= sleep_duration * 1e6);
  CHECK(t.diff_ms() >= sleep_duration * 1e3);
  CHECK(t.diff_s() >= sleep_duration);
}

MAKE_TEST(interval_autostart) {
  XTime t(true);
  sleep(sleep_duration);
  t.stop();
  CHECK(t.diff_us() >= sleep_duration * 1e6);
  CHECK(t.diff_ms() >= sleep_duration * 1e3);
  CHECK(t.diff_s() >= sleep_duration);
}
END_SUITE()
