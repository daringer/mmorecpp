#pragma once

#ifndef MM_CUSTOM_NAMESPACE
#define MM_CUSTOM_NAMESPACE TOOLS
#endif

// macros for namespace name creation
//#define MM_NAMESPACE() MM_NAMESPACE()
#define MM_NAMESPACE(other) MM_CUSTOM_NAMESPACE ## other

#ifndef MM_NO_VECTOR 
#include <vector>
#endif 

#ifndef MM_NO_MAP
#include <map>
#endif 

#ifndef MM_NO_SET
#include <set>
#endif 

#ifndef MM_NO_SET
#include <string>
#endif 

#ifndef MM_NO_IOSTREAM
#include <iostream>
#endif 

#ifndef MM_NO_EXCEPTION
#include <exception>
#endif 


#include <cstdlib>


namespace MM_NAMESPACE() {

const int MAX_LISTEN_QUEUE = 10;
const int BUF_LEN = 1024;
const int MAX_GROUP_COUNT = 10;

enum SOCKET_TYPE {
  UNIX,
  TCP,
  UDP
};

typedef unsigned int uint;


#ifndef MM_NO_VECTOR
typedef std::vector<std::string> tStringList;
typedef tStringList::iterator tStringIter;

typedef std::vector<int> tIntList;
typedef tIntList::iterator tIntIter;

typedef std::vector<long> tLongList;
#endif 

#ifndef MM_NO_MAP
typedef std::map<std::string, std::string> tStringMap;
typedef tStringMap::iterator tStringMapIter;
#endif

#ifndef MM_NO_SET
typedef std::set<std::string> tStringSet;
#endif




}



