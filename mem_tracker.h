#ifndef MEM_TRACKER_H
#define MEM_TRACKER_H

#include <new>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>

/** Set to 1 to activate global memory leak tracker. Attention: activating
 *  this leads to a lot of overhead. Default is 0 -> off!
 *  prefereably activate the memory tracker during compiletime via 
 *  -DMEMORY_TRACKER_ACTIVE */
//#define MEMORY_TRACKER_ACTIVE
#ifndef MEMORY_TRACKER_ACTIVE
#define MEMORY_TRACKER_ACTIVE 0
#else
#define MEMORY_TRACKER_ACTIVE 0
#endif 

#if MEMORY_TRACKER_ACTIVE 

// allocation/deletion position (filename, lineno)
typedef std::pair<std::string, size_t> tMemOpPos;

// all ptr related data
class PtrData {
  public:
    PtrData();
    PtrData(const PtrData& obj);
    PtrData(void* ptr, const size_t& len);

    void* p;
    size_t size;

    bool allocated;
    bool deleted;

    tMemOpPos new_call;
    tMemOpPos delete_call;
};

// vector of PtrData
typedef std::vector<PtrData> tPtrDataList;
typedef tPtrDataList::iterator tPtrDataIter;

// associative storage for ptr -> PtrData
typedef std::unordered_map<void*, PtrData> tPtrDataStorage;
typedef tPtrDataStorage::iterator tPtrDataStorageIter;

// reverse associative storage tMemOpPos -> (PtrData1, PtrData2, ..., PtrDataN)
typedef std::map<tMemOpPos, tPtrDataList> tOpPtrDataMap;
typedef tOpPtrDataMap::iterator tOpPtrDataIter;

// pointer storage vars
extern tPtrDataStorage ALLOCATED_PTRS;
extern tPtrDataList ARCHIVED_PTRS;

/** Global counters for new/delete */
// caller count for tracked new/delete
extern unsigned long long CALL_COUNT_NEW;
extern unsigned long long CALL_COUNT_DELETE;

/** byte count for tracked new/delete */
extern unsigned long long MEMORY_COUNT_NEW;
extern unsigned long long MEMORY_COUNT_DELETE;

/** Is true, if new/delete journaling should be enabled! */
extern bool USE_MEM_TRACKER;

/** temporary variables for delete-call */
//extern const char* ____DELETE_FILENAME____;
//extern size_t ____DELETE_LINE____;

/**************************** FUNCTION PROTOTYPES *****************************/

/** overriding exit to avoid segfault, due to map-cleanup on exit() */
void exit(int status) throw();

/** overriding new to count occurences and allocated heap-memory */
//void* operator new(size_t size, const char* fn, size_t line) _GLIBCXX_THROW(std::bad_alloc);
//void* operator new[](size_t size, const char* fn, size_t line) _GLIBCXX_THROW(std::bad_alloc);
//void* operator new(size_t size) _GLIBCXX_THROW(std::bad_alloc);
//void* operator new[](size_t size) _GLIBCXX_THROW(std::bad_alloc);

/** overriding delete to be able to count lost/leaked pointers/memory */
//void operator delete(void* ptr) _GLIBCXX_USE_NOEXCEPT;
//void operator delete[](void* ptr) _GLIBCXX_USE_NOEXCEPT;

/** init memory-tracker variables */
void init_memory_tracker();

/** show results summary */
std::string get_memory_tracker_results(bool verbose=false);
std::string __print_memory_details(bool delete_mode, bool verbose=false);

/** backend function - handle new-call */
void* __handle_new_request(size_t size, const char* fn, const size_t line) _GLIBCXX_THROW(std::bad_alloc);
void __handle_delete_request(void* ptr, const char* fn, const size_t line);

/** Global "new" operator overload */
//void* operator new(size_t size, const char* fn, size_t line) _GLIBCXX_THROW(std::bad_alloc) {
inline void* operator new(size_t size) _GLIBCXX_THROW(std::bad_alloc) {
  return __handle_new_request(size, "no.cc", 123);
}
//void* operator new[](size_t size, const char* fn, size_t line) _GLIBCXX_THROW(std::bad_alloc) {
inline void* operator new[](size_t size) _GLIBCXX_THROW(std::bad_alloc) {
  return __handle_new_request(size, "no.cc", 123);
}

/** Global "delete" operator overload */
inline void operator delete(void* ptr) _GLIBCXX_USE_NOEXCEPT {
  __handle_delete_request(ptr, "del.cc", 312);
}
inline void operator delete[](void* ptr) _GLIBCXX_USE_NOEXCEPT {
  __handle_delete_request(ptr, "del.cc", 312);
}
/** backend function - handle delete-call */
//void __handle_delete_meta_data(const char* fn, const size_t& line);

// avoid definition of macros for mem_tracker.cc
/*#ifndef NO_ALLOC_MACRO_OVERRIDE
#define OVERRIDE_DELETE __handle_delete_meta_data(__FILE__, __LINE__); delete
#define delete OVERRIDE_DELETE

#define OVERRIDE_NEW new (__FILE__, __LINE__)
#define new OVERRIDE_NEW
#endif*/

#endif // if MEMORY_TRACKER_ACTIVE

#endif // if header already included
