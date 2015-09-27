#include <time.h>

#include "xlogger.h"
#include "../core/xtime.h"
#include "../core/xstring.h"
#include "../core/converter.h"
#include "../ooapi/testing_suite.h"

using namespace std;
using namespace MM_NAMESPACE();

tXLoggerMap XLogger::log_map;

// Logger main class
// Make sure you place a instance of this logger inside the heap! No STACK use!
XLogger::XLogger(const string& id)
    : id(id), time_format("%d.%m - %T"), min_loglvl(0), strip_msg(true),
      to_strip({"\n", "\t", " ", "\r"}) {

  if (log_map.find(id) != log_map.end())
    throw LoggerIDAlreadyRegistered(id);

  backends.clear();
  log_map[id] = this;
  set_loglvl_desc(0, "?");
}

// cleaning up / deleting backends
XLogger::~XLogger() {
  log_msg("[XLogger] Exiting logger: " + id + "\n");
  for (tBackendIter i = backends.begin(); i != backends.end(); ++i) {
    (*i)->cleanup();
    delete *i;
  }
  log_map.erase(id);
}

void XLogger::quick_init(const std::string& newlogid,
                         const std::string& console_format, const int& minloglvl,
                         bool strip) {

  XLogger* xlog = new XLogger(newlogid);
  ConsoleBackend* xlog_console = new ConsoleBackend(newlogid);
  xlog->add_backend(xlog_console, console_format);

  xlog->set_loglvl_desc(10, "ERR");
  xlog->set_loglvl_desc(7, "W");
  xlog->set_loglvl_desc(5, "i");
  xlog->set_loglvl_desc(3, "DEBUG");
  xlog->set_loglvl_desc(1, "PROFILE");

  xlog->set_min_loglvl(minloglvl);
  xlog->set_msg_stripping(strip);
}

void XLogger::add_filename_filter(const string& fn) {
  fn_filter.insert(fn);
}

void XLogger::remove_filename_filter(const string& fn) {
  fn_filter.erase(fn_filter.find(fn));
}

void XLogger::set_msg_stripping(const bool& yesno) {
  strip_msg = yesno;
}

void XLogger::set_msg_stripping(const bool& yesno, const tStringList& trims) {
  set_msg_stripping(yesno);
  to_strip = trims;
}

// adding a backend
void XLogger::add_backend(BaseLoggerBackend* back, const string& tmpl) {
  if (tmpl == "")
    back2tmpl[back] =
          "[%%FANCYLVL%%] %%TIME%% (%%FILE%%:%%LINE%% call: %%FUNC%%) || "
          "%%MSG%%\n";
  else
    back2tmpl[back] = tmpl;

  backends.push_back(back);
  back->init();
  back->write("[XLogger] Initializing backend (" + back->name +
              ") for logger: " + id + "\n");
}

// static global getter for the registered XLoggers
XLogger* XLogger::get(const string& id) throw(NoSuchXLoggerAvailable) {
  if (log_map.find(id) != log_map.end())
    return log_map[id];
  throw NoSuchXLoggerAvailable(id);
}

// static to check, if loglevel is high enough to log the msg
bool XLogger::check_loglevel(const int& lvl,
                             const string& id) throw(NoSuchXLoggerAvailable) {
  return (XLogger::get(id)->min_loglvl <= lvl);
}

// accept %%MSG%% , %%TIME%%, %%FANCYLVL%%, %%LOGLVL%%, %%FILE%% , %%FUNC%% ,
// %%LINE%%
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

void XLogger::set_min_loglvl(int loglvl) {
  XLogger::min_loglvl = loglvl;
}

// render the message
string XLogger::render_msg(BaseLoggerBackend* back, const string& data,
                           int loglevel, int line, const string& fn,
                           const string& func) {
  XString msg(back2tmpl[back]);

  XString mymsg = data;
  if (strip_msg) {
    for (const string& s : to_strip)
      mymsg.strip(s);
  }

  return msg.subs("%%MSG%%", mymsg)
        .subs("%%TIME%%", XDateTime(time_format).format())
        .subs("%%LOGLVL%%", MM_NAMESPACE()::str(loglevel))
        .subs("%%FANCYLVL%%", get_fancy_level(loglevel))
        .subs("%%FILE%%", fn)
        .subs("%%LINE%%", MM_NAMESPACE()::str(line))
        .subs("%%FUNC%%", func);
}

