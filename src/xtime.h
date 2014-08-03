#ifndef XTIME_H
#define XTIME_H

#include <sys/time.h>

#include <string>
#include <ostream>

#define BUFSIZE 256

namespace TOOLS {
class XTime {
 private:
  struct timeval raw_start;
  struct timeval raw_end;

 public:
  XTime(bool auto_start = false);

  void start();
  void stop();

  long diff_us();
  long diff_s();
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
