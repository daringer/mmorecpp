#include "config_manager.h"

using namespace std;
using namespace TOOLS;

NoSuchConfigItemException::NoSuchConfigItemException(const string& msg) :
  BaseException(msg, "NoSuchConfigItemException") { }

ConfigItemAlreadyExists::ConfigItemAlreadyExists(const string& msg) :
  BaseException(msg, "ConfigItemAlreadyExists") { }

RequiredConfigItemNotSet::RequiredConfigItemNotSet(const string& msg) :
  BaseException(msg, "RequiredConfigItemNotSet") {}

ConfigItem::ConfigItem(const std::string& name, const std::string& desc, 
  const std::string& lcmd, const std::string& scmd, const std::string& itype)
    : name(name), desc(desc), cmd_long(lcmd), cmd_short(scmd), item_type(itype) {}

void ConfigItem::set_flags(int value) {
  // apply default OCCURE_ONCE flag if not set
  if(value & (OCCURE_ONCE | OCCURE_POSITIONAL | OCCURE_MULTI) == 0)
    value |= OCCURE_ONCE;
  flags = value;
}

ConfigManager::ConfigManager(const string& name, const string& desc) 
  : meta_name(name), meta_desc(desc) {}

ConfigManager::~ConfigManager() {
  for(tConfigItemIter i=config.begin(); i!=config.end(); ++i)
    delete i->second;
}

int ConfigManager::parse_item(const string& key, const string& val) {
  for(tConfigItemIter i=config.begin(); i!=config.end(); ++i) {
    if(key != ("--" + i->second->cmd_long) && key != ("-" + i->second->cmd_short)) 
      continue;

    if(i->second->item_type == typeid(string).name()) {
      set<string>(i->second->name, str(val));
      return 2;
    } else if(i->second->item_type == typeid(int).name()) {
      set<int>(i->second->name, integer(val));
    } else if(i->second->item_type == typeid(double).name()) {
      set<double>(i->second->name, real(val));
      return 2;
    } else if(i->second->item_type == typeid(bool).name()) {
      set<bool>(i->second->name, bool(true));
      return 1;
    } else
      cout << "Looks like a bug!" << endl;
  }
  return 0;
}

void ConfigManager::parse_cmdline(int argc, char *argv[]) {
  vector<string> tmp;
  for(int i=0; i<argc; ++i) {
    if (i==0)
      command = argv[i];
    else
      tmp.push_back(argv[i]);
  }

  while(tmp.size() != 0) {
    int to_remove = parse_item(tmp[0], (tmp.size() > 1) ? tmp[1] : tmp[0]);
    if(to_remove == 0)
      throw NoSuchConfigItemException("The param: '" + tmp[0] + "' is not valid!");
    else if(to_remove == 1)
      tmp.erase(tmp.begin());
    else if(to_remove == 2) {
      tmp.erase(tmp.begin());
      tmp.erase(tmp.begin());
    }
  }

  for(tConfigItemIter i=config.begin(); i!=config.end(); ++i) {
    if((i->second->flags & REQUIRED) && (data[config[i->first]].size() == 0))
      throw RequiredConfigItemNotSet(
          "The required config item: " + i->first + " was not set");
  }
}
 
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

void ConfigManager::usage(ostream& ss) {
  ss << meta_name << " - " << meta_desc << endl;
  
  ss << "Usage: " << command << " <";
  string::size_type cmd_len = 0;
  for(tConfigItemIter i=config.begin(); i!=config.end(); ++i) {
    if ((i->second->item_type != typeid(bool).name())) 
      ss << "[-" << i->second->cmd_short << " <data>" << "]";
    else
      ss << "[-" << i->second->cmd_short << "]";
    
    cmd_len = (cmd_len < i->second->cmd_short.length()) ?
      i->second->cmd_short.length() : cmd_len;
  }
  ss << ">" << endl << endl;

  for(tConfigItemIter i=config.begin(); i!=config.end(); ++i)
    ss << setw(6)  << "-" + i->second->cmd_short << setiosflags(ios::right) << setw(3) << \
      "| " << setw(cmd_len+10) << "--" + i->second->cmd_long << \
      setw(10) << i->second->name << "    (" << i->second->desc << ")" << endl;
  ss << endl;
}

ConfigManager::tConfigData::size_type 
ConfigManager::count_data_items(const string& name) {
  return data[config[name]].size();
}

/*
int main(int argc, char *argv[]) {
 
  ConfigManager m("Test", "Test foo");
  m.register_config_item<string>("p1", "tjodsifjiofds dqwdw ddqwd", "something", "sh");
  m.register_config_item<double>("p2", "ihfsdauisdh", "was", "w", REQUIRED | OCCURE_ONCE);
  m.register_config_item<bool>("p3", "jojojo????", "jhdss", "j", false);

  try {
    m.parse_cmdline(argc, argv);
  } catch (NoSuchConfigItemException e) {
    e.show();
    m.usage(cout);
    //exit(1);
  } catch (RequiredConfigItemNotSet e) {
    e.show();
    m.usage(cout);
    //exit(1);
  }

  cout << "test?" << endl;
  cout << m.get_data<string>("p1") << endl;
  cout << m.get_data<double>("p2") << endl;
  cout << ((m.get_data<bool>("p3")) ? "jo" : "nee") << endl;
}*/





