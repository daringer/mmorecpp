#ifndef XMEMORY_H
#define XMEMORY_H

namespace TOOLS {

// globally overload new and delete ???
// NO! better: provide macros, which usefully
// mimic the behavior of new/delete and a also
// do some fancy stuff:
// - auto-bzero anything new
// - auto-NULL anything new
// - save line/file and variablename of allocated variables
//   - this could lead to a very basic garbage collector
//     (not really without a special pointer)
// - well why not a reference-counting pointer? isn't there one already?
// - nothing automagic, but stuff that can be manually started/used

#include <typeinfo>

#include "exception.h"

DEFINE_EXCEPTION(PointerNotFound, BaseException);

#define Xnew \
   

typedef std::map<std::string, std::map<void*, uint> > tRefMap;
class MemMan {
  public:
    template<class T>
    static void add(const Xptr<T>& obj) {
      add(obj.ptr);
    }

    template<class T>
    static void add(T* ptr) {
      std::string t = typeid(T);
      if(ref_map[t].find(ptr) == ref_map[t].end())
        ref_map[t][ptr] = 1;
      else
        ref_map[t][ptr]++;
    }

    template<class T>
    static void del(Xptr<T>& obj) {
      del(obj.ptr);
    }

    template<class T>
    static void del(T* ptr) {
      std::string t = typeid(T);
      if(ref_map[t].find(ptr) == ref_map[t].end())
        throw PointerNotFound(ptr);
      ref_map[t][ptr]--;
    }

  private:
    static tRefMap ref_map;
};
static tRefMap MemMan::ref_map;

/*template<class T>
class BuildWrapper {
  public:
    BuildWrapper() {}
    ~BuildWrapper() {

    }
}*/

template<class T>
class XPtr {
  public:
    explicit XPtr(T* p) {
      MemMan::add(p);
    }
    XPtr(const XPtr& obj) {
      ptr = obj.ptr;
      MemMan::add(obj.ptr);
    }
    /*XPtr() {
      MemMan::add(ptr);
    }*/
    ~XPtr() {
      MemMan::del(ptr);
    }
    XPtr& operator=(const XPtr& rhs) {
      if(this == &rhs)
        return *ptr;
      else {
        ptr = obj.ptr;
        return MemMan::add(rhs.ptr);
      }
    }
    XPtr& operator=(const XPtr* rhs) {
      return this->operator=(*rhs);
    }
    const T& operator*() const {
      return *ptr;
    }
    const T* operator->() const {
      return ptr;
    }
    T& operator*() {
      return *ptr;
    }
    T* operator->() {
      return ptr;
    }
    operator void* () const;
    {
      return ptr;
    }
    operator T* () const {
      return ptr;
    }

  protected:
    T* ptr;
};

}

#endif
