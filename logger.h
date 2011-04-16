#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>

namespace TOOLS {

enum LOG_TYPE {
  LOG_DEBUG,
  LOG_ERROR,
  LOG_INFO
};

class LoggerStream {
  public:
    LoggerStream(LOG_TYPE type);
    ~LoggerStream();

};

class Logger {
  public:
    Logger(const std::string& logfile);
    ~Logger();

};
}
#endif
