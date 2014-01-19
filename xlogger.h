#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <sstream>
#include <ostream>
#include <vector>
#include <map>

#include "general.h"
#include "exception.h"
#include "xstring.h"

// This is the main #DEFINE - do not change!
#define LOG(lvl, LOGID)  \
  TOOLS::LogStream(TOOLS::XLogger::get(LOGID), lvl,__LINE__,__FILE__,__FUNCTION__)

// Some ready-to-use examples for special loggers
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
    std::string name;

    BaseLoggerBackend(const std::string& my_id, const std::string& my_name);

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
    typedef void (*tLogActionPtr)(void);
    typedef std::map<int, tLogActionPtr> tLevelActionMap;
    typedef std::map<int, std::string> tLevelDescMap;
    typedef std::map<BaseLoggerBackend*, std::string> tBackendTemplateMap;

    XLogger(const std::string& id);
    ~XLogger();

    static tXLoggerMap log_map;
    static XLogger* get(const std::string& id) throw(NoSuchXLoggerAvailable);

    void add_backend(BaseLoggerBackend* back, const std::string& tmpl="");

    void set_logging_template(BaseLoggerBackend* back, const std::string& tmpl);
    void set_time_format(const std::string& format);
    void set_loglvl_action(int loglvl, tLogActionPtr func);
    void set_loglvl_desc(int loglvl, const std::string& desc);

    void log_msg(const std::string data, int loglevel, int line, 
                 const std::string fn, const std::string func);
    void log_msg(const std::string data);

  private:
    tBackendList backends;
    tLevelActionMap lvl2action;
    tLevelDescMap lvl2desc;
    tBackendTemplateMap back2tmpl;
    std::string time_format;
    std::string id;

    std::string render_msg(BaseLoggerBackend* back, const std::string& data, 
                           int loglevel, int line, const std::string& fn, 
                           const std::string& func);
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
    LogStream(XLogger* logobj, int loglvl, int line, const std::string& fn, 
              const std::string& func);
    ~LogStream();

    // STILL NO endl support here, weird error:
    // template argument deduction/substitution failed:
    // note: ‘TOOLS::LogStream’ is not derived from 
    //       ‘std::basic_ostream<_CharT, _Traits>’
    // AGGRGRRRRr
    template<class T>
    LogStream& operator<<(const T& obj) {
      msg << obj;
      return *this;
    }
};

}

#endif
