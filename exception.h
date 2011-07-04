#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
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

    static BaseException* last_exception;

    BaseException(const std::string& msg, const std::string& exc_name);
    BaseException(const BaseException& obj);
    BaseException();

    virtual ~BaseException() throw ();

    void set_message(const std::string& input);
    void show();
};

// better exception handler - feed this to set_terminate and 
// derive your exceptions from BaseException
void tools_lib_exception_handler();

}

#define DEFINE_EXCEPTION(CLASS,PARENT) \
  class CLASS : public PARENT { \
    public: \
      CLASS(const std::string& msg) : PARENT(msg, #CLASS) { } \
  };

#define DEFINE_PARENT_EXCEPTION(CLASS,PARENT) \
  class CLASS : public PARENT { \
    public: \
      CLASS(const std::string& msg, const std::string& exc_name) : PARENT(msg, exc_name) { } \
      CLASS(const CLASS& obj) : PARENT(obj) { } \
  };

#endif




