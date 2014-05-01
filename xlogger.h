#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <sstream>
#include <ostream>
#include <vector>
#include <map>
#include <typeinfo>

#include "general.h"
#include "exception.h"
#include "xstring.h"

// default shows it all 
// (set this before you include xlogger.h)
#ifndef XLOG_MIN_LOG_LVL
#define XLOG_MIN_LOG_LVL 1
#endif

// This is the main #DEFINE - do not change!
#define LOG(lvl, LOGID) \
  if(TOOLS::XLogger::check_loglevel(lvl, LOGID)) \
    TOOLS::LogStream(TOOLS::XLogger::get(LOGID), lvl,__LINE__,__FILE__,__FUNCTION__)

#define FAKELOG() \
    TOOLS::FakeLogStream()

// Some ready-to-use examples for special loggers
// all can be used like this:
// WARN << "my message goes here" << "another one" << 123 << " and add formatting";

#if XLOG_MIN_LOG_LVL <= 1
#define PROFILING LOG(1, LOGID)
#else
#define PROFILING FAKELOG()
#endif 

#if XLOG_MIN_LOG_LVL <= 3
#define DEBUG LOG(3, LOGID)
#else
#define DEBUG FAKELOG() 
#endif 

#if XLOG_MIN_LOG_LVL <= 5
#define INFO LOG(5, LOGID)
#else
#define INFO FAKELOG()
#endif 

#if XLOG_MIN_LOG_LVL <= 7
#define WARN LOG(7, LOGID)
#else
#define WARN FAKELOG()
#endif 

#if XLOG_MIN_LOG_LVL <= 10
#define ERROR LOG(10, LOGID)
#else
#define ERROR FAKELOG()
#endif 


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
    static bool check_loglevel(const int& lvl, const std::string& id) throw(NoSuchXLoggerAvailable);

    void add_backend(BaseLoggerBackend* back, const std::string& tmpl="");

    void set_logging_template(BaseLoggerBackend* back, const std::string& tmpl);
    void set_time_format(const std::string& format);
    void set_loglvl_action(int loglvl, tLogActionPtr func);
    void set_loglvl_desc(int loglvl, const std::string& desc);

    void set_min_loglvl(int loglvl);

    void log_msg(const std::string data, int loglevel, int line, 
                 const std::string fn, const std::string func);
    void log_msg(const std::string data);

    void set_msg_stripping(const bool& strip_msg);
    void set_msg_stripping(const bool& strip_msg, const tStringList& trims);

  private:
    tBackendList backends;
    tLevelActionMap lvl2action;
    tLevelDescMap lvl2desc;
    tBackendTemplateMap back2tmpl;
    std::string time_format;
    std::string id;
    int min_loglvl;

    bool strip_msg;
    tStringList to_strip;

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
    
    /*template<typename R, typename P>
      LogStream& operator<<(R & (*pf)(P &)) {
        msg << pf;
        return *this;
      }*/

    template<class T>
      LogStream& operator<<(const T& obj) {
        msg << obj;
        return *this;
      }
};

class FakeLogStream {
  public:
    template<class T>
      FakeLogStream& operator<<(const T& obj) {
        return *this;
      }
};

}


#endif
