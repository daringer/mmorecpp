#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <signal.h>
#include <execinfo.h>
#include <cxxabi.h>
#include <unistd.h>

#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>

namespace TOOLS {
/* @brief The global (abstract) exception-base-class for all
 *        thrown exceptions
 */
class BaseException : public std::exception {
  public:
    std::string output;
    std::string exception_name;
    std::string message;

    template<class T>
    BaseException(const std::string& exc_name, const T& msg)
      : exception(), exception_name(exc_name) {
      std::stringstream ss;
      if(!(ss << msg)) {
        throw exception();
      }
      message = ss.str();
      init();
    }

    BaseException(const std::string& exc_name);
    BaseException(const BaseException& obj);

    virtual ~BaseException() throw();

    void set_message(const std::string& input);
    void show();
    void what(); // keep compatibility to c++ default
  private:
    void init();
};

// better exception handler - feed this to set_terminate
void tools_lib_exception_handler();

// better signal (SIGSEGV) handler, check tests for usage
void signal_handler(int sig);
}

// print a stacktrace
void print_stacktrace(uint max_frames = 63);



#define DEFINE_EXCEPTION(CLASS,PARENT) \
  class CLASS : public PARENT { \
    public: \
      CLASS() : PARENT(#CLASS) { } \
      template<class T> \
      CLASS(const T& msg) : PARENT(#CLASS, msg) { } \
      template<class T> \
      CLASS(const std::string& exc, const T& msg) : PARENT(exc, msg) { } \
      CLASS(const CLASS& obj) : PARENT(obj.exception_name, obj.message) { } \
  };

#endif
