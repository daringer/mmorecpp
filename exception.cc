#include "exception.h"

using namespace std;
using namespace TOOLS;


BaseException* BaseException::last_exception = NULL;

void TOOLS::tools_lib_exception_handler() {
  std::cout << endl << "[E] An uncaught exception occurred!" << std::endl;
  BaseException::last_exception->show();
  std::cout << "[i] exiting now..." << std::endl;
  std::abort();
}

/**
* @brief constructor global BaseException (without additional information)
* @param exc_name the name of the exception to be shown to the user
*/
BaseException::BaseException(const std::string& exc_name) 
  : exception(), exception_name(exc_name), message("") {
  set_message(message);
  BaseException::last_exception = this;
}
/**
 * @brief copy-constructor
 * @param the object to be copied
 */
BaseException::BaseException(const BaseException& obj) 
  : exception(), exception_name(obj.exception_name), message(obj.message) {
  set_message(message);
  BaseException::last_exception = this;  
}
/**
* @brief descructor including very important throw declaration
*/
BaseException::~BaseException() throw() { }
/**
* @brief show full message through stdout
*/
void BaseException::show() {
  cerr << endl << output << endl;
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

