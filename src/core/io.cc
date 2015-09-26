#include "io.h"

using namespace std;
using namespace MM_NAMESPACE()::IO;

string MM_NAMESPACE()::IO::nextline(istream& inp) {
  char buffer[256] = {0};

  if (inp.eof())
    throw IOException("Unexpected end-of-file");

  inp.getline(buffer, 255);
  return buffer;
}

string MM_NAMESPACE()::IO::readfile(const string& filename) {
  ifstream t(filename.c_str());
  string tmp((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
  return tmp;
}
