#include "fs.h"

using namespace std;
using namespace TOOLS::FS;

/**
 * @brief Path constructor from a std::string
 * @param path the string-representation of the path
 */
Path::Path(const string& path) : path(path) {}
/**
 * @brief Path empty constructor
 */
Path::Path() : path("") {}
/**
 * @brief Path copy constructor
 * @param obj an instance of the class Path
 */
Path::Path(const Path& obj) : path(obj.path) {}
/**
 * @brief Path desctructor
 */
Path::~Path() {}
/**
 * @brief collect all filenames in a given directory
 * @return vector<Path> a vector containing the new order of the circuits
 * (maybe)
 */
vector<Path> Path::listdir() { return listdir(false); }
/**
 * @brief (recursivly) collect all filenames in a given directory
 * @param recursive recursivly descend through the filesystem
 * @return vector<Path> a vector containing the new order of the circuits
 */
vector<Path> Path::listdir(bool recursive) {
  DIR* dir;
  struct dirent* entry;
  vector<Path> out;

  if (!exists())
    throw PathException("Directory does not exist: " + path);
  if ((dir = opendir(path.c_str())) == NULL)
    throw PathException("Could not open directory: " + path);

  while ((entry = readdir(dir)) != NULL) {
    if (string(entry->d_name) == "." || string(entry->d_name) == "..")
      continue;
    Path filepath = *this + entry->d_name;
    out.push_back(Path(entry->d_name));
    if (recursive && filepath.is_dir()) {
      vector<Path> contents = filepath.listdir(recursive);
      for (vector<Path>::iterator i = contents.begin(); i != contents.end();
           ++i)
        out.push_back(Path(entry->d_name) + *i);
    }
  }
  free(dir);
  return out;
}

Path Path::operator+(const string& add) {
  return (path.substr(path.size() - 1) == "/") ? Path(path + add)
                                               : Path(path + "/" + add);
}
Path Path::operator+(const Path& obj) {
  return (path.substr(path.size() - 1) == "/") ? Path(path + obj.path)
                                               : Path(path + "/" + obj.path);
}

Path::operator string() { return path; }

bool Path::operator==(const Path& obj) { return path == obj.path; }
/**
 * @brief Does this Path instance represent a directory?
 * @return bool true or false
 */
bool Path::is_dir() {
  struct stat s;
  if (stat(path.c_str(), &s) != 0)
    return false;
  return S_ISDIR(s.st_mode);
}
/**
 * @brief Does this Path instance represent an file?
 * @return bool true or false
 */
bool Path::is_file() {
  struct stat s;
  if (stat(path.c_str(), &s) != 0)
    return false;
  return S_ISREG(s.st_mode);
}
/**
 * @brief Does this Path instance represent an directory?
 * @return bool true or false
 */
Path Path::parent() {
  string::size_type pos = path.rfind("/", path.size() - 1);
  if (pos == string::npos)
    return Path("./");
  return Path(path.substr(0, pos));
}

/**
 * @brief check if a given path exists on the filesystem
 * @param path the path to be checked as string relative or absolute
 * @return true if path exists, else false
 */
bool Path::exists() {
  struct stat s;
  return (stat(path.c_str(), &s) == 0);
}
/**
 * @brief create the given directory
 * @param the path of the directory to be created
 * @return true on success
 */
bool Path::create_dir() { return (::mkdir(path.c_str(), 0777) == 0); }
/**
 * @brief move a given path to another path
 * @param from the path that should be moved
 * @param to the target path of the movement
 * @return true on success
 */
bool Path::move(const string& to) {
  bool res = (rename(path.c_str(), to.c_str()) == 0);
  if (res)
    path = to;
  return res;
}
/**
 * @brief change the current workdir to the given path
 * @param the path the workdir should be changed to
 * @return true on success
 */
bool TOOLS::FS::change_dir(const string& path) {
  return (chdir(path.c_str()) == 0);
}

string TOOLS::FS::current_dir() {
  char buf[1024];
  char* dir = getcwd((char*)&buf, 1024);
  string out;
  out += dir;
  return out;
}
