#ifndef FS_H
#define FS_H

#include <stdarg.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#include <iostream>
#include <sstream>
#include <cassert>
#include <string>
#include <fstream>
#include <vector>

#include "exception.h"
#include "xstring.h"

namespace TOOLS {
namespace FS {

DEFINE_EXCEPTION(PathException, BaseException)

/**
 * @brief A handy Path representation and manipulation class.
 *        Supports "smart" path joining through addition:
 *        Path full = Path("/tmp") + Path("myfile")
 *        leads to the desired behavior
 */
class Path {
 public:
  std::string path;

  Path();
  Path(const std::string& path);
  Path(const Path& obj);
  ~Path();

  bool empty() const;
  size_t size() const;
  const char* c_str() const;

  bool exists();
  bool is_dir();
  bool is_file();

  Path parent();
  Path pwd();

  Path join(const std::string& what);

  bool create_dir();

  bool move(const std::string& to);

  std::vector<Path> listdir(bool recursive=false);

  Path operator+(const char*& add);
  Path operator+(const std::string& add);
  Path operator+(const Path& obj);
  bool operator!=(const Path& obj);
  bool operator==(const Path& obj);
  Path& operator=(const std::string& obj);

  //operator std::string();
};

bool change_dir(const std::string& path);
std::string current_dir();

Path operator+(const char* lhs, Path& rhs);
//Path operator+(const std::string& LHS, Path& RHS);

}
}
#endif
