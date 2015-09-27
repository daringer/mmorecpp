#pragma once

#ifndef MM_CUSTOM_NAMESPACE
#define MM_CUSTOM_NAMESPACE TOOLS
#endif

// macros for namespace name creation
#define MM_NAMESPACE(other) MM_CUSTOM_NAMESPACE ## other

#ifndef MM_NO_VECTOR 
#include <vector>
#endif 

#ifndef MM_NO_MAP
#include <map>
#endif 

#ifndef MM_NO_SET
#include <set>
#endif 

#ifndef MM_NO_SET
#include <string>
#endif 

#ifndef MM_NO_IOSTREAM
#include <iostream>
#endif 

#ifndef MM_NO_EXCEPTION
#include <exception>
#endif 


#include <cstdlib>

/** some macro hacks to provide a more-or-less default replaceable allocator */
// replace the DEF_ALLOCATOR macro with an allocator of choice
#define DEF_ALLOCATOR(item_type) std::allocator<item_type>
// use these for container definitions
#define MMVector(item_type) std::vector<item_type, DEF_ALLOCATOR(item_type)>
#define MMMap(item_type) std::map<item_type, DEF_ALLOCATOR(item_type)>
#define MMSet(item_type) std::set<item_type, DEF_ALLOCATOR(item_type)>

/** if we need default-arguments for preprocessor macros: */
// varargs handling macros
#define VARGS_(_10, _9, _8, _7, _6, _5, _4, _3, _2, _1, N, ...) N 
#define VARGS(...) VARGS_(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
// concatenation
#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)
// finally called macro
#define MACRO_2(a, b) my_fancy_function(a, b) 
// provide default argument
#define MACRO_1(a) MACRO_2(a, "my fancy default") 
// put (concat) all together (only macro used by the user!)
#define MACRO(...) CONCAT(MACRO_, VARGS(__VA_ARGS__))(__VA_ARGS__) 
/** working uglyness */



namespace MM_NAMESPACE() {

const int MAX_LISTEN_QUEUE = 10;
const int BUF_LEN = 1024;
const int MAX_GROUP_COUNT = 10;

enum SOCKET_TYPE {
  UNIX,
  TCP,
  UDP
};

typedef unsigned int uint;


#ifndef MM_NO_VECTOR
typedef std::vector<std::string> tStringList;
typedef tStringList::iterator tStringIter;

typedef std::vector<int> tIntList;
typedef tIntList::iterator tIntIter;

typedef std::vector<long> tLongList;
#endif 

#ifndef MM_NO_MAP
typedef std::map<std::string, std::string> tStringMap;
typedef tStringMap::iterator tStringMapIter;
#endif

#ifndef MM_NO_SET
typedef std::set<std::string> tStringSet;
#endif




}



