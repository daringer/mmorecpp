#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <sstream>

#include "general.h"
#include "exception.h"
#include "xstring.h"

#define LOG(lvl, logid)  \
  TOOLS::LogStream(TOOLS::Logger::get(logid), lvl,__LINE__,__FILE__,__FUNCTION__) 
#define DEBUG \
    TOOLS::LogStream(TOOLS::Logger::get(logid), 10,__LINE__,__FILE__,__FUNCTION__) 
#define INFO \
    TOOLS::LogStream(TOOLS::Logger::get(logid), 7,__LINE__,__FILE__,__FUNCTION__) 
#define WARN \
    TOOLS::LogStream(TOOLS::Logger::get(logid), 5,__LINE__,__FILE__,__FUNCTION__) 
#define ERROR \
    TOOLS::LogStream(TOOLS::Logger::get(logid), 3,__LINE__,__FILE__,__FUNCTION__) 

namespace TOOLS {

  DEFINE_EXCEPTION(BackendFailedToWrite, BaseException);
  DEFINE_EXCEPTION(NoSuchLoggerAvailable, BaseException);

  class Logger;

  typedef std::map<std::string, Logger*> tLoggerMap;

  class BaseLoggerBackend {
    public:
      virtual void write(const std::string& text) = 0;
      virtual void init_sink() = 0;
      virtual void cleanup_sink() = 0;
  }; // must they be pure virtual, (*sink)

  class FileBackend : public BaseLoggerBackend {
    public:
      std::string filename;

      FileBackend(const std::string& fn);

      void write(const std::string& msg);
  };
  //class PersistentFileBackend

  class ConsoleBackend : public BaseLoggerBackend {
    //public:
      //void 
      // write console backend, and think about multiple backends inside logger
  }

  
  class Logger {
    public:
      Logger(const std::string& id, BaseLoggerBackend* back);

      static tLoggerMap log_map;
      static Logger* get(const std::string& id) throw (NoSuchLoggerAvailable);

      void set_logging_template(const std::string& tmpl);
      void set_time_format(const std::string& format);

      void log_msg(std::string& data, int loglevel, std::string& line, const std::string& fn, const std::string& func);

    private:
      BaseLoggerBackend* backend;
      std::string log_template;
      std::string time_format;
  };
  tLoggerMap Logger::log_map;

  class LogStream : public std::ostringstream {
    public:
      Logger* obj;
      int loglvl;
      std::string line;
      std::string fn;
      std::string func;

      LogStream(Logger* logobj, int loglvl, const std::string& line, const std::string& fn, const std::string& func);
      ~LogStream();
  };

}
#endif
