#include "io.h"

using namespace std;
using namespace TOOLS::IO;

string TOOLS::IO::nextline(istream& inp) {
  char buffer[256] = {0};

  if(inp.eof())
    throw IOException("Unexpected end-of-file");

  inp.getline(buffer, 255);
  return buffer;

}

string TOOLS::IO::readfile(const string& filename) {
  ifstream t(filename.c_str());
  string tmp((istreambuf_iterator<char>(t)),
             istreambuf_iterator<char>());
  return tmp;
}
