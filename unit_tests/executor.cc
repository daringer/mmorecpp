#include "stdlib.h"
#include "string.h"

#include "executor.h"
#include "testing_suite.h"

using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;
using namespace std;

class ExecutorToolsTestSuite : public TestSuite {
  public:
    string example_cmd;
    char* c_cmd;

    ExecutorToolsTestSuite() {
      REG_TEST(&ExecutorToolsTestSuite::test_system_like)
      REG_TEST(&ExecutorToolsTestSuite::test_nonblocking_wait)
      REG_TEST(&ExecutorToolsTestSuite::test_bash_and_output)
      REG_TEST(&ExecutorToolsTestSuite::test_bash_again)
      REG_TEST(&ExecutorToolsTestSuite::test_bash_third_time)
      REG_TEST(&ExecutorToolsTestSuite::test_simple_output)
      REG_TEST(&ExecutorToolsTestSuite::test_non_path_constructor)
      REG_TEST(&ExecutorToolsTestSuite::test_constructor_and_defaults)
      REG_TEST(&ExecutorToolsTestSuite::test_initialization)
    }

    virtual void setup() {
      example_cmd = "cat /proc/cpuinfo";
      c_cmd = new char[CMD_LEN];
      bzero(c_cmd, sizeof(char) * CMD_LEN);
      strncpy(c_cmd, example_cmd.c_str(), CMD_LEN-1);
    }

    virtual void tear_down() {
      delete [] c_cmd;
    }

    void test_initialization() {
      Executor e("bash");
      CHECK_EXC(CommunicationNotInited, e.read_stdout());
    }

    void test_system_like() {
      Executor e("head -n 1 /proc/cpuinfo");
      e.run();
      CHECK(true);
    }

    void test_constructor_and_defaults() {
      Executor e(c_cmd);
      CHECK(e.result == -1 && e.pid == -1 && e.verbose == false);
    }

    void test_non_path_constructor() {
      CHECK_EXC(CommandNotFound, Executor("/foo/bar/ls", false));
    }

    void test_simple_output() {
      //Executor e(("ls " + dirname).c_str(), true);
      Executor e(c_cmd);
      //e.verbose = true;
      e.communicate();
      string err = e.read_stderr();
      string out = e.read_stdout();
      /*cout << "out:" << endl;
      cout << out << endl;
      cout << err << endl;
      cout << "end out..." << endl;
      cerr << "WAAS HIER LOOOS????" << endl;
      cout << "EXITCODE: " << strerror(e.check_for_exit(true)) << endl;*/
      CHECK(out.find("bogomips") != string::npos && out.find("cpu") != string::npos);
    }

    void test_bash_and_output() {
      Executor e("bash");
      e.communicate();
      e.write_stdin(example_cmd);
      //perror("WHOOOT: ");
      //e.write_stdin("exit");
      e.write_stdin("exit");
      string out = e.read_stdout();
      //cout << "EXITCODE: " << strerror(e.check_for_exit(true)) << endl;
      
      //perror("WHOOOT: ");
      //cout << out << endl;
      int ret = e.check_for_exit();
      CHECK(ret == 0 && out.find("bogomips") != string::npos && out.find("cpu") != string::npos);
    }

    void test_bash_again() {
      test_bash_and_output();
    }

    void test_bash_third_time() {
      test_bash_and_output();
    }
    void test_nonblocking_wait() {
      Executor e("bash");
      e.communicate();
      
      int res;
      e.write_stdin(c_cmd);
      res = e.check_for_exit();
      //cout << "RES NONBLOCKING wait??: " << strerror(res) << endl;
      CHECK(res == -1);
      
      e.write_stdin("exit");
      sleep(1);
      res = e.check_for_exit();
      CHECK(res >= 0);
    }
};
