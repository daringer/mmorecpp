#include <time.h>

#include "xlogger.h"
#include "xtime.h"
#include "xstring.h"
#include "testing_suite.h"

using namespace std;
using namespace TOOLS;

tXLoggerMap XLogger::log_map;

// Logger main class
// Make sure you place a instance of this logger inside the heap! No STACK use!
XLogger::XLogger(const string& id) :
  id(id),
  log_template("(%%FANCYLVL%%) %%TIME%% || %%MSG%% (%%FILE%%, line: %%LINE%%, caller: %%FUNC%%)\n"),
  time_format("%d.%m.%Y - %T") {
  backends.clear();
  if(log_map.find(id) != log_map.end())
    throw LoggerIDAlreadyRegistered(id);
  log_map[id] = this;
}

// cleaning up / deleting backends
XLogger::~XLogger() {
  log_msg("Exiting logger: " + id, 5);
  for(tBackendIter i=backends.begin(); i!=backends.end(); ++i) {
    (*i)->cleanup();
    delete(*i);
  }
}

// adding a backend
void XLogger::add_backend(BaseLoggerBackend* back) {
  backends.push_back(back);
  back->init();
  back->write(render_msg("Initializing backend for logger: " + id, 5, 0, "---", "---"));
}

// static global getter for the registered XLoggers
XLogger* XLogger::get(const string& id) throw(NoSuchXLoggerAvailable) {
  if(log_map.find(id) != log_map.end())
    return log_map[id];
  throw NoSuchXLoggerAvailable(id);
}

// accept %%MSG%% , %%TIME%%, %%FANCYLVL%%, %%LOGLVL%%, %%FILE%% , %%FUNC%% , %%LINE%%
void XLogger::set_logging_template(const string& tmpl) {
  log_template = tmpl;
}

// %%TIME%% is formated according to set_time_format() and strftime()
void XLogger::set_time_format(const string& format) {
  time_format = format;
}

// decoration
string XLogger::get_fancy_level(int lvl) {
  switch(lvl) {
    case 10:
      return "E";
      break;;
    case 7:
      return "W";
      break;;
    case 5:
      return "i";
      break;;
    case 3:
      return "o";
      break;;
    default:
      return "?";
      break;;
  }
}

// render the message
string XLogger::render_msg(const string& data, int loglevel, int line, const string& fn, const string& func) {
  XString msg(log_template);
  return msg.subs("%%MSG%%", data).\
         subs("%%TIME%%", XDateTime(time_format).format()).\
         subs("%%LOGLVL%%", TOOLS::str(loglevel)).\
         subs("%%FANCYLVL%%", get_fancy_level(loglevel)). \
         subs("%%FILE%%", fn).\
         subs("%%LINE%%", TOOLS::str(line)).\
         subs("%%FUNC%%", func);
}

// log give message and related data to _all_ backends
void XLogger::log_msg(const string data, int loglevel, int line, const string fn, const string func) {
  // just to be sure to avoid empty log msgs
  if(data.length() == 0)
    return;

  for(tBackendIter i=backends.begin(); i!=backends.end(); ++i)
    (*i)->write(render_msg(data, loglevel, line, fn, func));
}

// short-cut method for not source-code related logging
void XLogger::log_msg(const string data, int loglevel) {
  log_msg(data, loglevel, 0, "---", "---");
}

// LogStream magic happens here
// - get an instance of LogStream
// - stream data into it
// - destruct it
// - save the streamed data for the logger (during object destruction)
LogStream::LogStream(XLogger* logobj, int loglvl, int line, const std::string& fn, const std::string& func)
  : obj(logobj), loglvl(loglvl), line(line), fn(fn), func(func) { }

LogStream::~LogStream() {
  obj->log_msg(string(msg.str()), loglvl, line, string(fn), string(func));
}

// BaseLoggerBackend abstract class
BaseLoggerBackend::BaseLoggerBackend(const string& my_id)
  : id(my_id) { }
void BaseLoggerBackend::init() { }
void BaseLoggerBackend::cleanup() { }

// FileBackend realization
FileBackend::FileBackend(const string& my_id, const string& fn)
  : filename(fn), BaseLoggerBackend(my_id) { }

void FileBackend::write(const string& msg) {
  ofstream fd(filename.c_str(), ios::app);
  fd.write(msg.c_str(), msg.size());
  fd.close();
}

// ConsoleBackend realization
ConsoleBackend::ConsoleBackend(const string& my_id)
  : BaseLoggerBackend(my_id) {}
void ConsoleBackend::write(const string& msg) {
  cout << msg;
}

// MemoryBackend
MemoryBackend::MemoryBackend(const string& my_id) : BaseLoggerBackend(my_id) {
  log_msgs.clear();
}
void MemoryBackend::write(const string& msg) {
  log_msgs.push_back(msg);
}
