#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <sstream>

#include "xstring.h"

#define NO_NEW_MACRO_DEFINE

#include "mem_tracker.h"

using namespace std;


/** actually define the global (extern-ed) variables here
 *  and do some initialization directly at the beginning of main() */
long CALL_COUNT_NEW;
long CALL_COUNT_DELETE;
long MEMORY_COUNT_NEW;
long MEMORY_COUNT_DELETE;
long MEMORY_NOT_JOURNALED;
bool USE_MEM_TRACKER;
tMemoryMap MEMORY_MAP;
tMemoryDataMap MEMORY_DATA_MAP;


void* __handle_new_request(size_t size, const char* fn, size_t line) _GLIBCXX_THROW(std::bad_alloc) {
  if(USE_MEM_TRACKER) {
    CALL_COUNT_NEW++;
    MEMORY_COUNT_NEW += size;
  }
  void* out = malloc(size);

  if(USE_MEM_TRACKER) {
    USE_MEM_TRACKER = false;
    MEMORY_MAP[out] = size;
    MEMORY_DATA_MAP[out] = make_pair(std::string(fn), line);
    USE_MEM_TRACKER = true;
  }
  return out;
}

/** Globally overloads the "new" (and "new[]") operator,
    USE_MEM_TRACKER can be false to disable the mechanism */
void* operator new(size_t size, const char* fn, size_t line) _GLIBCXX_THROW(std::bad_alloc) {
  return __handle_new_request(size, fn, line);
}

void* operator new[](size_t size, const char* fn, size_t line) _GLIBCXX_THROW(std::bad_alloc) {
  return __handle_new_request(size, fn, line);
}

/** Global "delete" operator overload */
void operator delete(void* ptr) _GLIBCXX_USE_NOEXCEPT {
  if(USE_MEM_TRACKER) {
    CALL_COUNT_DELETE++;
    tMemoryIter i = MEMORY_MAP.find(ptr);
    if(i != MEMORY_MAP.end()) {
      USE_MEM_TRACKER = false;
      MEMORY_COUNT_DELETE += MEMORY_MAP[ptr];
      MEMORY_MAP.erase(i);
      MEMORY_DATA_MAP.erase(i->first);
      USE_MEM_TRACKER = true;
    } else
      MEMORY_NOT_JOURNALED++;
  }
  free(ptr);
}

/** show some results and hints to search the leaks */
string show_memory_tracker_results() {
  long m_diff = (MEMORY_COUNT_NEW - MEMORY_COUNT_DELETE);
  long ptr_diff = CALL_COUNT_NEW - CALL_COUNT_DELETE;
  stringstream ss;
  ss << endl;
  ss << "[STATS] Showing new/delete statistics..." << endl;
  ss << "[i] Called new: " << CALL_COUNT_NEW << \
     " delete: " << CALL_COUNT_DELETE << endl;
  ss << "[i] Allocated memory: " <<  MEMORY_COUNT_NEW <<
     " Bytes freed memory: " << MEMORY_COUNT_DELETE << " Bytes" << endl;
  ss << "[i] (Minimum) leaked memory: " << m_diff <<
     " Bytes within: " << ptr_diff << " allocations." << endl;

  if(MEMORY_DATA_MAP.size() == 0)
    return ss.str();

  ss << endl;
  ss << "[LEAK DATA] showing not deleted pointers [size] and their file origin:" << endl;
  tMemoryDataBackMap pos2ptr;
  for(tMemoryDataIter i=MEMORY_DATA_MAP.begin(); i!=MEMORY_DATA_MAP.end(); ++i)
    pos2ptr[i->second].push_back(i->first);

  for(tMemoryDataBackIter i=pos2ptr.begin(); i!=pos2ptr.end(); ++i) {
    //for(tPointerIter& i : pos2ptr) {
    ss << "[E] " << i->first.first << " - line: " << i->first.second << " leaking ptrs: (#" << i->second.size() << ")" << endl;
    ss << "[E]    ";

    // find widest size (written size string length)
    //size_t max_size = *max_element(i->second.begin(), i->second.end());
    size_t max_size = 123456;
    size_t max_width = 1;
    while((max_size = max_size/10.0) > 0.0)
      max_width++;

    // pretty print leaked pointer[size]
    size_t col = 0;
    for(tPointerIter p=i->second.begin(); p!=i->second.end(); ++p, col++) {
      ss << *p << "[" << setw(max_width) << MEMORY_MAP[*p] << "]";
      ss << (((p+1) != i->second.end()) ? ", " : "\n");
      if(col%6 == 5)
        ss << endl << "       ";
    }
  }
  return ss.str();
}

/** To avoid a segfault on exit, if MEM_TRACKER is used.
 *  (Segfault due to the automated cleanup of std::map on leaving scope) */
void exit(int status) throw() {
  MEMORY_MAP.clear();
  MEMORY_DATA_MAP.clear();
  // abort() is not entirely correct here -
  // FIXME: How to call the "original" exit() after overriding it?
  // (Save a funtion pointer to the original function?)
  abort();
}

void init_memory_tracker() {
  CALL_COUNT_NEW = 0;
  CALL_COUNT_DELETE = 0;
  MEMORY_COUNT_NEW = 0;
  MEMORY_COUNT_DELETE = 0;
  MEMORY_NOT_JOURNALED = 0;

  // do not track MEMORY_MAP itself during ctor
  USE_MEM_TRACKER = false;
  MEMORY_MAP.clear();
  MEMORY_DATA_MAP.clear();

  // remain off
  //USE_MEM_TRACKER = true;
}




