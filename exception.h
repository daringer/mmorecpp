#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
#include <iostream>

namespace TOOLS {
/**
 * @brief The global (abstract) exception-base-class for all
 *        thrown exceptions
 */
class BaseException {
  protected:
    std::string exception_name;
    std::string message;

  public:
    std::string output;

    BaseException(const std::string& msg, const std::string& exc_name);
    BaseException(const BaseException& obj);

    void set_message(const std::string& input);
    void show();
};

}
#endif
