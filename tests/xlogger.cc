#include <stdlib.h>
#include <unistd.h>

#define XLOG_MIN_LOG_LVL 3

#include "tools.h"

using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;
using namespace TOOLS::IO;
using namespace std;

#define LOGID "system"

#ifndef GENERATE_HEADER
void loglvl_action() { INFO << "Loglvl action successful!"; }
#endif

START_SUITE(XLoggerToolsTestSuite) {
  REG_TEST(simple)
  REG_TEST(loglvl_action)
  REG_TEST(endline)
  REG_TEST(persistent_file)
  REG_TEST(buffered_file)
}

XLogger* xlog;
MemoryBackend* mb;
std::string log_fn;

virtual void setup() {
  xlog = new XLogger(LOGID);
  // xlog->add_backend(new ConsoleBackend(LOGID));
  mb = new MemoryBackend(LOGID);
  xlog->set_min_loglvl(1);
  xlog->add_backend(mb);

  log_fn = "/tmp/logger_test_file.log";
}

virtual void tear_down() { 
  unlink(log_fn.c_str());
  if (xlog != nullptr)
    delete xlog; 
}

MAKE_TEST(simple) {
  CHECK(mb->log_msgs.size() == 1);
  int tmp = 123;
  DEBUG << "starting logger testing!";
  INFO << "more info logging, let's log an integer: " << tmp;
  WARN << "oh oh, this is a warning - this could end bad!";

  CHECK(mb->log_msgs.size() == 4);
}

MAKE_TEST(loglvl_action) {
  INFO << "STARTING";
  xlog->set_loglvl_action(10, loglvl_action);
  ERROR << "ERRRRORRRRR, should trigger action";
  CHECK(mb->log_msgs.size() == 4);
}

MAKE_TEST(endline) {
  INFO << "yiha - compiles -> ship it!" << std::endl;
  CHECK(true);
}

MAKE_TEST(persistent_file) {
  PersistentFileBackend* pfb = \
    new PersistentFileBackend(LOGID, log_fn);

  xlog->add_backend(pfb);

  // before logging, only header is there
  CHECK(mb->log_msgs.size() == 1);

  INFO << "writing some fancy log";
  ERROR << "some more, but now an error!";

  // after logging one additional line per log-msg
  CHECK(mb->log_msgs.size() == 3);

  delete xlog;
  xlog = nullptr;

  // destroying the logger adds another bye-bye-message to the log
  CHECK(mb->log_msgs.size() == 4);

  XString out = TOOLS::IO::readfile(log_fn);

  CHECK(out.length() > 0);
  CHECK(out.find("writing some fancy log") != string::npos);
  CHECK(out.find("some more, but now an error") != string::npos);
  CHECK(out.split("\n").size() == 5);
}

MAKE_TEST(buffered_file) {
  const int bufsize = 500;
  BufferedFileBackend* bfb = new BufferedFileBackend(LOGID, log_fn, bufsize);

  xlog->add_backend(bfb);

  // mem-logging should be unaffected!
  CHECK(mb->log_msgs.size() == 1);

  // write 2 entries, should not be written to file yet!
  XString log_out = "1234567890";
  for(size_t i=0; i<2; ++i)
    INFO << log_out;
  
  XString out = TOOLS::IO::readfile(log_fn);

  // still empty
  CHECK(out.length() == 0);

  // now adding a lot, in the meantime the file should have been flushed
  for(size_t i=0; i<20; ++i)
    INFO << log_out;

  // give it the FS some time to actually flush and write!
  sleep(1);

  out = TOOLS::IO::readfile(log_fn);

  // should contain at least the last full buffer, aka 'bufsize' chars
  size_t oldlen = out.length();
  CHECK(out.length() >= bufsize);

  delete xlog;
  xlog = nullptr;

  // after destruction the logdata should fully be written...
  out = TOOLS::IO::readfile(log_fn);
  CHECK(out.length() > oldlen);
}

END_SUITE()
