#ifndef GENERAL_H
#define GENERAL_H

#include <vector>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <exception>
#include <cstdlib>

namespace TOOLS {

const int MAX_LISTEN_QUEUE = 10;
const int BUF_LEN = 1024;
const int MAX_GROUP_COUNT = 10;

enum SOCKET_TYPE {
  UNIX,
  TCP,
  UDP
};

typedef std::vector<std::string> tStringList;
typedef tStringList::iterator tStringIter;

typedef std::vector<int> tIntList;
typedef tIntList::iterator tIntIter;

typedef std::map<std::string, std::string> tStringMap;
typedef tStringMap::iterator tStringMapIter;

typedef std::set<std::string> tStringSet;
}

#endif
