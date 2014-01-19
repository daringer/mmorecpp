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

DEFINE_EXCEPTION(ConfigManagerException, BaseException)
DEFINE_EXCEPTION(UnknownParameter,              ConfigManagerException)
DEFINE_EXCEPTION(MissingParameter,              ConfigManagerException)
DEFINE_EXCEPTION(ValueHasNotBeenSet,            ConfigManagerException)
DEFINE_EXCEPTION(OptionAlreadyExists,           ConfigManagerException)
DEFINE_EXCEPTION(ShortCommandAlreadyExists,     ConfigManagerException)
DEFINE_EXCEPTION(OptionNotFound,                ConfigManagerException)
DEFINE_EXCEPTION(GroupNotFound,                 ConfigManagerException)
DEFINE_EXCEPTION(IncompatibleDataTypes,         ConfigManagerException)
DEFINE_EXCEPTION(ErrorParsingConfigFile,        ConfigManagerException)

typedef std::map<std::string, ConfigOption*> tOptionMap;
typedef tOptionMap::value_type tOptionPair;
typedef tOptionMap::iterator tOptionIter;
typedef tOptionMap::const_iterator tOptionCIter;

typedef std::map<std::string, ConfigGroup*> tGroupList;
typedef tGroupList::value_type tGroupPair;
typedef tGroupList::iterator tGroupIter;
typedef tGroupList::const_iterator tGroupCIter;

// class handling the different data-types transparently
class ConfigDataKeeper {
  private:
    // contains the stored data
    void* storage;

  public:
    // typeinfo string for saved datatype
    std::string tinfo;

    // copy-constructor
    explicit ConfigDataKeeper(const ConfigDataKeeper& obj);
    // either start with empty 
    ConfigDataKeeper(const std::string& tinfo);
    // or directly with contents
    ConfigDataKeeper(void* data, const std::string& tinfo);

    // return verbose type identification or string representation
    std::string verbose_type() const;
    std::string verbose_data(void* raw_data=nullptr) const;

    // return data with type provided by template e.g., get<int>
    template<class T>
    const T& get() const {
      if(!same_data_types<T>())
        throw IncompatibleDataTypes( \
            "Data: " + tinfo + \
            " Template(Keeper::get) was: " + typeid(T).name());
      return *((T*) storage);
    }

    // set data to 'value'
    template<class T>
    void set(const T& value) {
      if(!same_data_types<T>())
        throw IncompatibleDataTypes( \
            "Data: " + tinfo + \
            " Template(Keeper::set) was: " + typeid(T).name());

      T* ptr = new T(value);
      storage = (void*) ptr;
    }

    // verify identical underlying type 
    template<class T>
    bool same_data_types() const {
      return (tinfo == typeid(T).name());
    }
};

class ConfigOption {
  public:
    ConfigDataKeeper data;
    ConfigDataKeeper default_data;
    std::string id;
    std::string cmd_short;
    std::string desc;
    bool was_set;
    bool has_default;
    ConfigGroup* parent;

    ConfigOption(const std::string& id, const std::string& desc, \
        const std::string& tinfo, const std::string& scmd);

    template<class T>
    ConfigOption& set_default(const T& value) {
      if(!data.same_data_types<T>())
        throw IncompatibleDataTypes( \
            "Data has: " + data.verbose_type() + \
            " Template(Option::set_default) was: " + typeid(T).name());

      has_default = true;
      data.set<T>(value);
      default_data.set<T>(value);

      // set back "was_set" to false, as default doesn't count
      was_set = false;

      return *this;
    }

    // workaround to allow to pass "blafoo" directly
    template<class T>
    ConfigOption& set_default(T value[]) {
      return set_default(str(value));
    }

    // global setter for a option
    template<class T>
    ConfigOption& set(const T& value) {
      if(!data.same_data_types<T>())
        throw IncompatibleDataTypes( \
            "Data has: " + data.verbose_type() + \
            " Template(Option::set) was: " + typeid(T).name());

      data.set<T>(value);
      was_set = true;
      return *this;
    }

