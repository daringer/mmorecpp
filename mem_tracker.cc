#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
//#include "atexit.h"
//#include "thread_private.h"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <sstream>

#include "xstring.h"

// NEED TWO MEM_TRACKER header files
// one for this .cc file 
// another one for the target source files
#define NO_ALLOC_MACRO_OVERRIDE
#include "mem_tracker.h"

using namespace std;

// fully remove MemoryTracker, if not needed
#if MEMORY_TRACKER_ACTIVE

/** actually define the global (extern-ed) variables here
 *  and do some initialization directly at the beginning of main() */
unsigned long long CALL_COUNT_NEW;
unsigned long long CALL_COUNT_DELETE;
unsigned long long MEMORY_COUNT_NEW;
unsigned long long MEMORY_COUNT_DELETE;

/** this flag activates the tracking */
bool USE_MEM_TRACKER;

/** temporary save variables provided during "delete" call " */
//const char* ____DELETE_FILENAME____;
//size_t ____DELETE_LINE____;

/** the two main "databases" for the pointers */
tPtrDataStorage ALLOCATED_PTRS;
tPtrDataList ARCHIVED_PTRS;

/** no args ctor */
PtrData::PtrData()
  : p(nullptr), size(0), allocated(false), deleted(false),
    new_call(), delete_call() { }

/** copy ctor */
PtrData::PtrData(const PtrData& obj)
  : p(obj.p), size(obj.size), allocated(obj.allocated), deleted(obj.deleted),
    new_call(obj.new_call), delete_call(obj.delete_call) { }

/** keeps all the data for one pointer */
PtrData::PtrData(void* ptr, const size_t& len)
  : p(ptr), size(len), allocated(true), deleted(false),
    new_call(), delete_call() { }

/** handle new-call */
void* __handle_new_request(size_t size, const char* fn, size_t line) _GLIBCXX_THROW(std::bad_alloc) {
  if(USE_MEM_TRACKER) {
    CALL_COUNT_NEW++;
    MEMORY_COUNT_NEW += size;
  }
  void* out = malloc(size);

  if(USE_MEM_TRACKER) {
    USE_MEM_TRACKER = false;
    ALLOCATED_PTRS[out] = PtrData(out, size);
    ALLOCATED_PTRS[out].new_call = make_pair(string(fn), line);
    USE_MEM_TRACKER = true;
  }
  return out;
}

/** handle delete-call */
void __handle_delete_request(void* ptr, const char* fn, size_t line) {
  if(USE_MEM_TRACKER) {
    tPtrDataStorageIter i = ALLOCATED_PTRS.find(ptr);
    if(i != ALLOCATED_PTRS.end()) {
      USE_MEM_TRACKER = false;

      // move PtrData instance from ALLOCATED to ARCHIVED
      ARCHIVED_PTRS.push_back(ALLOCATED_PTRS[ptr]);
      ALLOCATED_PTRS.erase(ptr);

      ARCHIVED_PTRS.back().deleted = true;
      ARCHIVED_PTRS.back().delete_call = make_pair(string(fn), line);

      CALL_COUNT_DELETE++;
      MEMORY_COUNT_DELETE += ARCHIVED_PTRS.back().size;

      USE_MEM_TRACKER = true;
    }
  }
  free(ptr);
}



