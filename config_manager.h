#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <typeinfo>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "general.h"
#include "exception.h"
#include "xstring.h"


namespace TOOLS {

class ConfigGroup;
class ConfigOption;

DEFINE_PARENT_EXCEPTION(ConfigManagerException, BaseException);
DEFINE_EXCEPTION(UnknownParameter,              ConfigManagerException);
DEFINE_EXCEPTION(MissingParameter,              ConfigManagerException);
DEFINE_EXCEPTION(ValueHasNotBeenSet,            ConfigManagerException);
DEFINE_EXCEPTION(OptionAlreadyExists,           ConfigManagerException);
DEFINE_EXCEPTION(ShortCommandAlreadyExists,     ConfigManagerException);
DEFINE_EXCEPTION(OptionNotFound,                ConfigManagerException);
DEFINE_EXCEPTION(GroupNotFound,                 ConfigManagerException);
DEFINE_EXCEPTION(IncompatibleDataTypes,         ConfigManagerException);
DEFINE_EXCEPTION(ErrorParsingConfigFile,        ConfigManagerException);

typedef std::map<std::string, ConfigOption*> tOptionMap;
typedef tOptionMap::iterator tOptionIter;

typedef std::map<std::string, ConfigGroup*> tGroupList;
typedef tGroupList::iterator tGroupIter;


class ConfigDataKeeper {
  private:
    void* storage;
  public:
    std::string tinfo;
    
    ConfigDataKeeper(const std::string& tinfo);
    ConfigDataKeeper(void* data, const std::string& tinfo);

    std::string verbose_type();

    template<class T>
    const T& get() {
      if (!same_data_types<T>())
        throw IncompatibleDataTypes("Data: " + tinfo + \
            " Template(Keeper::get) was: " + typeid(T).name());
      return *( (T*) storage);
    };

    template<class T>
    void set(const T& value) {
      if(!same_data_types<T>())
        throw IncompatibleDataTypes("Data: " + tinfo + \
            " Template(Keeper::set) was: " + typeid(T).name());
      
      T* ptr = new T(value);
      storage = (void*) ptr;
    };

    template<class T>
    bool same_data_types() {
      return (tinfo == typeid(T).name());
    }
}; 

// is is also longcommandline arg and 
class ConfigOption {
  public:
    ConfigDataKeeper data;
    // id is also used as long cmd
    std::string id; 
    std::string cmd_short;
    std::string desc;
    bool was_set;
    bool has_default;
    ConfigGroup* parent;

    ConfigOption(const std::string& id, const std::string& desc, const std::string& tinfo, const std::string& scmd); 

    template<class T>
    ConfigOption& set_default(const T& value) {
      if(!data.same_data_types<T>())
        throw IncompatibleDataTypes("Data has: " + data.verbose_type() + \
            " Template(Option::set_default) was: " + typeid(T).name());
      
      has_default = true;
      data.set<T>(value);
      // set back "was_set" to false, as default doesn't count
      was_set = false;

      return *this;
    }

    // workaround to allow to pass "blafoo" directly
    template<class T>
    ConfigOption& set_default(T value[]) {
      return set_default(str(value));
    }
    
    template<class T>
    ConfigOption& set(const T& value) {
      if(!data.same_data_types<T>())
        throw IncompatibleDataTypes("Data has: " + data.verbose_type() + \
            " Template(Option::set) was: " + typeid(T).name());

      data.set<T>(value);
      was_set = true;
      return *this;
    }

    template<class T>
    ConfigOption& set(T value[]) {
      return set(str(value));
    }

    template<class T>
    const T& get() {
      if (!was_set && !has_default)
        throw ValueHasNotBeenSet(id);
      return data.get<T>();
    }
};

class ConfigManager {
  public:
    tOptionMap members;
    tGroupList groups;
    tStringMap cmdmap;
    
    std::string command;
    
    ConfigManager(const std::string& start_command);

    ConfigOption& get_option(const std::string& id);

    template<class T>
    const T& get(const std::string& id) {
      return get_option(id).get<T>();
    }
    
    template<class T>
    ConfigOption& set(const std::string& id, const T& data) {
      return get_option(id).set(data);
    }
 
    bool is_option_set(const std::string& id);
    bool is_group_active(const std::string& name); 

    ConfigGroup& new_group(const std::string& name);
    ConfigGroup& get_group(const std::string& name);
    ConfigGroup& get_group_from_option(const std::string& id);
    
    void parse(tStringList* args);
    void parse_cmdline(int argc, char* argv[]);
    void parse_config_file(const std::string& fn);
    void usage(std::ostream& ss);
};

class ConfigGroup {
  public:
    tOptionMap members;
    std::string name;
    ConfigManager* parent;
 
    // command -> id   
    tStringMap _cmdmap;

    ConfigGroup(const std::string& name, ConfigManager* par);
    
    ConfigOption& get_option(const std::string& id);

    template<class T>
    ConfigOption& new_option(const std::string& new_id, const std::string& desc, const std::string& short_cmd="") {
      if(members.find(new_id) != members.end())
        throw OptionAlreadyExists(new_id);

      std::string scmd = "-" + short_cmd;
      std::string lcmd = "--" + new_id;

      if(parent->cmdmap.find(scmd) != parent->cmdmap.end())
        throw ShortCommandAlreadyExists(scmd);

      members[new_id] = new ConfigOption(new_id, desc, typeid(T).name(), short_cmd);
      members[new_id]->parent = this;
      parent->members[new_id] = members[new_id];

      _cmdmap[lcmd] = new_id;
      parent->cmdmap[lcmd] = new_id;
      if(short_cmd != "") {
        _cmdmap[scmd] = new_id;
        parent->cmdmap[scmd] = new_id;
      }
      return *members[new_id];
    }
};
}

#endif
