#include "xtime.h"

using namespace TOOLS;

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
    return raw_end.tv_usec - raw_start.tv_usec;
}


