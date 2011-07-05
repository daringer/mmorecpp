#include "config_manager.h"

using namespace std;
using namespace TOOLS;

ConfigDataKeeper::ConfigDataKeeper(void* data, const string& tinfo) : storage(data), tinfo(tinfo) { }

ConfigDataKeeper::ConfigDataKeeper(const string& tinfo) : storage(NULL), tinfo(tinfo) { }

string ConfigDataKeeper::verbose_type() {
  if (tinfo == typeid(int).name())
    return "integer";
  else if (tinfo == typeid(double).name())
    return "float";
  else if (tinfo == typeid(string).name())
    return "string";
  else if (tinfo == typeid(tStringList).name())
    return "string list";
  else if (tinfo == typeid(bool).name())
    return "boolean";
  return "UNKNOWN";
}

ConfigOption::ConfigOption(const std::string& id, const std::string& desc, const std::string& tinfo, const std::string& scmd) 
  : data(tinfo), id(id), cmd_short(scmd), desc(desc), was_set(false), has_default(false), parent(NULL) {}

ConfigGroup::ConfigGroup(const std::string& name, ConfigManager* par) 
  : name(name), parent(par)  {}
    
ConfigOption& ConfigGroup::get_option(const std::string& id) {
  tOptionIter it = members.find(id);
  if(it != members.end())
    return *it->second;
  throw OptionNotFound(id);
}

ConfigGroup& ConfigManager::new_group(const std::string& name) {
    groups[name] = new ConfigGroup(name, this);
    return *groups[name];
}

ConfigGroup& ConfigManager::get_group(const string& name) {
  tGroupIter it = groups.find(name);
  if (it == groups.end())
    throw GroupNotFound(name);
  return *it->second;
}

ConfigGroup& ConfigManager::get_group_from_option(const string& name) {
  return *(get_option(name).parent);
}

ConfigOption& ConfigManager::get_option(const std::string& id) {
  tOptionIter it = members.find(id);
  if(it == members.end())
    throw OptionNotFound(id);
  return *(it->second);
}

ConfigManager::ConfigManager(const std::string& start_command) 
  : command(start_command) { }

void ConfigManager::parse(tStringList* args) {
  string cmd = args->at(0);

  if(cmdmap.find(cmd) == cmdmap.end()) 
    throw UnknownParameter(cmd);

  string id = cmdmap[cmd];
  ConfigDataKeeper* tmp = &members[id]->data;
  
  // catch boolean value as this doesn't need an arg
  if(tmp->same_data_types<bool>()) {
    set<bool>(id, true);
    args->erase(args->begin());
    return;
  }

  if(args->size() < 2)
    throw MissingParameter(cmd + " - found no more args");

  string arg = args->at(1);
  
  // check for integer and double
  try {
    if (tmp->same_data_types<int>()) {
      set<int>(id, integer(arg));
      args->erase(args->begin(), args->begin()+2);
      return;
    } else if(tmp->same_data_types<double>()) {
      set<double>(id, real(arg));
      args->erase(args->begin(), args->begin()+2);
      return;
    } 
  } catch (ConvertValueError e) { 
    throw IncompatibleDataTypes("data: " + tmp->verbose_type() + " passed: " + arg);
  }
  
  // get string 
  if(tmp->same_data_types<string>()) {
    set<string>(id, str(arg));
    args->erase(args->begin(), args->begin()+2);
    return;
  }

  // build tStringList from "," separated input
  if(tmp->same_data_types<tStringList>()) {
    set<tStringList>(id, XString(arg).split(","));
    args->erase(args->begin(), args->begin()+2);
    return;
  }
  throw IncompatibleDataTypes("data: " + tmp->verbose_type() + " passed: " + arg);
}

void ConfigManager::parse_cmdline(int argc, char* argv[]) {
  tStringList args;
  for(int i=1; i<argc; ++i)
    args.push_back(argv[i]);

  while (args.size() > 0)
    parse(&args);
}

void ConfigManager::parse_config_file(const string& fn) {
  ifstream fd(fn.c_str(), ios::in);
  XString line;
  tStringList tokens;
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

    tStringList lr = line.split("=");
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

bool ConfigManager::is_group_active(const std::string& name) {
  tOptionMap& opts = groups[name]->members;
  for(tOptionIter i=opts.begin(); i!=opts.end(); ++i)
    if(i->second->was_set)
      return true;
  return false;
}

bool ConfigManager::is_option_set(const std::string& id) {
  return (get_option(id).was_set || get_option(id).has_default);
}

void ConfigManager::usage(ostream& ss) {
  ss << "Usage: " << command << " <options>" << endl;
  ss << endl << "Options:" << endl;

  string::size_type id_len = 0, scmd_len = 0, desc_len = 0;
  for(tStringMapIter i=cmdmap.begin(); i!=cmdmap.end(); ++i) {
    ConfigOption* opt = members[i->second];
    if(i->first.find("--") != string::npos)
      id_len = max(opt->id.length(), id_len);
    else 
      scmd_len = max(opt->cmd_short.length(), scmd_len);
    desc_len = max(opt->desc.length(), desc_len);
  }

  for(tGroupIter g=groups.begin(); g!=groups.end(); ++g) {
    ConfigGroup* grp = g->second;
    ss << endl << "--- Group: " << grp->name << endl;
    for(tOptionIter i=grp->members.begin(); i!=grp->members.end(); ++i) {
      ConfigOption* opt = grp->members[i->first];

      ss << right << setw(scmd_len+1) << ((opt->cmd_short.size() > 0) ? \
          ("-" + opt->cmd_short) : "") << " | " << flush;
      ss << left << setw(id_len+2) << ("--" + opt->id) << "   " << opt->desc;
      ss << " [" << opt->data.verbose_type() << "]";
      ss << endl;
    }
  }
  ss << endl;
}

/*
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  set_terminate(TOOLS::tools_lib_exception_handler);

  ConfigManager m(argv[0]); 
  ConfigGroup g1 = m.new_group("Application Server");
  ConfigGroup g2 = m.new_group("another one");
  g1.new_option<string>("my-id-name", "blabla foo bla", "s").set_default("foo");
  g1.new_option<double>("some-other", "ihfsdauisdh", "w").set_default(13.123);
  g2.new_option<bool>("andsobool", "jojojo????", "j").set_default(false);
  g2.new_option<int>("abba", "my fancey nice description", "a").set_default(12);
  g2.new_option<tStringList>("more", "wh00000tuuup", "m");
  
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
  cout << m.get<string>("my-id-name") << endl;
  cout << m.get<double>("some-other") << endl;
  cout << ((m.get<bool>("andsobool")) ? "jo" : "nee") << endl; 
  cout << m.get<int>("abba") << endl;
  m.set<int>("abba", 1234);
  cout << "abba: " << m.get<int>("abba") << endl;
  tStringList foo = m.get<tStringList>("more");
  for(tStringList::iterator i=foo.begin(); i!=foo.end(); ++i)
    cout << "list: " << *i << endl;
  
  return 0;
}*/
