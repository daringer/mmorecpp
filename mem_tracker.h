#ifndef MEM_TRACKER_H
#define MEM_TRACKER_H


#include <new>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>

typedef std::unordered_map<void*, size_t> tMemoryMap;
typedef tMemoryMap::iterator tMemoryIter;

typedef std::pair<std::string, size_t> tMemoryAllocPos;

typedef std::vector<void*> tPointerList;
typedef tPointerList::iterator tPointerIter;

typedef std::unordered_map<void*, tMemoryAllocPos> tMemoryDataMap;
typedef tMemoryDataMap::iterator tMemoryDataIter;

typedef std::map<tMemoryAllocPos, tPointerList> tMemoryDataBackMap;
typedef tMemoryDataBackMap::iterator tMemoryDataBackIter;

typedef std::unordered_map<void*, size_t> tMemorySizeMap;
typedef tMemorySizeMap::iterator tMemorySizeIter;

/** Global counters for new/delete */
// caller count for new/delete
extern long CALL_COUNT_NEW;
extern long CALL_COUNT_DELETE;

/** amount of memory passed from/to new/delete */
extern long MEMORY_COUNT_NEW;
extern long MEMORY_COUNT_DELETE;

/** delete calls to untracked ptrs */
extern long MEMORY_NOT_JOURNALED;

/** Is true, if new/delete journaling should be enabled! */
extern bool USE_MEM_TRACKER;

////////// saving various data for each allocation by "new" //////////
/** void* -> (malloc-size) */
extern tMemoryMap MEMORY_MAP;
/** void* -> (filename, linenr) */
extern tMemoryDataMap MEMORY_DATA_MAP;

/************ FUNCTION PROTOTYPES ************/

/** overriding exit to avoid segfault, due to map-cleanup on exit() */
void exit(int status) throw();

/** overriding new to count occurences and allocated heap-memory */
void* operator new(size_t size, const char* fn, size_t line) _GLIBCXX_THROW(std::bad_alloc);
void* operator new[](size_t size, const char* fn, size_t line) _GLIBCXX_THROW(std::bad_alloc);

/** overriding delete to be able to count lost/leaked pointers/memory */
void operator delete(void* ptr, size_t size) _GLIBCXX_USE_NOEXCEPT;

/** init memory-tracker variables */
void init_memory_tracker();

/** show results summary */
std::string show_memory_tracker_results();

/** backend function, which handles new-calls */
void* __handle_new_request(size_t size, const char* fn, size_t line) _GLIBCXX_THROW(std::bad_alloc);


#endif

#ifndef NO_NEW_MACRO_DEFINE

#define DEBUG_NEW new (__FILE__, __LINE__)
#define new DEBUG_NEW

#endif
