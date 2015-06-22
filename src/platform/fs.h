#pragma once

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

#include "../core/exception.h"
#include "../core/xstring.h"

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
  typedef enum {
    RIGHTS_UNSET = 0,
    RIGHTS_OTHER = 1,
    RIGHTS_GROUP = 2,
    RIGHTS_USER = 4
  } RIGHTS_PERSON;

  typedef enum {
    MODE_UNSET = 0,
    MODE_EXEC = 1,
    MODE_READ = 2,
    MODE_WRITE = 4
  } RIGHTS_MODE;

  std::string path;

  Path();
  Path(const std::string& path);
  Path(const Path& obj);
  ~Path();

  // forwarders to underlying string
  bool empty() const;
  size_t size() const;
  const char* c_str() const;

  // path status, type
  bool exists();
  bool is_dir();
  bool is_file();
  //bool is_symlink();
  //bool is_socket();

  //int get_uid();
  //string get_user();
  //int get_gid();
  //string get_group();

  //RIGHTS_MODE get_mode(const RIGHTS_PERSON& who);

  //bool is_readable();
  //bool is_writeable();
  //bool is_executable();
  
  //bool touch();

  //tTime get_access_time();
  //tTime get_atime();
  //tTime get_modified_time();
  //tTime get_mtime();

  // dirname
  Path parent();
  //Path dirname(); -- make alias

  // basename
  //Path basename(); -- make alias
  
  Path join(const std::string& what);

  // bool TODO: recursive missing
  bool create_dir();
  //bool mkdir(bool recursive); -- make alias

  bool move(const std::string& to);
  
  //bool remove(bool recursive, bool inc_dirs, bool force);

  //bool copy(bool recursive, Path target);

  // func(const std::string& dir, const std::string& fn)
  //void walk(lambda func);

  std::vector<Path> listdir(bool recursive=false);

  Path operator+(const char*& add);
  Path operator+(const std::string& add);
  Path operator+(const Path& obj);
  bool operator!=(const Path& obj);
  bool operator==(const Path& obj);
  Path& operator=(const std::string& obj);

};

// add Path overloaded
bool change_dir(const std::string& path);
//bool change_dir(const FS::Path& path);

std::string current_dir();

// create 


Path operator+(const char* lhs, Path& rhs);
//Path operator+(const std::string& LHS, Path& RHS);

}
}
