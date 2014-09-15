#include "xstring.h"

using namespace std;
using namespace TOOLS;

/**
 * @brief empty constructor
 */
XString::XString() : string() {}

XString::~XString() {}

/**
 * @brief strip all leading and trailing occurences of the given char(s)
 * @param s the char(s) to be stripped
 * @return *this
 */
XString& XString::strip(const string& s) {
  if (length() == 0)
    return *this;
  while (substr(0, s.length()) == s)
    erase(0, s.length());
  while (substr(length() - s.length(), s.length()) == s)
    erase(length() - s.length(), s.length());
  return *this;
}
/**
 * @brief split the string using given seperator char(s)
 * @return a vector<string> containing the results without the seperator char(s)
 */
tStringList XString::split(const string& s) {
  tStringList out;
  string::size_type pos = 0, last_pos = 0;
  while ((pos = find(s, last_pos)) != string::npos) {
    out.push_back(substr(last_pos, pos - last_pos));
    last_pos = pos + s.length();
  }
  out.push_back(substr(last_pos));
  return out;
}
/**
 * @brief substitute all occurences of the given string
 * @param what the searchstring to replace
 * @param with the string, which will be in place of what
 * @return *this
 */
XString& XString::subs_all(const string& what, const string& with) {
  return subs(what, with, -1);
}
/**
 * @brief substitute an arbitrary number of occurences of the given string
 * @param what the searchstring to replace
 * @param with the string, which will be in place of what
 * @param max_replaces replace as much as max_replaces occurences of what,
 *                     -1 means unlimited/all occurences
 * @return *this
 */
XString& XString::subs(const string& what, const string& with,
                       int max_replaces) {
  string::size_type pos = 0;
  int replaced = 0;
  while ((pos = find(what, pos)) != string::npos) {
    replace(pos, what.length(), with);
    if (++replaced == max_replaces)
      break;
    pos += with.length();
  }
  return *this;
}
/**
 * @brief join a given vector<string> together using the string as glue
 * @param the instance of the vector<string> to be joined
 * @return a string reference with the complete-joined string (*this)
 */
XString& XString::join(const tStringList& list) {
  XString delim(*this);
  clear();

  for (tStringList::const_iterator i = list.begin(); i != list.end(); ++i)
    append((*i) + ((i != --list.end()) ? delim : ""));
  return *this;
}
/**
 * @brief join a given vector<string> together using the string as glue
 * @param the instance of the vector<string> to be joined
 * @return a string reference with the complete-joined string (*this)
 */
XString& XString::join(const tStringSet& set) {
  XString delim(*this);
  clear();

  for (tStringSet::const_iterator i = set.begin(); i != set.end(); ++i)
    append((*i) + ((i != --set.end()) ? delim : ""));
  return *this;
}

/**
 * @brief convert all chars to lowercase
 * @return a string reference with the converted content
 */
XString& XString::lower() {
  for (string::iterator i = begin(); i != end(); ++i)
    if ((*i) >= 65 && (*i) <= 90)
      replace(i, i + 1, 1, (char)((*i) + 32));
  return *this;
}
/**
 * @brief convert all chars to uppercase
 * @return a string reference with the converted content
 */
XString& XString::upper() {
  for (string::iterator i = begin(); i != end(); ++i)
    if ((*i) >= 97 && (*i) <= 122)
      replace(i, i + 1, 1, (char)((*i) - 32));
  return *this;
}

bool XString::startswith(const string& what) { 
  return (find(what) == 0); 
}

bool XString::endswith(const string& what) {
  return (rfind(what) == (length() - what.length()));
}


/* replaced in favor of the new "check-for-real" function: */
/* maybe not unicode ready - but are there unicode-floating-point-numbers??? */
bool TOOLS::is_real(const string& s) {
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
