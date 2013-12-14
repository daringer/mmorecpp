#include "exception.h"

using namespace std;
using namespace TOOLS;

void TOOLS::tools_lib_exception_handler() {
  std::cout << endl << "[E] An uncaught exception occurred!" << std::endl;
  print_stacktrace();
  try {
    std::exception_ptr exc = std::current_exception();
    std::rethrow_exception(exc);
  } catch(std::exception& e) {
    cout << "[E] Exception description following:" << endl;
    cout << "[E] " << e.what() << endl;
  }

  cout << "[i] exiting now..." << std::endl;
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
BaseException::~BaseException() throw() { }
/**
* @brief show full message through stderr
*/
void BaseException::show() {
  cerr << output << endl;
}
/**
* @brief show full message through stderr (C++ wrapper method)
*/
void BaseException::what() {
  show();
}
/**
 * @brief actual initialization
 */
void BaseException::init() {
  set_message(message);
}
/**
* @brief explicitly set the message
* @param msg the message to be set
*/
void BaseException::set_message(const string& msg="") {
  output = "[" + exception_name + "]";
  if(msg != "")
    output += " " + msg;
}

/**
 * @brief print a stacktrace for the active frame
 * @param max_frames maximum depth of the trace
 */

void print_stacktrace(uint max_frames) {
  cerr << "[BT] Showing stacktrace: " << endl;

  void* addrlist[max_frames+1];
  int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

  if(addrlen == 0) {
    cerr << "[E] backtrace() returned 0 - Error!" << endl;
    return;
  }
  // resolve addresses to -> "filename(function+address)"
  // symlist must be free()-ed !
  char** symlist = backtrace_symbols(addrlist, addrlen);

  size_t funcnamesize = 256;
  char* funcname = (char*)malloc(funcnamesize);

  // demangle all functionnames
  for(int i=1; i<addrlen; ++i) {
    char* begin_name = 0;
    char* begin_offset = 0;
    char* end_offset = 0;

    // find parentheses and +address offset surrounding the mangled name:
    // ./module(function+0x15c) [0x8048a6d]
    for(char* p = symlist[i]; *p; ++p) {
      if(*p == '(')
        begin_name = p;
      else if(*p == '+')
        begin_offset = p;
      else if(*p == ')' && begin_offset) {
        end_offset = p;
        break;
      }
    }

    if(begin_name && begin_offset && end_offset
        && begin_name < begin_offset) {
      *begin_name++ = '\0';
      *begin_offset++ = '\0';
      *end_offset = '\0';

      // mangled name is now in [begin_name, begin_offset) and caller
      // offset in [begin_offset, end_offset). now apply
      // __cxa_demangle():

      int status;
      char* ret = abi::__cxa_demangle(begin_name,
                                      funcname, &funcnamesize, &status);
      if(status == 0) {
        funcname = ret; // use possibly realloc()-ed string
        cerr << "[BT] " << symlist[i] << " " \
             << ret << "+" << begin_offset << endl;
      } else { // demangle failes
        cerr << "[BT] " << symlist[i] << " " \
             << begin_name << "+" << begin_offset << endl;
      }
    } else // parsing failed
      cerr << "[BT]" << symlist[i] << endl;
  }
  free(funcname);
  free(symlist);
}

