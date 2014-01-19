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
  time_format("%d.%m - %T") {
  
  if(log_map.find(id) != log_map.end())
    throw LoggerIDAlreadyRegistered(id);
  
  backends.clear();
  log_map[id] = this;
  set_loglvl_desc(0, "?");
}

// cleaning up / deleting backends
XLogger::~XLogger() {
  log_msg("[XLogger] Exiting logger: " + id + "\n");
  for(tBackendIter i=backends.begin(); i!=backends.end(); ++i) {
    (*i)->cleanup();
    delete *i;
  }
  log_map.erase(id);
}

// adding a backend
void XLogger::add_backend(BaseLoggerBackend* back, const string& tmpl) {
  if(tmpl == "")
    back2tmpl[back] = "[%%FANCYLVL%%] %%TIME%% (%%FILE%%:%%LINE%% call: %%FUNC%%) || %%MSG%%\n";
  else
    back2tmpl[back] = tmpl;

  backends.push_back(back);
  back->init();
  back->write("[XLogger] Initializing backend (" + back->name + ") for logger: " + id + "\n");
}

// static global getter for the registered XLoggers
XLogger* XLogger::get(const string& id) throw(NoSuchXLoggerAvailable) {
  if(log_map.find(id) != log_map.end())
    return log_map[id];
  throw NoSuchXLoggerAvailable(id);
}

// accept %%MSG%% , %%TIME%%, %%FANCYLVL%%, %%LOGLVL%%, %%FILE%% , %%FUNC%% , %%LINE%%
void XLogger::set_logging_template(BaseLoggerBackend* back, const string& tmpl) {
  back2tmpl[back] = tmpl;
}

// %%TIME%% is formated according to set_time_format() and strftime()
void XLogger::set_time_format(const string& format) {
  time_format = format;
}

// set action for loglvl == 10
void XLogger::set_loglvl_action(int loglvl, tLogActionPtr func) {
  lvl2action[loglvl] = func;
}

// set description to be used for loglvl 
void XLogger::set_loglvl_desc(int loglvl, const string& desc) {
  lvl2desc[loglvl] = desc;
}

// get loglvl description, decrease lvl gradually to 0, if not found.
string XLogger::get_fancy_level(int lvl) {
  while (lvl2desc.find(lvl) == lvl2desc.end())
    lvl--;
  return lvl2desc[lvl];
}


// render the message
string XLogger::render_msg(BaseLoggerBackend* back, const string& data, int loglevel, int line, const string& fn, const string& func) {
  XString msg(back2tmpl[back]);
  return msg.subs("%%MSG%%", data).\
         subs("%%TIME%%", XDateTime(time_format).format()).\
         subs("%%LOGLVL%%", TOOLS::str(loglevel)).\
         subs("%%FANCYLVL%%", get_fancy_level(loglevel)). \
         subs("%%FILE%%", fn).\
         subs("%%LINE%%", TOOLS::str(line)).\
         subs("%%FUNC%%", func);
}

// log message (including meta data)
void XLogger::log_msg(const string data, int loglevel, int line, const string fn, const string func) {
  // render and write to all backends
  for(BaseLoggerBackend* back : backends)
    back->write(render_msg(back, data, loglevel, line, fn, func));
  
  // check for action with given loglvl
  //if (error_action != NULL)
  //  error_action();
  if (lvl2action.find(loglevel) != lvl2action.end())
    lvl2action[loglevel]();
}

// log raw string
void XLogger::log_msg(const string data) {
  // just to be sure to avoid empty log msgs
  if(data.length() == 0)
    return;

  // write data to all backends
  for(BaseLoggerBackend* back : backends)
    back->write(data);
}


// LogStream provides the interface for logging
LogStream::LogStream(XLogger* logobj, int loglvl, int line, const std::string& fn, const std::string& func)
  : obj(logobj), loglvl(loglvl), line(line), fn(fn), func(func) { }

// Save the streamed data for the logger (during object destruction)
LogStream::~LogStream() {
  obj->log_msg(string(msg.str()), loglvl, line, string(fn), string(func));
}

// BaseLoggerBackend abstract class
BaseLoggerBackend::BaseLoggerBackend(const string& my_id, const string& my_name)
  : id(my_id), name(my_name) { }

// Hook for custom initialization 
void BaseLoggerBackend::init() { }

// Hook for custom destructor/cleanup
void BaseLoggerBackend::cleanup() { }


// FileBackend realization
FileBackend::FileBackend(const string& my_id, const string& fn)
  : filename(fn), BaseLoggerBackend(my_id, "file-append") { }

// writing to file: opening - appending message - closing 
void FileBackend::write(const string& msg) {
  ofstream fd(filename.c_str(), ios::app);
  fd.write(msg.c_str(), msg.size());
  fd.close();
}


// ConsoleBackend realization
ConsoleBackend::ConsoleBackend(const string& my_id) 
  : BaseLoggerBackend(my_id, "stdout") {}

// write to stdout through "cout"
void ConsoleBackend::write(const string& msg) {
  cout << msg;
}


// MemoryBackend
MemoryBackend::MemoryBackend(const string& my_id) : BaseLoggerBackend(my_id, "memory") {
  log_msgs.clear();
}

// save data to logger vector
void MemoryBackend::write(const string& msg) {
  log_msgs.push_back(msg);
}
