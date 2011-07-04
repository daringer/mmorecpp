#include "xstring.h"

using namespace std;
using namespace TOOLS;

/**
 * @brief empty constructor
 */
XString::XString() : string() { }
/**
 * @brief strip all leading and trailing whitespaces
 * @return *this
 */
XString& XString::strip() {
  return strip(" ");
}
/**
 * @brief strip all leading and trailing occurences of the given char(s)
 * @param s the char(s) to be stripped
 * @return *this
 */
XString& XString::strip(const string& s) {
  if (length() == 0)
    return *this;
  while(substr(0, s.length()) == s)
    erase(0, s.length());
  while(substr(length() - s.length(), s.length()) == s)
    erase(length() - s.length(), s.length());
  return *this;
}
/**
 * @brief split the string using a whitespace as seperator
 * @return a vector<string> containing the results without any whitespaces
 */
vector<string> XString::split() {
  return split(" ");
}
/**
 * @brief split the string using given seperator char(s)
 * @return a vector<string> containing the results without the seperator char(s)
 */
tStringList XString::split(const string& s) {
  tStringList out;
  string::size_type pos = 0, last_pos = 0;
  while((pos = find(s, last_pos)) != string::npos) {
    out.push_back(substr(last_pos, pos-last_pos));
    last_pos = pos + s.length();
  }
  out.push_back(substr(last_pos));
  return out;
}
/**
 * @brief substitute one occurence of the given string
 * @param what the searchstring to replace
 * @param with the string, which will be in place of what
 * @return *this
 */
XString& XString::subs(const string& what, const string& with) {
  return subs(what, with, 1);
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
XString& XString::subs(const string& what, const string& with, int max_replaces) {
  string::size_type pos = 0;
  int replaced = 0;
  //string::size_type last_repl_len = 0;
  //string placeholder = "!\"§$%&/()=";
  while((pos = find(what, pos)) != string::npos) {
    replace(pos, what.length(), with);
    if(++replaced == max_replaces)
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
XString& XString::join(const tStringList& pieces) {
  XString delim(*this);
  clear();

  for(tStringList::const_iterator i=pieces.begin(); i!=pieces.end(); ++i)
    append((*i) + ((i != --pieces.end()) ? delim : ""));
  return *this;
}
/**
 * @brief convert all chars to lowercase
 * @return a string reference with the converted content
 */
XString& XString::lower() {
  for(string::iterator i=begin(); i!=end(); ++i)
    if((*i) >= 65 && (*i) <= 90)
      replace(i, i+1, 1, (char)((*i)+32));
  return *this;
}
/**
 * @brief convert all chars to uppercase
 * @return a string reference with the converted content
 */
XString& XString::upper() {
  for(string::iterator i=begin(); i!=end(); ++i)
    if((*i) >= 97 && (*i) <= 122)
      replace(i, i+1, 1, (char)((*i)-32));
  return *this;
}

bool XString::startswith(const string& what) {
  return (find(what) == 0);
}

bool XString::endswith(const string& what) {
  return (rfind(what) == (length() - what.length()));
}



