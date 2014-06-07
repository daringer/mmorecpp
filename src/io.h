#ifndef IO_H
#define IO_H

#include <stdarg.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#include <iostream>
#include <sstream>
#include <cassert>
#include <string>
#include <fstream>
#include <vector>
#include <streambuf>

#include "general.h"
#include "exception.h"

namespace TOOLS {
namespace IO {

DEFINE_EXCEPTION(IOException, BaseException)

std::string nextline(std::istream& inp);
std::string readfile(const std::string& filename);
}
}
#endif
