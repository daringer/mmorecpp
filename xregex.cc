#include "xregex.h"

using namespace std;
using namespace TOOLS;

RegexException::RegexException(const string& msg) : BaseException(msg, "RegexException") {}

RegexException::RegexException(int errcode, const regex_t* pattern) : BaseException("", "RegexException") {
  char buf[BUF_LEN];
  regerror(errcode, pattern, (char*) &buf, BUF_LEN);
  set_message(buf);
}

Regex::Regex(const std::string& query) : raw_pattern(query) {
  int ret = regcomp(&pattern, query.c_str(), REG_EXTENDED);
  if(ret != 0)
    throw RegexException(ret, &pattern);
}

Regex::~Regex() {
  regfree(&pattern);
}

void Regex::apply_pattern(const string& s, int max_results) {
  regmatch_t res[MAX_GROUP_COUNT];

  const char* pos = s.c_str();
  int offset = 0;
  int ret;

  results.clear();

  //cout << "whooot" << endl;

  while((ret = regexec(&pattern, pos, MAX_GROUP_COUNT, res, 0)) == 0) {
    Grouplist group_data;
    int group = -1;
    while(res[++group].rm_so != -1)
      group_data.push_back(s.substr(res[group].rm_so + offset, res[group].rm_eo - res[group].rm_so));
    results.push_back(group_data);

    int len = (res[0].rm_eo - res[0].rm_so);
    pos += len;
    offset += len;

    if(max_results == 0)
      break;
    max_results--;
  }
  if(ret != REG_NOMATCH && ret != 0)
    throw RegexException(ret, &pattern);
}

Regex::iterator Regex::begin() {
  return results.begin();
}

Regex::iterator Regex::end() {
  return results.end();
}

bool Regex::match(const string& s) {
  apply_pattern(s, 1);
  return (results.size() > 0) ? true : false;
}

Regex::Matchinglist& Regex::search(const string& s) {
  apply_pattern(s, -1);
  return results;
}

string Regex::replace(const string& s, const string& repl) {
  apply_pattern(s, -1);

  if(results.size() == 0)
    return s;

  Regex re_repl("[\\][1-9]");
  Regex::Matchinglist refs = re_repl.search(repl);

  XString out = s;
  for(iterator i=begin(); i!=end(); ++i) {
    XString replacement = repl;
    for(iterator j=refs.begin(); j!=refs.end(); ++j)
      replacement.subs((*j)[0], (*i)[integer((*j)[0].substr(1))]);
    out.subs((*i)[0], replacement);
  }
  return out;
}

