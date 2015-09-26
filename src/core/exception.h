#pragma once

#include <signal.h>
#include <execinfo.h>
#include <cxxabi.h>
#include <unistd.h>

#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <vector>

#include "general.h"

namespace MM_NAMESPACE() {
/** BaseException 
 * provides advanced exception handling, i.e., 
 * - derived exceptions should call BaseException("ExceptionName", msg)
 *   in their ctor
 * - by default provides a single string argument used for a description
 */
class BaseException : public std::exception {
 public:
  std::string output;
  std::string exception_name;
  std::string message;

  template <class T>
  BaseException(const std::string& exc_name, const T& msg)
      : exception(), exception_name(exc_name) {

    _ss.clear();
    if (!(_ss << msg)) {
      throw exception();
    }
    message = _ss.str();
    init();
  }

  BaseException(const std::string& exc_name);
  BaseException(const BaseException& obj);

  virtual ~BaseException() throw();

  // set message for exception
  void set_message(const std::string& input);

  // get_message
  const std::string get_message() const;

  // overload regular what()
  const char* what() const noexcept;

  // dump_message
  void dump() const;

 private:
  std::stringstream _ss;
  void init();
};

// better exception handler - feed this to set_terminate
void tools_lib_exception_handler();

// better signal (SIGSEGV) handler, check tests for usage
void signal_handler(int sig);
}

// print a stacktrace
void print_stacktrace(uint max_frames = 63);
// void get_stackdata(uint max_frames = 63);

/**
 * Macro to easily define a exception.
 * usage:
 *
 * DEFINE_EXCEPTION(MyNewException, MM_NAMESPACE()::BaseException);
 * this defines a new exception, which allows one or no
 * argument for construction
**/
#define DEFINE_EXCEPTION(CLASS, PARENT)                                  \
  class CLASS : public PARENT {                                          \
   public:                                                               \
    CLASS() : PARENT(#CLASS) {}                                          \
    template <class T>                                                   \
    CLASS(const T& msg)                                                  \
        : PARENT(#CLASS, msg) {}                                         \
    template <class T>                                                   \
    CLASS(const std::string& exc, const T& msg)                          \
        : PARENT(exc, msg) {}                                            \
    CLASS(const CLASS& obj) : PARENT(obj.exception_name, obj.message) {} \
  };
