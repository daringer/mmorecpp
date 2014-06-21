#include "exception.h"

using namespace std;
using namespace TOOLS;

void TOOLS::tools_lib_exception_handler() {
  cerr << endl << "[EXC] An uncaught exception occurred!" << endl;
  //print_stacktrace();
  try {
    exception_ptr exc = current_exception();
    rethrow_exception(exc);
  
  } catch (BaseException& e) {
    cerr << "[EXC] TOOLS::BaseException:" << endl;
    cerr << "[EXC] " << e.what() << endl;
  } catch (std::exception& e) {
    cerr << "[EXC] Exception description following:" << endl;
    cerr << "[EXC] " << e.what() << endl;
  }

  cerr << "...exiting now..." << endl;
  exit(1);
}

void TOOLS::signal_handler(int sig) {
  cout << "DAAAAAAAAAAAAAAAMNNNNN SEGFAUUUUULT!!!!" << endl;
  print_stacktrace();
  exit(1);
}

/**
* @brief constructor global BaseException (without additional information)
* @param exc_name the name of the exception to be shown to the user
*/
BaseException::BaseException(const std::string& exc_name)
    : exception(), exception_name(exc_name), message("") {
  init();
}

/**
 * @brief copy-constructor
 * @param the object to be copied
 */
BaseException::BaseException(const BaseException& obj)
    : exception(), exception_name(obj.exception_name), message(obj.message) {
  init();
}

/**
* @brief descructor including very important throw declaration
*/
BaseException::~BaseException() throw() {}

/**
* @brief show full message through stderr
*/
const string BaseException::get_message() const { 
  return output; 
}

const char* BaseException::what() const noexcept {
  return output.c_str();
}

/**
* @brief show full message through stderr (C++ wrapper method)
*/
void BaseException::dump() const { cerr << output << endl; }

/**
 * @brief actual initialization
 */
void BaseException::init() { set_message(message); }

/**
* @brief explicitly set the message
* @param msg the message to be set
*/
void BaseException::set_message(const string& msg = "") {
  output = "[" + exception_name + "]";
  if (msg != "")
    output += " " + msg;
}

/**
 * @brief print a stacktrace for the active frame
 * @param max_frames maximum depth of the trace
 */

void print_stacktrace(uint max_frames) {
  cout << "[BT] Showing stacktrace: " << endl;
  //  for(string& s : get_stackdata(max_frames)) {
  //    cout << "[BT] " << s << endl;
  //  }
}

void get_stackdata(uint max_frames) {
  void* addrlist[max_frames + 1];
  int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

  if (addrlen == 0) {
    cerr << "[E] backtrace() returned 0 - Error!" << endl;
    return;
  }
  // resolve addresses to -> "filename(function+address)"
  // symlist must be free()-ed !
  char** symlist = backtrace_symbols(addrlist, addrlen);

  size_t funcnamesize = 256;
  char* funcname = (char*)malloc(funcnamesize);

  // demangle all functionnames
  for (int i = 1; i < addrlen; ++i) {
    char* begin_name = 0;
    char* begin_offset = 0;
    char* end_offset = 0;

    // find parentheses and +address offset surrounding the mangled name:
    // ./module(function+0x15c) [0x8048a6d]
    for (char* p = symlist[i]; *p; ++p) {
      if (*p == '(')
        begin_name = p;
      else if (*p == '+')
        begin_offset = p;
      else if (*p == ')' && begin_offset) {
        end_offset = p;
        break;
      }
    }

    string tmp;
    if (begin_name && begin_offset && end_offset && begin_name < begin_offset) {
      *begin_name++ = '\0';
      *begin_offset++ = '\0';
      *end_offset++ = '\0';

      // mangled name is now in [begin_name, begin_offset) and caller
      // offset in [begin_offset, end_offset). now apply
      // __cxa_demangle():

      int status;
      char* ret =
          abi::__cxa_demangle(begin_name, funcname, &funcnamesize, &status);
      cout << "status: " << status << endl;
      // use possibly realloc()-ed string
      if (status == 0) {
        funcname = ret;
        cout << funcname << endl;
        // tmp = string(funcname);
        // demangle failed
      } else {
        cout << symlist[i] << endl;
        // tmp = string(symlist[i]);
      }
      cout << " EVER HERE???" << endl;
      // parsing failed
    } else {
      cout << symlist[i] << endl;

      // tmp = string(symlist[i]);
    }

    // move into vector
    // cout << tmp << endl;
    // cout << out.size() << endl;
    // out.push_back(tmp);
  }
  free(funcname);
  free(symlist);
}