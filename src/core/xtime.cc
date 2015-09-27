#include "strings.h"
#include "time.h"
#include "assert.h"

#include "general.h"
#include "xtime.h"

using namespace MM_NAMESPACE();

using std::string;
using std::ostream;

XTime::XTime(bool auto_start) : passed_us(0), active(false), cycles(0) {
  if (auto_start)
    start();
}

void XTime::start() {
  assert(!active);
  gettimeofday(&raw_start, NULL);
  active = true;
}

void XTime::stop() {
  assert(active);
  gettimeofday(&raw_end, NULL);
  active = false;

  // update cache (passed_us) accordingly - attention: '.tv_usec' is % 1e6
  passed_us += (raw_end.tv_usec - raw_start.tv_usec) +
               (raw_end.tv_sec - raw_start.tv_sec) * us2s_factor;
  cycles++;
}

void XTime::reset() {
  passed_us = 0;
  active = false;
  cycles = 0;
}

tMicroTime XTime::diff_s() const {
  assert(!active);
  return passed_us / us2s_factor;
}

tMicroTime XTime::diff_ms() const {
  assert(!active);
  return passed_us / us2ms_factor;
}

tMicroTime XTime::diff_us() const {
  assert(!active);
  return passed_us;
}

XDateTime::XDateTime(const XDateTime& obj)
    : rawtime(obj.rawtime), timeinfo(obj.timeinfo),
      fmt_template(obj.fmt_template) {
  render();
}

XDateTime::XDateTime(const string& fmt)
    : rawtime(time(NULL)), timeinfo(localtime(&rawtime)), fmt_template(fmt) {
  render();
}

XDateTime::XDateTime(const time_t& stamp)
    : rawtime(stamp), timeinfo(localtime(&stamp)), fmt_template("") {
  render();
}

void XDateTime::render() {
  bzero(&buf, BUFSIZE);
  timeinfo = localtime(&rawtime);
  ::strftime(buf, BUFSIZE, fmt_template.c_str(), timeinfo);
}

string XDateTime::format() const {
  return string(buf);
}

void XDateTime::set_format(const string& fmt) {
  fmt_template = fmt;
  render();
}

time_t XDateTime::get_raw_stamp() const {
  return rawtime;
}

ostream& operator<<(ostream& os, const XDateTime& dt) {
  return (os << dt.format());
}
