#include "config_manager.h"

using namespace std;
using namespace TOOLS;

ConfigDataKeeper::ConfigDataKeeper(const ConfigDataKeeper& obj) 
  : storage(obj.storage), tinfo(obj.tinfo) { }


ConfigDataKeeper::ConfigDataKeeper(void* data, const string& tinfo) 
  : storage(data), tinfo(tinfo) { }

ConfigDataKeeper::ConfigDataKeeper(const string& tinfo) 
  : storage(nullptr), tinfo(tinfo) { }

// return verbose type string
string ConfigDataKeeper::verbose_type() const {
  if(tinfo == typeid(int).name())
    return "integer";
  else if(tinfo == typeid(double).name())
    return "float";
  else if(tinfo == typeid(string).name())
    return "string";
  else if(tinfo == typeid(tStringList).name())
    return "string list";
  else if(tinfo == typeid(tStringMap).name())
    return "string map";
  else if(tinfo == typeid(bool).name())
    return "boolean";
  return "unknown data type!!! ";
}

// return verbose data string representation
string ConfigDataKeeper::verbose_data(void* raw_data) const {
  string out;
  const ConfigDataKeeper* cdk;
  if(raw_data != nullptr)
    cdk = new ConfigDataKeeper(raw_data, tinfo);
  else
    cdk = this;

  if(tinfo == typeid(int).name())
    out = str(cdk->get<int>());
  else if(tinfo == typeid(double).name())
    out = str(cdk->get<double>());
  else if(tinfo == typeid(string).name())
    out = cdk->get<string>();
  else if(tinfo == typeid(tStringList).name())
    out = XString(",").join(cdk->get<tStringList>());
  else if(tinfo == typeid(tStringMap).name()) {
    tStringList tmp;
    for(auto& key_val : cdk->get<tStringMap>())
      tmp.push_back(key_val.first + "=" + key_val.second);
    out = XString(",").join(tmp);
  } else if(tinfo == typeid(bool).name())
    out = (cdk->get<bool>()) ? "true" : "false";
  else
    out = "unknown data";

  if(raw_data != nullptr)
    delete cdk;
  return out;
}

ConfigOption::ConfigOption(const std::string& id, const std::string& desc, \
  const std::string& tinfo, const std::string& scmd)
  : data(tinfo), default_data(tinfo), id(id), cmd_short(scmd), desc(desc), 
    was_set(false), has_default(false), parent(NULL) { }


string ConfigOption::verbose_type() const {
  return data.verbose_type();
}

string ConfigOption::verbose_data() const {
  return data.verbose_data();
}

string ConfigOption::verbose_default() const {
  if(has_default)
    return default_data.verbose_data();
  return "<no default set>";
}

ConfigGroup::ConfigGroup(const std::string& name, ConfigManager* par)
  : name(name), parent(par)  {}

ConfigGroup::iterator ConfigGroup::begin() {
  return members.begin();
}

ConfigGroup::const_iterator ConfigGroup::begin() const { 
  return members.begin();
}

ConfigGroup::iterator ConfigGroup::end() { 
  return members.end();
}

ConfigGroup::const_iterator ConfigGroup::end() const { 
  return members.end();
}

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
  if(it == groups.end())
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

ConfigManager::iterator ConfigManager::begin() { 
  return groups.begin(); 
}

ConfigManager::const_iterator ConfigManager::begin() const { 
  return groups.begin();
}

ConfigManager::iterator ConfigManager::end() { 
  return groups.end();
}

ConfigManager::const_iterator ConfigManager::end() const { 
  return groups.end();
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
    if(members[id]->has_default)
      set<bool>(id, !get<bool>(id));
    else
      set<bool>(id, true);
    args->erase(args->begin());
    return;
  }

  if(args->size() < 2)
    throw MissingParameter(cmd + " - found no more args");

  string arg = args->at(1);

  // check for integer and double
  try {
    if(tmp->same_data_types<int>()) {
      set<int>(id, integer(arg));
      args->erase(args->begin(), args->begin()+2);
      return;
    } else if(tmp->same_data_types<double>()) {
      set<double>(id, real(arg));
      args->erase(args->begin(), args->begin()+2);
      return;
    }
  } catch(ConvertValueError e) {
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
    // build tStringMap from "," and "=" separated input
  }
  if(tmp->same_data_types<tStringMap>()) {
    tStringList tmp = XString(arg).split(",");
    tStringMap tmpmap;
    for(tStringIter i=tmp.begin(); i!=tmp.end(); ++i) {
      tStringList two = XString(*i).split("=");
      tmpmap[two[0]] = two[1];
    }
    set<tStringMap>(id, tmpmap);
    args->erase(args->begin(), args->begin()+2);
    return;
  }
  throw IncompatibleDataTypes("data: " + tmp->verbose_type() + " passed: " + arg);
}

void ConfigManager::parse_cmdline(int argc, char* argv[]) {
  tStringList args;
  for(int i=1; i<argc; ++i)
    args.push_back(argv[i]);

  while(args.size() > 0)
    parse(&args);
}

void ConfigManager::write_config_file(ostream& fd, bool shorter) {
  fd << endl;
  fd << "###" << endl;
  fd << "### config file for: " << command << endl;
  fd << "###" << endl;
  fd << endl;
  for(tGroupPair& grp : groups) {
    if(!shorter)
      fd << "####################################################" << endl;
    fd << "# [ " << grp.first << " ]" << endl;
    for(tOptionPair& opt : *grp.second) {
      const ConfigOption* c = opt.second;
      const string& id = opt.first;

      // write desc/help text
      if(!shorter) {
        fd << "# " << c->desc << " [" << c->verbose_type() << "]";      
        // write if ConfigOption has default
        if(c->has_default)
          fd << " default: " << c->verbose_default();
        fd << endl;
      }

      // ConfigOption was set, output its value
      if(c->was_set) {
        fd << id << " = " << c->verbose_data() << endl;
      // ConfigOption was _not_ set, output commented out line and default
      } else if(c->has_default) {
          fd << "# " << id << " = " << c->verbose_default() << endl;
      // ConfigOption was neither set nor has a default value
      } else {
        if(!shorter)
          fd << "# " << id << " = " << "<not set>" << endl;
      }
      if(!shorter)
        fd << endl;
    }
  }
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

    if(line.startswith("#"))
      continue;

    if(line.find("=") == string::npos) {
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
  } catch(IncompatibleDataTypes& e) {
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
