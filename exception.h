#ifndef EXCEPTION_H
#define EXCEPTION_H

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

    static BaseException* last_exception;

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
  private:
    void init();
};

// better exception handler - feed this to set_terminate and
// derive your exceptions from BaseException
void tools_lib_exception_handler();

}

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
