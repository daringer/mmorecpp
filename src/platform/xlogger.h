#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <ostream>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <typeinfo>

#include "../core/general.h"
#include "../core/exception.h"
#include "../core/converter.h"
#include "../core/xstring.h"

// default shows it all
// (set this before you include xlogger.h)
#ifndef XLOG_MIN_LOG_LVL
#define XLOG_MIN_LOG_LVL 1
#endif

// This is the main #DEFINE - do not change!
#define LOG(lvl, LOGID)                                                         \
  if (MM_NAMESPACE()::XLogger::check_loglevel(lvl, LOGID))                      \
  MM_NAMESPACE()::LogStream(MM_NAMESPACE()::XLogger::get(LOGID), lvl, __LINE__, \
                            __FILE__, __FUNCTION__)

#define FAKELOG() MM_NAMESPACE()::FakeLogStream()

// Some ready-to-use examples for special loggers
// all can be used like this:
// WARN << "my message goes here" << "another one" << 123 << " and add
// formatting";

#if XLOG_MIN_LOG_LVL <= 1
#define MAX_LOG LOG(1, LOGID)
#define IS_MAX_LOG true && MM_NAMESPACE()::XLogger::check_loglevel(1, LOGID)
#else
#define MAX_LOG \
  if (false)    \
  FAKELOG()
#define IS_MAX_LOG false
#endif

#if XLOG_MIN_LOG_LVL <= 2
#define MORE_DEBUG LOG(2, LOGID)
#define IS_MORE_DEBUG true && MM_NAMESPACE()::XLogger::check_loglevel(2, LOGID)
#else
#define MORE_DEBUG \
  if (false)       \
  FAKELOG()
#define IS_MORE_DEBUG false
#endif

#if XLOG_MIN_LOG_LVL <= 3
#define DEBUG LOG(3, LOGID)
#define IS_DEBUG true && MM_NAMESPACE()::XLogger::check_loglevel(3, LOGID)
#else
#define DEBUG \
  if (false)  \
  FAKELOG()
#define IS_DEBUG false
#endif

#if XLOG_MIN_LOG_LVL <= 5
#define INFO LOG(5, LOGID)
#define IS_INFO true && MM_NAMESPACE()::XLogger::check_loglevel(4, LOGID)
#else
#define INFO \
  if (false) \
  FAKELOG()
#define IS_INFO false
#endif

#if XLOG_MIN_LOG_LVL <= 7
#define WARN LOG(7, LOGID)
#define IS_WARN true && MM_NAMESPACE()::XLogger::check_loglevel(7, LOGID)
#else
#define WARN \
  if (false) \
  FAKELOG()
#define IS_WARN false
#endif

#if XLOG_MIN_LOG_LVL <= 10
#define ERROR LOG(10, LOGID)
#define IS_ERROR true && MM_NAMESPACE()::XLogger::check_loglevel(10, LOGID)
#else
#define ERROR \
  if (false)  \
  FAKELOG()
#define IS_ERROR false
#endif

namespace MM_NAMESPACE() {

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
    virtual ~BaseLoggerBackend();

    virtual void write(const std::string& text) = 0;
    virtual void init();
    virtual void cleanup();
  };

  // append log data to a file named 'fn'
  // re-open it for every write
  class FileBackend : public BaseLoggerBackend {
   public:
    FileBackend(const std::string& my_id, const std::string& fn,
                const std::string& my_name = "file-append");
    virtual ~FileBackend();

    void write(const std::string& msg);

   protected:
    virtual void write_to_fstream(std::ofstream& fd, const std::string& msg);

    std::string filename;
  };

  // the only difference to FileBackend: keep open file-descriptor!
  class PersistentFileBackend : public FileBackend {
   public:
    PersistentFileBackend(const std::string& my_id, const std::string& fn,
                          const std::string& my_name = "persistent-file-append");
    virtual ~PersistentFileBackend();

    void write(const std::string& msg);

   protected:
    void init();
    void cleanup();

    std::ofstream fd;
  };

  // Persistant _AND_ Buffered (likely the best performing log-file-backend)
  class BufferedFileBackend : public PersistentFileBackend {
   public:
    BufferedFileBackend(
          const std::string& my_id, const std::string& fn, const int& buf_size,
          const std::string& my_name = "buffered-pers-file-append");
    virtual ~BufferedFileBackend();

    void write(const std::string& msg);

    // (force) flush buffer and write it to file!
    void flush();

   protected:
    // flush with cleanup!
    void cleanup();

    std::string _buf;
    int _buffer_size;
  };

  // class MutexedFileBackend
  // class UDP/TCPServerBackend

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
    static bool check_loglevel(const int& lvl, const std::string& id) throw(
          NoSuchXLoggerAvailable);

    // quick init logger (e.g., for testing)
    static void quick_init(
          const std::string& newlogid,
          const std::string& console_format = "[%%FANCYLVL%%] %%MSG%%\n",
          const int& minloglvl = 3, bool strip = false);

    void add_backend(BaseLoggerBackend* back, const std::string& tmpl = "");

    void set_logging_template(BaseLoggerBackend* back, const std::string& tmpl);
    void set_time_format(const std::string& format);
    void set_loglvl_action(int loglvl, tLogActionPtr func);
    void set_loglvl_desc(int loglvl, const std::string& desc);

    void add_filename_filter(const std::string& fn);
    void remove_filename_filter(const std::string& fn);

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
    std::string id;
    std::string time_format;
    int min_loglvl;

    bool strip_msg;
    tStringList to_strip;

    std::set<std::string> fn_filter;

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
    typedef std::basic_ostream<char, std::char_traits<char> > tCout;
    typedef tCout& (*tEndl)(tCout&);

    LogStream(XLogger* logobj, int loglvl, int line, const std::string& fn,
              const std::string& func);
    ~LogStream();

    LogStream& operator<<(tEndl fnc);

    template <class T>
    LogStream& operator<<(const T& obj) {
      msg << obj;
      return *this;
    }
  };

  class FakeLogStream {
   public:
    template <class T>
    FakeLogStream& operator<<(const T& obj) {
      return *this;
    }
  };

  template <class T>
  LogStream& operator<<(LogStream & out, const std::set<T>& v) {
    for (const T& item : v)
      out << item << " ";
    return out;
  }

  template <class T>
  LogStream& operator<<(LogStream & out, const std::vector<T>& v) {
    for (const T& item : v)
      out << item << "  ";
    return out;
  }

  template <class K, class V>
  LogStream& operator<<(LogStream & out, const std::map<K, V>& m) {
    for (const auto& item : m)
      out << item.first << ": " << item.second << " ";
    return out;
  }

  template <class K, class V>
  LogStream& operator<<(LogStream & out, const std::unordered_map<K, V>& m) {
    for (const auto& item : m)
      out << item.first << ": " << item.second << " ";
    return out;
  }

  template <class K, class V>
  LogStream& operator<<(LogStream & out, const std::pair<K, V>& p) {
    out << "key: " << p.first << " val: " << p.second;
    return out;
  }
}
