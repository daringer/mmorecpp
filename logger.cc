

#include "logger.h"

using namespace std;
using namespace TOOLS;


FileBackend::FileBackend(const string& fn) : filename(fn) {}

void FileBackend::write(const string& msg) {
  ofstream fd(filename.c_str(), ios::app);
  fd.write(msg.c_str(), msg.size());
  fd.close();
}

Logger::Logger(const string& id, BaseLoggerBackend* back) : backend(back) {
  log_map[id] = this;
}

Logger* Logger::get(const string& id) throw(NoSuchLoggerAvailable) {
  if(log_map.find(id) != log_map.end())
    return log_map[id];
  throw NoSuchLoggerAvailable(id);
}

// accept %%MSG%% , %%TIME%%, %%LOGLVL%%, %%FILE%% , %%FUNC%% , %%LINE%%
void Logger::set_logging_template(const string& tmpl) {
  log_template = tmpl;
}

void Logger::set_time_format(const string& format) {
  time_format = format;
}

void Logger::log_msg(string& data, int loglevel, string& line, const string& fn, const string& func) {
  XString msg(log_template);
  backend->write(msg.subs("%%MSG%%", data).\
                 subs("%%TIME%%", time_format).\
                 subs("%%LOGLVL%%", str(loglevel)).\
                 subs("%%FILE%%", fn).\
                 subs("%%LINE%%", str(line)).\
                 subs("%%FUNC%%", func));
}

LogStream::LogStream(Logger* logobj, int loglvl, const std::string& line, const std::string& fn, const std::string& func) :
  obj(logobj), loglvl(loglvl), line(line), fn(fn), func(func) { }

LogStream::~LogStream() {
  string msg = str();
  obj->log_msg(msg, loglvl, line, fn, func);
}

int main() {
  return 0;
}
