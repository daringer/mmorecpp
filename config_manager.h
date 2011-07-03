#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <typeinfo>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "exception.h"
#include "xstring.h"

#include "general.h"

namespace TOOLS {
/*enum CONFIG_MANAGER_FLAGS {
  CF_ONCE =       1,
  CF_MULTI =      2,
  CF_POSITIONAL = 4,
  CF_REQUIRED =   8,
  CF_EXCLUSIVE = 16,
  CF_ONEOF =     32,
};*/

class ConfigManagerException : public BaseException {
  public:
    ConfigManagerException(const std::string& msg, const std::string& exc_name);
    ConfigManagerException(const ConfigManagerException& obj);
};

class UnknownParameter : public ConfigManagerException {
  public:
    UnknownParameter(const std::string& msg);
};

class ConfigItemAlreadyExists : public ConfigManagerException {
  public:
    ConfigItemAlreadyExists(const std::string& msg);
};

class ShortCommandAlreadyExists : public ConfigManagerException {
  public:
    ShortCommandAlreadyExists(const std::string& msg);
};

class RequiredConfigItemNotSet : public ConfigManagerException {
  public:
    RequiredConfigItemNotSet(const std::string& msg);
};

class ConfigNotFound : public ConfigManagerException {
  public:
    ConfigNotFound(const std::string& msg);
};

class IncompatibleDataTypes : public ConfigManagerException {
  public:
    IncompatibleDataTypes(const std::string& msg);
};

template<class T>
class ConfigItem {
  public:
    ConfigItem(const std::string& id, 
               const std::string& desc, const std::string& scmd="") 
      : id(id), cmd_short(scmd), desc(desc), //flags(CF_ONCE), 
      data_type(typeid(T).name()), empty(true)  {}

    /*ConfigItem<T>& set_flags(const int value) {
      flags = value;
      return *this;
    }*/
    
    ConfigItem<T>& set_default(const T& value) {
      initial = value;
      if(empty)
        data = value;
      return *this;
    }
    
    ConfigItem<T>& set(const T& value) {
      data = value;
      empty = false;
      return *this;
    }

    // id is also used as long cmd
    std::string id; 
    std::string cmd_short;
    std::string desc;
    //int flags;
    T data;
  private:
    T initial;
    std::string data_type;
    bool empty;
};

class ConfigManager;

typedef std::map<std::string, std::string> tStringMap;
typedef tStringMap::iterator tStringMapIter;

class ConfigGroup {
  public:
    typedef std::map<std::string, void*> tItemList;
    typedef tItemList::iterator tItemIter;
    
    std::string name;
    ConfigManager* parent;
    tItemList members;
 
    // command -> id   
    tStringMap _cmdmap;
    // id -> typeid
    tStringMap _data_types;
    // id -> desc
    tStringMap _usagemap;

    ConfigGroup(const std::string& name, ConfigManager* par);

    template<class T>
    ConfigItem<T>& new_option(const std::string& new_id, const std::string& desc,
                              const std::string& scmd="") {
      // check if id is free - TODO
      // check if short-cmd is free - TODO
      ConfigItem<T>* save = new ConfigItem<T>(new_id, desc, scmd);
      members[new_id] = (void*) save;
      _cmdmap["--" + new_id] = new_id;
      if(scmd != "")
        _cmdmap["-" + scmd] = new_id;
      _data_types[new_id] = typeid(T).name();
      _usagemap[new_id] = desc;
      //std::cout << "_cmdmap size: " << _cmdmap.size() << std::endl;
      //std::cout << "_dt size: " << _data_types.size() << std::endl;
      return *save;
    }

    template<class T>
    T& get(const std::string& id) {
      return get_config<T>(id).data;
    }

    template<class T>
    ConfigItem<T>& get_config(const std::string& id) {
      return *((ConfigItem<T>*) members[id]);
    }

    template<class T>
    void set(const std::string& id, const T& new_data) {
      get_config<T>(id).set(new_data);
    }
};

class ConfigManager {
  public:
    typedef std::vector<ConfigGroup*> tGroupList;
    typedef tGroupList::iterator tGroupIter;
    //typedef std::map<std::string, ConfigGroup*> tIdGroupMap;

    std::string command;
    tStringMap cmdmap;
    tStringMap data_types;
    tStringMap usagemap;
    tGroupList groups;

    virtual ~ConfigManager();

    template<class T>
    ConfigItem<T>& get_config(const std::string& id) {
      for(tGroupIter i=groups.begin(); i!=groups.end(); ++i) {
        if((*i)->_cmdmap.find("--" + id) == (*i)->_cmdmap.end()) 
          continue;
        return (*i)->get_config<T>(id);
      }
      throw ConfigNotFound(id);
    }
    
    template<class T>
    T& get(const std::string& id) {
      return get_config<T>(id).data;
    }

    template<class T>
    void set(const std::string& id, const T& new_data) {
      get_config<T>(id).set(new_data);
    }
  
    void build_maps();
    ConfigGroup* new_group(const std::string& name);

    int parse(int pos, int max, char* argv[]);
    void parse_cmdline(int argc, char* argv[]);
/*    
    void load_config_file(const std::string& fn);
    */
    void usage(std::ostream& ss);
    
};
}


#endif
