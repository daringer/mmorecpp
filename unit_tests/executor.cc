#include "stdlib.h"
#include "string.h"
#include "unistd.h"

#include "executor.h"
#include "testing_suite.h"

using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;
using namespace std;

START_SUITE(ExecutorToolsTestSuite) {
      REG_TEST(system_like)
      REG_TEST(nonblocking_wait)
      REG_TEST(bash_and_output)
      REG_TEST(bash_again)
      REG_TEST(bash_third_time)
      REG_TEST(simple_output)
      REG_TEST(non_path_constructor)
      REG_TEST(constructor_and_defaults)
      REG_TEST(initialization)
    }

    string example_cmd;
    char* c_cmd;

    virtual void setup() {
      example_cmd = "cat /proc/cpuinfo";
      c_cmd = new char[CMD_LEN];
      bzero(c_cmd, sizeof(char) * CMD_LEN);
      strncpy(c_cmd, example_cmd.c_str(), CMD_LEN-1);
    }

    virtual void tear_down() {
      delete [] c_cmd;
    }

    MAKE_TEST(initialization) {
      Executor e("bash");
      CHECK_EXC(CommunicationNotInited, e.read_stdout());
    }

    MAKE_TEST(system_like) {
      Executor e("head -n 1 /proc/cpuinfo");
      e.run();
      CHECK(true);
    }

    MAKE_TEST(constructor_and_defaults) {
      Executor e(c_cmd);
      CHECK(e.result == -1 && e.pid == -1 && e.verbose == false);
    }

    MAKE_TEST(non_path_constructor) {
      CHECK_EXC(CommandNotFound, Executor("/foo/bar/ls", false));
    }

    MAKE_TEST(simple_output) {
      Executor e(c_cmd);
      e.communicate();
      string err = e.read_stderr();
      string out = e.read_stdout();
      CHECK(out.find("bogomips") != string::npos && out.find("cpu") != string::npos);
    }

    MAKE_TEST(bash_and_output) {
      Executor e("bash");
      e.communicate();
      e.write_stdin(example_cmd);
      e.write_stdin("exit");
      string out = e.read_stdout();

      int ret = e.check_for_exit();
      CHECK(ret == 0 && out.find("bogomips") != string::npos && out.find("cpu") != string::npos);
    }

    MAKE_TEST(bash_again) {
      test_bash_and_output();
    }

    MAKE_TEST(bash_third_time) {
      test_bash_and_output();
    }
    MAKE_TEST(nonblocking_wait) {
      Executor e("bash");
      e.communicate();

      int res;
      e.write_stdin(c_cmd);
      res = e.check_for_exit();
      CHECK(res == -1);

      e.write_stdin("exit");
      sleep(1);
      res = e.check_for_exit();
      CHECK(res >= 0);
    }

END_SUITE()
