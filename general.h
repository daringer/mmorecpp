#ifndef GENERAL_H
#define GENERAL_H

#include <vector>
#include <string>

namespace TOOLS {

const int MAX_LISTEN_QUEUE = 10;
const int BUF_LEN = 1024;
const int MAX_GROUP_COUNT = 10;

enum SOCKET_TYPE {
  UNIX,
  TCP,
  UDP
};

typedef std::vector<std::string> Stringlist;
typedef std::vector<int> Integerlist;

}
#endif