// log message (including meta data)
void XLogger::log_msg(const string data, int loglevel, int line, const string fn,
                      const string func) {

  // if not fn_filter.empty() show only msg from given files
  if (!fn_filter.empty() && fn_filter.find(fn) == fn_filter.end())
    return;

  // if loglvl below min_loglvl, discard!
  if (loglevel < min_loglvl)
    return;

  // render and write to all backends
  for (BaseLoggerBackend* back : backends)
    back->write(render_msg(back, data, loglevel, line, fn, func));

  // check for action with given loglvl
  if (lvl2action.find(loglevel) != lvl2action.end())
    lvl2action[loglevel]();
}

// log raw string
void XLogger::log_msg(const string data) {
  // just to be sure to avoid empty log msgs
  if (data.length() == 0)
    return;

  // write data to all backends
  for (BaseLoggerBackend* back : backends)
    back->write(data);
}

// LogStream provides the interface for logging
LogStream::LogStream(XLogger* logobj, int loglvl, int line,
                     const std::string& fn, const std::string& func)
    : obj(logobj), loglvl(loglvl), line(line), fn(fn), func(func) {}

// Save the streamed data for the logger (during object destruction)
LogStream::~LogStream() {
  obj->log_msg(string(msg.str()), loglvl, line, string(fn), string(func));
}

LogStream& LogStream::operator<<(tEndl fnc) {
  // just append \n
  msg << "\n";
  return *this;
}

// BaseLoggerBackend abstract class
BaseLoggerBackend::BaseLoggerBackend(const string& my_id, const string& my_name)
    : id(my_id), name(my_name) {
  init();
}

BaseLoggerBackend::~BaseLoggerBackend() {
  cleanup();
}

// Hook for custom initialization
void BaseLoggerBackend::init() {}

// Hook for custom destructor/cleanup
void BaseLoggerBackend::cleanup() {}

// FileBackend realization
FileBackend::FileBackend(const string& my_id, const string& fn,
                         const string& my_name)
    : BaseLoggerBackend(my_id, my_name), filename(fn) {}

FileBackend::~FileBackend() {}

// writing to file: opening - appending message - closing
void FileBackend::write(const string& msg) {
  ofstream fd(filename.c_str(), ios::app);
  write_to_fstream(fd, msg);
  fd.close();
}

// internal, write 'msg' to 'fd'
void FileBackend::write_to_fstream(ofstream& fd, const string& msg) {
  fd.write(msg.c_str(), msg.size());
}

// PersistentFileBackend keeps the fileobject open
PersistentFileBackend::PersistentFileBackend(const string& my_id,
                                             const string& fn,
                                             const string& my_name)
    : FileBackend(my_id, fn, my_name) {}

PersistentFileBackend::~PersistentFileBackend() {}

// write directly to file
void PersistentFileBackend::write(const string& msg) {
  write_to_fstream(fd, msg);
  fd.flush();
}

// open file-descriptor wrapper
void PersistentFileBackend::init() {
  fd.open(filename.c_str(), ios::app);
}

// close file-descriptor wrapper
void PersistentFileBackend::cleanup() {
  fd.close();
}

// BufferedFileBackend
BufferedFileBackend::BufferedFileBackend(const string& my_id, const string& fn,
                                         const int& buf_size,
                                         const string& my_name)
    : PersistentFileBackend(my_id, fn, my_name), _buffer_size(buf_size) {}

BufferedFileBackend::~BufferedFileBackend() {}

// first copy data to buffer, once it overflows, flush into file!
void BufferedFileBackend::write(const string& msg) {
  if (_buf.size() + msg.size() > _buffer_size)
    flush();

  _buf.append(msg);
}

// flush buffer contents and write them to file
void BufferedFileBackend::flush() {
  write_to_fstream(fd, _buf);
  fd.flush();
  _buf.clear();
}

void BufferedFileBackend::cleanup() {
  flush();
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
MemoryBackend::MemoryBackend(const string& my_id)
    : BaseLoggerBackend(my_id, "memory") {
  log_msgs.clear();
}

// save data to logger vector
void MemoryBackend::write(const string& msg) {
  log_msgs.push_back(msg);
}