    // wrap to global setter
    template<class T>
    ConfigOption& set(T value[]) {
      return set(str(value));
    }

    // get data from option
    template<class T>
    const T& get() {
      if(!was_set && !has_default)
        throw ValueHasNotBeenSet(id);
      return data.get<T>();
    }

    // verbose output methods
    std::string verbose_type() const;
    std::string verbose_data() const;
    std::string verbose_default() const;
};

class ConfigManager {
  public:
    tOptionMap members;
    tGroupList groups;
    tStringMap cmdmap;

    std::string command;

    // iteration over config manager returns ConfigGroups
    typedef tGroupIter iterator;
    typedef tGroupCIter const_iterator;
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;

    // construct ConfigManager instance for binary called: start_command
    ConfigManager(const std::string& start_command);

    // get option by id (long-cmdline-id without "--")
    ConfigOption& get_option(const std::string& id);

    // pass-through to ConfigOption
    template<class T>
    const T& get(const std::string& id) {
      return get_option(id).get<T>();
    }

    // pass-through to ConfigOption
    template<class T>
    ConfigOption& set(const std::string& id, const T& data) {
      return get_option(id).set(data);
    }

    // is ConfigOption set?
    bool is_option_set(const std::string& id);
    // is ConfigGroup active?
    bool is_group_active(const std::string& name);

    // group handling methods
    ConfigGroup& new_group(const std::string& name);
    ConfigGroup& get_group(const std::string& name);
    ConfigGroup& get_group_from_option(const std::string& id);

    // parse list of (correctly splitted) strings
    void parse(tStringList* args);
    // parse raw cmdline into list and pass it to parse()
    void parse_cmdline(int argc, char* argv[]);

    // parse config from file
    void parse_config_file(const std::string& fn);
    // write configfile to file
    void write_config_file(std::ostream& fd, bool shorter=false);


    // print fancy help/usage 
    void usage(std::ostream& ss);
};

class ConfigGroup {
  public:
    tOptionMap members;
    std::string name;
    ConfigManager* parent;

    // command -> id
    tStringMap _cmdmap;

    // iteration over ConfigGroup returns ConfigOptions
    typedef tOptionIter iterator;
    typedef tOptionCIter const_iterator;
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;

    // construct group by name and pass parent ConfigManager
    ConfigGroup(const std::string& name, ConfigManager* par);

    // return option by id
    ConfigOption& get_option(const std::string& id);

    // create new ConfigOption with
    // - id -> long cmdline name without "--"
    // - desc -> human readable description/short help text 
    // - short_cmd -> (optional) short cmdline with "-"
    template<class T>
    ConfigOption& new_option(const std::string& new_id, \
        const std::string& desc, const std::string& short_cmd="") {
      
      // catch duplicate IDs
      if(members.find(new_id) != members.end())
        throw OptionAlreadyExists(new_id);

      // create short and long-cmd match strings
      std::string scmd = "-" + short_cmd;
      std::string lcmd = "--" + new_id;

      // catch duplicate short cmdline option
      if(parent->cmdmap.find(scmd) != parent->cmdmap.end())
        throw ShortCommandAlreadyExists(scmd);

      // construct ConfigOption 
      members[new_id] = new ConfigOption(new_id, desc, typeid(T).name(), short_cmd);
      members[new_id]->parent = this;
      // save ConfigOption to ConfigGroup's members
      parent->members[new_id] = members[new_id];

      // quick-acces (local) command-map (long-cmd -> long-id)
      _cmdmap[lcmd] = new_id;
      // pass to parent command-map
      parent->cmdmap[lcmd] = new_id;
      
      // also allow quick-acces for short-cmd 
      if(short_cmd != "") {
        _cmdmap[scmd] = new_id;
        parent->cmdmap[scmd] = new_id;
      }
      // return created ConfigOption
      return *members[new_id];
    }
};
}

#endif
