#include "strings.h"
#include "time.h" 

#include "xtime.h"

using namespace TOOLS;
using namespace std;

XTime::XTime(bool auto_start) {
    if (auto_start)
        start();
}

void XTime::start() {
    gettimeofday(&raw_start, NULL);
}

void XTime::stop() {
    gettimeofday(&raw_end, NULL);
}

long XTime::diff_s() {
    return raw_end.tv_sec - raw_start.tv_sec;
}

long XTime::diff_us() {
    // tv_usec is % 1e6
    return (raw_end.tv_usec - raw_start.tv_usec) + diff_s()*1e6;
}

XDateTime::XDateTime(const XDateTime& obj) : rawtime(obj.rawtime), timeinfo(obj.timeinfo), fmt_template(obj.fmt_template) {
  render();
}

XDateTime::XDateTime(const std::string& fmt) : rawtime(time(NULL)), timeinfo(localtime(&rawtime)), fmt_template(fmt) {
  render();
}

XDateTime::XDateTime(const time_t& stamp) : rawtime(stamp), timeinfo(localtime(&stamp)), fmt_template("") {
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

ostream& operator<<(ostream& os, const XDateTime& dt) {
  return (os << dt.format());
}


