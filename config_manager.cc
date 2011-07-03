#include "config_manager.h"

using namespace std;
using namespace TOOLS;

ConfigManagerException::ConfigManagerException(const string& msg, const string& exc_name) :
  BaseException(msg, exc_name) {}

ConfigManagerException::ConfigManagerException(const ConfigManagerException& obj) :
  BaseException(obj.message, obj.exception_name) {}

UnknownParameter::UnknownParameter(const string& msg) :
  ConfigManagerException(msg, "UnknownParameter") {}

ConfigItemAlreadyExists::ConfigItemAlreadyExists(const string& msg) :
  ConfigManagerException(msg, "ConfigItemAlreadyExists") {}

ShortCommandAlreadyExists::ShortCommandAlreadyExists(const string& msg) :
  ConfigManagerException(msg, "ShortCommandAlreadyExists") {}

ConfigNotFound::ConfigNotFound(const string& msg) :
  ConfigManagerException(msg, "ConfigNotFound") {}

IncompatibleDataTypes::IncompatibleDataTypes(const string& dtype, const string& found) :
  ConfigManagerException("Wanted: '" + ConfigManager::type2str(dtype) + "' but found: '" + found + "'", "IncompatibleDataTypes") {}

ErrorParsingConfigFile::ErrorParsingConfigFile(const string& msg) :
  ConfigManagerException(msg, "ErrorParsingConfigFile") {}


ConfigManager::~ConfigManager() {
  // maybe the Items and the Groups - but when???
}

ConfigGroup::ConfigGroup(const std::string& name, ConfigManager* par) 
  : name(name), parent(par) {}
    
ConfigGroup* ConfigManager::new_group(const std::string& name) {
    ConfigGroup* grp = new ConfigGroup(name, this);
    groups.push_back(grp);
    return grp;
}

void ConfigManager::build_maps() {
  cmdmap.clear();
  data_types.clear();
  usagemap.clear();
  for(tGroupIter g=groups.begin(); g!=groups.end(); ++g) {
    for(tStringMapIter i=(*g)->_cmdmap.begin(); i!=(*g)->_cmdmap.end(); ++i)
      cmdmap[i->first] = i->second;
    for(tStringMapIter i=(*g)->_data_types.begin(); i!=(*g)->_data_types.end(); ++i)
      data_types[i->first] = i->second;
    for(tStringMapIter i=(*g)->_usagemap.begin(); i!=(*g)->_usagemap.end(); ++i) 
      usagemap[i->first] = i->second;
  }
}

void ConfigManager::parse(Stringlist* args) {
  string cmd = args->at(0);

  if(cmdmap.find(cmd) == cmdmap.end()) 
    throw UnknownParameter(cmd);

  string id = cmdmap[cmd];
  // catch boolean value as this doesn't need an arg
  if(data_types[id] == typeid(bool).name()) {
    set<bool>(id, true);
    args->erase(args->begin());
    return;
  }
  string arg = args->at(1);
  
  // check for integer and double
  try {
    if (data_types[id] == typeid(int).name()) {
      set<int>(id, integer(arg));
      args->erase(args->begin(), args->begin()+2);
      return;
    } else if(data_types[id] == typeid(double).name()) {
      set<double>(id, real(arg));
      args->erase(args->begin(), args->begin()+2);
      return;
    } 
  } catch (ConvertValueError e) { 
    IncompatibleDataTypes(data_types[id], arg);
  }
  
  // get string 
  if(data_types[id] == typeid(string).name()) {
    set<string>(id, str(arg));
    args->erase(args->begin(), args->begin()+2);
    return;
  }

  // build Stringlist from "," separated input
  if(data_types[id] == typeid(Stringlist).name()) {
    set<Stringlist>(id, XString(arg).split(","));
    args->erase(args->begin(), args->begin()+2);
    return;
  }

  throw IncompatibleDataTypes(data_types[id], arg);
}

void ConfigManager::parse_cmdline(int argc, char* argv[]) {
  build_maps();

  command = argv[0];

  Stringlist args;
  for(int i=1; i<argc; ++i)
    args.push_back(argv[i]);

  while (args.size() > 0)
    parse(&args);
}