/** return delete or new statistics */
string __print_memory_details(bool delete_mode, bool verbose) {
  stringstream ss;
  tOpPtrDataMap pos2ptr;
  tPtrDataList data;

  // generate reverse table
  if(!delete_mode) {
    for(tPtrDataStorage::value_type& i : ALLOCATED_PTRS) {
      pos2ptr[i.second.new_call].push_back(i.second);
      data.push_back(i.second);
    }
  } else {
    for(PtrData& i : ARCHIVED_PTRS) {
      pos2ptr[i.delete_call].push_back(i);
      data.push_back(i);
    }
  }

  // directly return, if no details are available
  if(data.size() == 0)
    return "";

  // find PtrData instance allocating most bytes
  tPtrDataIter max_iter = std::max_element(data.begin(), data.end(), \
                          [](const PtrData& a, const PtrData& b) \ 
  { return (a.size <= b.size); } \
                                          );

  // calc/set formatting vars
  size_t max_size = max_iter->size;
  size_t max_width = 0;
  size_t max_cols = 6;
  while(max_size) {
    max_size /= 10.0;
    max_width++;
  }

  // go over generated map and gen. results
  for(tOpPtrDataIter i=pos2ptr.begin(); i!=pos2ptr.end(); ++i) {
    // calculate sum of bytes alloced/deleted
    unsigned long long sum = 0;
    std::for_each(i->second.begin(), i->second.end(), \
    [&](const PtrData& x) {
      sum += x.size;
    }
                 );

    ss << "[i] " << setw(5) << i->first.first << setw(10) <<
       " line: " << setw(6) << i->first.second << setw(10) <<
       "#calls: " << setw(8) << i->second.size() << setw(10) <<
       " bytes: " << setw(16) << sum << endl;

    // print: ptr-addr[size] x (max_cols) each line
    size_t col = 0;
    if(verbose) {
      ss << "[E]    ";
      for(tPtrDataIter p=i->second.begin(); p!=i->second.end(); ++p, col++) {
        ss << p->p << "[" << setw(max_width) << p->size << "]";
        ss << (((p+1) != i->second.end()) ? ", " : "\n");
        if((col % max_cols) == (max_cols - 1))
          ss << endl << "       ";
      }
    }
  }
  return ss.str();
}

/** show some results and hints to search the leaks */
string get_memory_tracker_results(bool verbose) {
  long m_diff = (MEMORY_COUNT_NEW - MEMORY_COUNT_DELETE);
  long ptr_diff = CALL_COUNT_NEW - CALL_COUNT_DELETE;
  stringstream ss;

  ss << endl;
  ss << "[STATS] Memory tracking overview:" << endl;
  ss << endl;
  string sep = " | ";
  size_t pad = 14;
  size_t loff = 8;
  size_t hline = 82;
  ss << setw(pad+loff) << "tracked" << sep << setw(pad) << "tracked" << sep <<
     setw(pad) << "leaked" << sep << setw(pad) << "leaked" << sep << endl;

  ss << setw(pad+loff) << "calls" << sep << setw(pad) << "bytes" << sep <<
     setw(pad) << "calls" << sep << setw(pad) << "bytes" << sep << endl;

  ss << setw(hline) << setfill('-') << "" << setfill(' ') << endl;

  ss << setw(loff) << "NEW" << setw(pad) << CALL_COUNT_NEW << sep <<
     setw(pad) << MEMORY_COUNT_NEW << sep << setw(pad) << "n/a" << sep <<
     setw(pad) << "n/a" << sep << endl;

  ss << setw(hline) << setfill('-') << "" << setfill(' ') << endl;

  ss << setw(loff) << "DELETE" << setw(pad) << CALL_COUNT_DELETE << sep <<
     setw(pad) << MEMORY_COUNT_DELETE << sep << setw(pad) << ptr_diff << sep <<
     setw(pad) << m_diff << sep << endl;

  if(m_diff > 0) {
    ss << endl;
    ss << "[LEAK DATA] showing not deleted (bad) calls:" << endl;
    ss << __print_memory_details(false, verbose);
  } else
    cout << "[+] no leaks found!" << endl;

  ss << endl;

  if(verbose) {
    if(CALL_COUNT_DELETE > 0) {
      ss << "[DELETE DATA] showing deleted (good) calls:" << endl;
      ss << __print_memory_details(true, verbose);
      ss << endl;
    } else
      cout << "[i] no delete calls tracked..." << endl;
  }

  return ss.str();
}

// saving pointer to original ::exit() function call
auto original_exit = &::exit;

/** To avoid a segfault on exit, if MEM_TRACKER is used.
 *  (Segfault due to the automated cleanup of MemTracker datastructures on leaving scope) */
void exit(int status) throw() {
  ALLOCATED_PTRS.clear();
  ARCHIVED_PTRS.clear();
  // calling "real" exit()
<<<<<<< HEAD
  original_exit(status);
=======
  _exit(status);
>>>>>>> 40f2e8892616ce4d4ce6225c713a6e6c39293b2d
}

/** initilize the memory tracker variables */
void init_memory_tracker() {
  CALL_COUNT_NEW = 0;
  CALL_COUNT_DELETE = 0;
  MEMORY_COUNT_NEW = 0;
  MEMORY_COUNT_DELETE = 0;

  // set to true to start tracking!
  USE_MEM_TRACKER = false;

  ALLOCATED_PTRS.clear();
  ARCHIVED_PTRS.clear();
}

#endif // MEMORY_TRACKER_ACTIVE
