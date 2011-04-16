#include "stdlib.h"

#include "fs.h"

#include "testing_suite.h"


using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;
using namespace std;

class FSToolsTestSuite : public TestSuite {
  public:
    string fn, morefn, realdir, realfn, torealfn, realfile;

    FSToolsTestSuite() {
      add_test("Constructor", &FSToolsTestSuite::test_path_init);
      add_test("Copy constructor", &FSToolsTestSuite::test_copy_constructor);
      add_test("Path existance", &FSToolsTestSuite::test_exists);
      add_test("Move operation", &FSToolsTestSuite::test_move);
      add_test("Create directory", &FSToolsTestSuite::test_create_dir);
      add_test(".is_dir()", &FSToolsTestSuite::test_isdir);
      add_test("!.is_dir()", &FSToolsTestSuite::test_isnotdir);
      add_test(".is_file()", &FSToolsTestSuite::test_isfile);
      add_test("!.is_file()", &FSToolsTestSuite::test_isnotfile);
      add_test(".exists()", &FSToolsTestSuite::test_exists);
      add_test("!.exists()", &FSToolsTestSuite::test_notexists);
    };

    virtual void setup() {
      fn = "/tmp/test_test_foo";
      morefn = "/tmp/test_test_bar";
      realdir = "/tmp";
      realfn = "/tmp/foo_my_test_file_here";
      torealfn = "/tmp/foo_my_test_file_here";
      realfile = "/bin/bash";
    };

    virtual void tear_down() {
      system(("rm -rf " + fn + " " + morefn + " " + realfn + " " + torealfn).c_str());
    };

    void test_path_init() {
      FS::Path foo(fn);
      CHECK(foo.path == fn);
    };

    void test_copy_constructor() {
      FS::Path foo(fn);
      FS::Path foocopy(foo);

      foo.path = morefn;
      CHECK(foo.path != foocopy.path);
    };

    void test_exists() {
      FS::Path p(realdir);
      CHECK(p.exists());
    };

    void test_notexists() {
      FS::Path p(fn);
      CHECK(!p.exists());
    };

    void test_isfile() {
      FS::Path p(realfile);
      CHECK(p.is_file());
    };

    void test_isnotfile() {
      FS::Path p(realdir);
      CHECK(!p.is_file());
    };

    void test_isdir() {
      FS::Path p(realdir);
      CHECK(p.is_dir());
    };

    void test_isnotdir() {
      FS::Path p(realfile);
      CHECK(!p.is_dir());
    };

    void test_create_dir() {
      FS::Path p(realfn);
      p.create_dir();
      CHECK(p.exists());
      CHECK(FS::Path(realfn).exists());
    };

    void test_move() {
      FS::Path p(realfn);
      p.create_dir();
      p.move(torealfn);
      FS::Path p2(torealfn);
      CHECK(p2.exists());
    };
};
