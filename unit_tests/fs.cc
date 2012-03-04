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
      REG_TEST(&FSToolsTestSuite::test_path_init)
      REG_TEST(&FSToolsTestSuite::test_copy_constructor)
      REG_TEST(&FSToolsTestSuite::test_exists)
      REG_TEST(&FSToolsTestSuite::test_move)
      REG_TEST(&FSToolsTestSuite::test_create_dir)
      REG_TEST(&FSToolsTestSuite::test_isdir)
      REG_TEST(&FSToolsTestSuite::test_isnotdir)
      REG_TEST(&FSToolsTestSuite::test_isfile)
      REG_TEST(&FSToolsTestSuite::test_isnotfile)
      REG_TEST(&FSToolsTestSuite::test_exists)
      REG_TEST(&FSToolsTestSuite::test_notexists)
    }

    virtual void setup() {
      fn = "/tmp/test_test_foo";
      morefn = "/tmp/test_test_bar";
      realdir = "/tmp";
      realfn = "/tmp/foo_my_test_file_here";
      torealfn = "/tmp/foo_my_test_file_here";
      realfile = "/bin/bash";
    }

    virtual void tear_down() {
      system(("rm -rf " + fn + " " + morefn + " " + realfn + " " + torealfn).c_str());
    }

    MAKE_TEST(path_init) {
      FS::Path foo(fn);
      CHECK(foo.path == fn);
    }

    MAKE_TEST(copy_constructor) {
      FS::Path foo(fn);
      FS::Path foocopy(foo);

      foo.path = morefn;
      CHECK(foo.path != foocopy.path);
    }

    MAKE_TEST(exists) {
      FS::Path p(realdir);
      CHECK(p.exists());
    }

    MAKE_TEST(notexists) {
      FS::Path p(fn);
      CHECK(!p.exists());
    }

    MAKE_TEST(isfile) {
      FS::Path p(realfile);
      CHECK(p.is_file());
    }

    MAKE_TEST(isnotfile) {
      FS::Path p(realdir);
      CHECK(!p.is_file());
    }

    MAKE_TEST(isdir) {
      FS::Path p(realdir);
      CHECK(p.is_dir());
    }

    MAKE_TEST(isnotdir) {
      FS::Path p(realfile);
      CHECK(!p.is_dir());
    }

    MAKE_TEST(create_dir) {
      FS::Path p(realfn);
      p.create_dir();
      CHECK(p.exists());
      CHECK(FS::Path(realfn).exists());
    }

    MAKE_TEST(move) {
      FS::Path p(realfn);
      p.create_dir();
      p.move(torealfn);
      FS::Path p2(torealfn);
      CHECK(p2.exists());
    }
};
