#include <stdlib.h>
#include <unistd.h>

#include "tools.h"

using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;
using namespace std;

START_SUITE(FSToolsTestSuite) {
  REG_TEST(path_init)
  REG_TEST(copy_constructor)
  REG_TEST(exists)
  REG_TEST(move)
  REG_TEST(create_dir)
  REG_TEST(isdir)
  REG_TEST(isnotdir)
  REG_TEST(isfile)
  REG_TEST(isnotfile)
  REG_TEST(exists)
  REG_TEST(notexists)
}

string fn, morefn, realdir, realfn, torealfn, realfile;

virtual void setup() {
  fn = "/tmp/test_test_foo";
  morefn = "/tmp/test_test_bar";
  realdir = "/tmp";
  realfn = "/tmp/foo_my_test_file_here";
  torealfn = "/tmp/foo_my_test_file_here";
  realfile = "/bin/bash";
}

virtual void tear_down() {
  system(
      ("rm -rf " + fn + " " + morefn + " " + realfn + " " + torealfn).c_str());
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

END_SUITE()
