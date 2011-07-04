#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <sstream>

#include "general.h"
#include "exception.h"
#include "xstring.h"

#define LOG(lvl)  \
  if(TOOLS::Logger::instance == NULL) { \
    std::cerr << "You are trying to log, without having the logger initialized - exiting..." << endl; \
    exit(1); \
  } TOOLS::LogStream(TOOLS::Logger::instance,lvl,__LINE__,__FILE__,__FUNCTION__) 

#define DEBUG LOG(10)
#define INFO LOG(7)
#define WARN LOG(3)
#define ERROR LOG(1)

namespace TOOLS {

  DEFINE_EXCEPTION(BackendFailedToWrite, BaseException);
  
  class BaseLoggerBackend {
    public:
      void write(const std::string& text) = 0;
  };

  class FileBackend : public BaseLoggerBackend {
    public:
      void write(const std::string&);
  };
  class ReOp

  
  class Logger {
    public:
      Logger(BaseLoggerBackend* backend);

      static Logger* instance;

      void set_logging_template(const std::string& tmpl);
      void set_time_format(const std::string& format);

      void log_msg(const std::string& data);

    private:
      BaseLoggerBackend* backend;
      string log_template;
      string time_format;
  };

  Logger* Logger::instance = NULL;

  class LogStream : public std::ostringstream {
    public:
      LogStream(Logger* logobj, int loglvl, const std::string& line, const std::string& fn, const std::string& func);
      ~LogStream();
  };

}
#endif
