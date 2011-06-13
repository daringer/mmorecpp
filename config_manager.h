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
  enum CONFIG_MANAGER_FLAGS {
    OCCURE_ONCE =       1,
    OCCURE_MULTI =      2,
    OCCURE_POSITIONAL = 4,
    REQUIRED =          8
  };

  class NoSuchConfigItemException : public BaseException {
    public:
      NoSuchConfigItemException(const std::string& msg);
  };
  
  class ConfigItemAlreadyExists : public BaseException {
    public:
      ConfigItemAlreadyExists(const std::string& msg);
  };

  class RequiredConfigItemNotSet : public BaseException {
    public:
      RequiredConfigItemNotSet(const std::string& msg);
  };

  class ConfigItem {
    public:
      ConfigItem(const std::string& name, const std::string& desc, 
                 const std::string& lcmd, const std::string& scmd,
                 const std::string& itype);

      void set_flags(int value);

      std::string name, desc;
      std::string item_type;
      std::string cmd_long, cmd_short;
      int flags;
  };
  
  class ConfigManager {
    public:
      typedef std::map<std::string, ConfigItem*> tConfigItemMap;
      typedef tConfigItemMap::iterator tConfigItemIter;

      typedef std::vector<void*> tVoidVector;
      typedef std::map<ConfigItem*, tVoidVector> tConfigData;

      std::string meta_name;
      std::string meta_desc;
      std::string command;

      tConfigItemMap config;
      tConfigData data;
       
      ConfigManager(const std::string& name, const std::string& desc);
      virtual ~ConfigManager();
      
      int parse_item(const std::string& key, const std::string& val, bool cmdline=true);
      void parse_cmdline(int argc, char *argv[]);
      void load_config_file(const std::string& fn);
      void usage(std::ostream& ss);
      tConfigData::size_type count_data_items(const std::string& name);

      template<class T>
      void register_config_item(const std::string& name, 
          const std::string& desc, const std::string& lcmd, 
          const std::string& scmd, const T& initial,
          int flags=OCCURE_ONCE) {

        register_config_item<T>(name, desc, lcmd, scmd, flags);
        set_data<T>(name, initial);
      }

      template<class T>
      void register_config_item(const std::string& name, 
          const std::string& desc, const std::string& lcmd, 
          const std::string& scmd,
          int flags=OCCURE_ONCE) {
  
        if(config.find(name) != config.end())
          throw ConfigItemAlreadyExists("The ConfigItem with name: " + name + " was already created");

        config[name] = new ConfigItem(name, desc, lcmd, scmd, typeid(T).name());
        config[name]->set_flags(flags);
      }
      
      template<class T>
      T get_data(const std::string& name, tConfigData::size_type idx=0) {
        if(config.find(name) == config.end())
          throw NoSuchConfigItemException("Your choosen name: '" + \
              name + "' is not available!");

        if(idx > data[config[name]].size()-1)
          throw NoSuchConfigItemException("Index for '" + name + \
            "' does not exist"); 
        
        return *( (T*) data[config[name]][idx]);
      };

      template<class T>
      void set_data(const std::string& name, const T& value) {
        T* ptr = new T;
        *ptr = value;

        if(config[name]->flags & OCCURE_ONCE) 
          data[config[name]].clear();

        data[config[name]].push_back(ptr);
      };
  };
}


#endif