void ConfigManager::parse_config_file(const string& fn) {
  build_maps();

  ifstream fd(fn.c_str(), ios::in);
  XString line;
  Stringlist tokens;
  while(fd.good()) {
    getline(fd, line);
    line.strip().strip("\n");
    if(line.length() == 0)
      continue;

    if (line.startswith("#"))
      continue;

    if (line.find("=") == string::npos) {
      tokens.push_back("--" + line);
      continue;
    }

    Stringlist lr = line.split("=");
    XString left(lr[0]), right(lr[1]);
    left.strip();
    right.strip();

    tokens.push_back("--" + left);
    tokens.push_back(right);
  }
  fd.close();
  
  try {
    while(tokens.size() > 0)
      parse(&tokens);
  } catch (IncompatibleDataTypes& e) {
    e.message += " (inside configfile)";
    throw e;
  }
}

void ConfigManager::usage(ostream& ss) {
  ss << "Usage: " << command << " <options>" << endl;
  ss << endl << "Options:" << endl;

  string::size_type id_len = 0, scmd_len = 0, desc_len = 0;
  for(tStringMapIter i=cmdmap.begin(); i!=cmdmap.end(); ++i) {
    if(i->first.find("--") != string::npos)
      id_len = max(i->first.length(), id_len);
    else 
      scmd_len = max(i->first.length(), scmd_len);
    desc_len = max(usagemap[i->second].length(), desc_len);
  }

  string cmd, scmd, id;
  for(tStringMapIter i=usagemap.begin(); i!=usagemap.end(); ++i) {
    id = i->first;
    for(tStringMapIter j=cmdmap.begin(); j!=cmdmap.end(); ++j) {
      if (j->second == id) {
        if (j->first.find("--") != string::npos)
          cmd = j->first;
        else if (j->first.find("-") != string::npos)
          scmd = j->first;
      }
    }
    ss << right << setw(scmd_len+1) << scmd << " | " << flush;
    ss << left << setw(id_len+2) << cmd << "   " << i->second;
    ss << " [" << ConfigManager::type2str(data_types[id]) << "]";
    ss << endl;
  }
  ss << endl;
}

string ConfigManager::type2str(const string& dtype) {
  if (dtype == typeid(int).name())
    return "integer";
  else if (dtype == typeid(double).name())
    return "float";
  else if (dtype == typeid(string).name())
    return "string";
  else if (dtype == typeid(Stringlist).name())
    return "string list";
  else if (dtype == typeid(bool).name())
    return "boolean";
  return "UNKNOWN";
}


//#include<stdlib.h>
//#include<stdio.h>

/*int main(int argc, char *argv[]) {
  ConfigManager m;
  ConfigGroup* g1 = m.new_group("Application Server");
  g1->new_option<string>("my-id-name", "blabla foo bla", "s").set_default("foo");
  g1->new_option<double>("some-other", "ihfsdauisdh", "w");
  g1->new_option<bool>("andsobool", "jojojo????", "j").set_default(false);
  g1->new_option<int>("abba", "my fancey nice description", "a").set_default(12);
  g1->new_option<Stringlist>("more", "wh00000tuuup", "m");

  try {
    m.parse_config_file("my.conf");
    m.parse_cmdline(argc, argv);
  } catch (ConfigManagerException e) {
    e.show();
    m.usage(cout);
    exit(1);
  } 
  cout << "parsing finished ";

  cout << "--- test --- test --- test ---" << endl;
  cout << g1->get<string>("my-id-name") << endl;
  cout << g1->get<double>("some-other") << endl;
  cout << ((g1->get<bool>("andsobool")) ? "jo" : "nee") << endl; 
  cout << g1->get<int>("abba") << endl;
  g1->set<int>("abba", 1234);
  cout << "abba: " << g1->get<int>("abba") << endl;
  Stringlist foo = m.get<Stringlist>("more");
  for(Stringlist::iterator i=foo.begin(); i!=foo.end(); ++i)
    cout << "list: " << *i << endl;
  return 0;
}*/





