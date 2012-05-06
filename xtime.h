#ifndef XTIME_H
#define XTIME_H

#include <sys/time.h>

#include <string>

namespace TOOLS {
  class XTime {
    private:
      struct timeval raw_start;
      struct timeval raw_end;

    public:
      XTime(bool auto_start=false);

      void start();
      void stop();

      long diff_us();
      long diff_s();
  };
}

#endif
