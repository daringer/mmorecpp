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

IncompatibleDataTypes::IncompatibleDataTypes(const string& msg) :
  ConfigManagerException(msg, "IncompatibleDataTypes") {}


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
  //cout << "build maps own ptr:  " << (int) 
  //cout << "BUILD MAPS" << endl;
  cmdmap.clear();
  data_types.clear();
  for(tGroupIter g=groups.begin(); g!=groups.end(); ++g) {
    //cout << "group pointer inside parse-loop: " << (int) *g << endl;
    //cout << "GROUP: " << (*g)->name << endl;
    //cout << "_cmdmap inside build maps?!: " << (*g)->_cmdmap.size() << endl;
    for(tStringMapIter i=(*g)->_cmdmap.begin(); i!=(*g)->_cmdmap.end(); ++i) //{
      cmdmap[i->first] = i->second;
      //cout << "cmdmap:  " << j->first << " -> " << j->second << endl;
    //}
    for(tStringMapIter i=(*g)->_data_types.begin(); i!=(*g)->_data_types.end(); ++i) //{
      data_types[i->first] = i->second;
      //cout << "data_types:  " << i->first << " -> " << i->second << endl;
    //}
    for(tStringMapIter i=(*g)->_usagemap.begin(); i!=(*g)->_usagemap.end(); ++i) 
      usagemap[i->first] = i->second;
  }
}


int ConfigManager::parse(int pos, int max, char* argv[]) {
  if(cmdmap.find(argv[pos]) == cmdmap.end()) 
    throw UnknownParameter(argv[pos]);

  string id = cmdmap[argv[pos]];
  
  //cout << "found: " << id << endl;
  
  if(data_types[id] == typeid(bool).name()) {
    set<bool>(id, true);
    return 1;
  }

  string arg = argv[pos+1];
  try {
    if (data_types[id] == typeid(int).name()) {
      set<int>(id, integer(arg));
      return 2;
    } else if(data_types[id] == typeid(double).name()) {
      set<double>(id, real(arg));
      return 2;
    }
  } catch (ConvertValueError e) { 
    IncompatibleDataTypes("id is of typeid: " + data_types[id] + \
                          " and the param is not! (" + arg + ")");
  }
  if(data_types[id] == typeid(string).name()) {
    set<string>(id, str(arg));
    return 2;
  }
  if(data_types[id] == typeid(Stringlist).name()) {
    set<Stringlist>(id, XString(arg).split(","));
    return 2;
  }

  throw UnknownParameter("'" + id + "' needs '" + data_types[id] + \
                         "' but found unknown (" + arg + ")");
}

void ConfigManager::parse_cmdline(int argc, char* argv[]) {
  build_maps();

  //cout << "global cmdmaps: " << cmdmap.size() << endl;
  //cout << "global dt: " << data_types.size() << endl;


  command = argv[0];
  int pos = 1;

  while(pos < argc) 
    pos += parse(pos, argc, argv);
    

  // Validation //
  /*for(tConfigItemIter i=config.begin(); i!=config.end(); ++i) {
    if((i->second->flags & CF_REQUIRED) && (data[config[i->first]].size() == 0))
      throw RequiredConfigItemNotSet(
          "The required config item: " + i->first + " was not set");
  }*/
}
/*
void ConfigManager::load_config_file(const string& fn) {
  ifstream fd(fn.c_str(), ios::in);
  XString line;
  while(fd.good()) {
    getline(fd, line);
    vector<string> lr = line.split("=");
    XString left(lr[0]);
    XString right(lr[1]);
    parse_item(left.strip(), right.strip());
  }
}
*/
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
    if (data_types[id] == typeid(Stringlist).name())
        ss << " (multiple possible - separate with ',')";
    ss << endl;
  }
  ss << endl;
}


#include<stdlib.h>
#include<stdio.h>

int main(int argc, char *argv[]) {

  ConfigManager m;
  ConfigGroup* g1 = m.new_group("Application Server");
  g1->new_option<string>("my-id-name", "blabla foo bla", "s").set_default("foo");
  g1->new_option<double>("some-other", "ihfsdauisdh", "w");
  g1->new_option<bool>("andsobool", "jojojo????", "j").set_default(false);
  g1->new_option<int>("abba", "my fancey nice description", "a").set_default(12);
  g1->new_option<Stringlist>("more", "wh00000tuuup", "m");

  try {
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
}





