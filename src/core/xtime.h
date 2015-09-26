#ifndef XTIME_H
#define XTIME_H

#include <sys/time.h>

#include <string>
#include <ostream>

#define BUFSIZE 256

namespace MM_NAMESPACE() {

typedef uint64_t tMicroTime; 
const tMicroTime us2ms_factor = 1e3;
const tMicroTime us2s_factor = 1e6;

// call start() + stop() multiple times to sum up multiple times/durations 
class XTime {
 private:
  // raw plain-C time data (backend)
  struct timeval raw_start;
  struct timeval raw_end;

  // cache passed us 
  tMicroTime passed_us;

 public:
  // is currently active? start() was called and stop() not yet!
  bool active;
  // number of start() + stop() calls (cycles)
  int cycles;

  // set 'auto_start' to 'true' to directly start after obj construction
  XTime(bool auto_start = false);

  // don't call start() twice in a row without stop() i.e.: active == false
  void start();
  // only call AFTER start() i.e.: active == true
  void stop();
  // reset cumulated times/durations -> 'passed_us' = 0
  void reset();

  // return various representations of the measured time
  // only call AFTER stop() - 'active' must be 'false'
  tMicroTime diff_us() const;
  tMicroTime diff_ms() const;
  tMicroTime diff_s() const;
};

class XDateTime {
 private:
  time_t rawtime;
  struct tm* timeinfo;
  char buf[BUFSIZE];
  std::string fmt_template;

  void render();

 public:
  XDateTime(const XDateTime& obj);
  XDateTime(const std::string& fmt = "%c");
  XDateTime(const time_t& stamp);

  void set_format(const std::string& fmt);
  std::string format() const;

  time_t get_raw_stamp() const;
};

std::ostream& operator<<(std::ostream& os, const XDateTime& dt);
}

#endif
