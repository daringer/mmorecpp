#include "converter.h"

using namespace std;
using namespace MM_NAMESPACE();

/* replaced in favor of the new "check-for-real" function: */
/* maybe not unicode ready - but are there unicode-floating-point-numbers??? */
bool MM_NAMESPACE()::is_real(const string& s) {
  // empty string is by-definition not a number!
  if(s.empty())
    return false;

  bool neg_found = false;
  bool dot_found = false;
  bool e_found = false;
  bool e_neg_found = false;
  bool legal = false;
  
  char* ptr = (char*) s.c_str();

  uint state = 0;
  while(*ptr != '\0') {

    // skip whitespace 
    while(*ptr == ' ')
      ptr++;

    // start digit ('-', '.', [0-9] ok)
    if(state == 0) {
      // '+' is _not_ allowed as first digit
      if(*ptr == '-' && !neg_found) {
        neg_found = true;
        state = 1;
        legal = false;
      // first '.' only if legal (left of it should be a digit)
      } else if(*ptr == '.' && !dot_found && legal) {
        dot_found = true;
        state = 1;
        legal = false;
      // exponent marker
      } else if((*ptr == 'e' || *ptr == 'E') && !e_found && legal) {
        e_found = true;
        state = 2;
        legal = false;
      // regular digit consumer
      } else if(*ptr >= 48 && *ptr <= 57) {
        state = 1;
        legal = true;
      } else {
        return false;
      }
    // number, i.e., digits and _one_ 'dot_char' or one of 'exp_chars'
    } else if(state == 1) {
      if(*ptr >= 48 && *ptr <= 57) {
        legal = true;
      } else if(*ptr == '.' && !dot_found) {
        dot_found = true;
        legal = true;
      } else {
        state = 0;
        ptr--;
      }
    // here after found 'e' or 'E', check for neg/pos exponent '-'/'+'
    } else if(state == 2) {
      state = 3;
      if((*ptr == '-' || *ptr == '+') && !e_neg_found) {
        e_neg_found = true;
        legal = false;
      } else if (*ptr >= 48 && *ptr <= 57) {
        legal = true;
      } else {
        return false;
      }
    // after 'E'/'e' and potential '-'/'+' was found, only digits are allowed now!
    } else if(state == 3) {
      if(*ptr >= 48 && *ptr <= 57) {
        legal = true;
      } else {
        legal = false;
        return false;
      }
    }
    ptr++;
  }
  return (*ptr == '\0') && legal;
}
