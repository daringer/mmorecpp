#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include "general.h"
#include "exception.h"
#include "xstring.h"

// This is the main #DEFINE - do not change!
#define LOG(lvl, LOGID)  \
  TOOLS::LogStream(TOOLS::XLogger::get(LOGID), lvl,__LINE__,__FILE__,__FUNCTION__)

// Some examples for special loggers
// all can be used like this:
// WARN << "my message goes here" << "another one" << 123 << " and add formatting";
#define DEBUG \
  LOG(3, LOGID)
#define INFO \
  LOG(5, LOGID)
#define WARN \
  LOG(7, LOGID)
#define ERROR \
  LOG(10, LOGID)


namespace TOOLS {

DEFINE_EXCEPTION(BackendFailedToWrite, BaseException)
DEFINE_EXCEPTION(NoSuchXLoggerAvailable, BaseException)
DEFINE_EXCEPTION(NoSuchBackendAvailable, BaseException)
DEFINE_EXCEPTION(NoMessageProvided, BaseException)
DEFINE_EXCEPTION(LoggerIDAlreadyRegistered, BaseException)

class XLogger;

typedef std::map<std::string, XLogger*> tXLoggerMap;

class BaseLoggerBackend {
  public:
    std::string id;

    BaseLoggerBackend(const std::string& my_id);

    virtual void write(const std::string& text) = 0;
    virtual void init();
    virtual void cleanup();
};

class FileBackend : public BaseLoggerBackend {
  public:
    std::string filename;

    FileBackend(const std::string& my_id, const std::string& fn);
    void write(const std::string& msg);
};
//class PersistentFileBackend
//class MutexedFileBackend
//class UDP/TCPServerBackend

class ConsoleBackend : public BaseLoggerBackend {
  public:
    ConsoleBackend(const std::string& my_id);
    void write(const std::string& msg);
};

class MemoryBackend : public BaseLoggerBackend {
  public:
    tStringList log_msgs;

    MemoryBackend(const std::string& my_id);
    void write(const std::string& msg);
};


class XLogger {
  public:
    typedef std::vector<BaseLoggerBackend*> tBackendList;
    typedef tBackendList::iterator tBackendIter;

    XLogger(const std::string& id);
    ~XLogger();

    static tXLoggerMap log_map;
    static XLogger* get(const std::string& id) throw(NoSuchXLoggerAvailable);

    void add_backend(BaseLoggerBackend* back);

    void set_logging_template(const std::string& tmpl);
    void set_time_format(const std::string& format);

    void log_msg(const std::string data, int loglevel, int line, const std::string fn, const std::string func);
    void log_msg(const std::string data, int loglevel);

  private:
    tBackendList backends;
    std::string log_template;
    std::string time_format;
    std::string id;

    std::string render_msg(const std::string& data, int loglevel, int line, const std::string& fn, const std::string& func);
    std::string get_fancy_level(int lvl);
};


class LogStream {
  private:
    XLogger* obj;
    int loglvl;
    int line;
    std::string fn;
    std::string func;
    std::stringstream msg;

  public:
    LogStream(XLogger* logobj, int loglvl, int line, const std::string& fn, const std::string& func);
    ~LogStream();

    template<class T>
    LogStream& operator<<(T obj) {
      msg << obj;
      return *this;
    }

    /* something like this to allow << endl
    template<std::ostream&>
    LogStream& operator<<(std::ostream& os) {
      msg << os;
      return *this;
    }*/
};

// namepace TOOLS end
}

#endif
