#include <iostream>
#include <map>

#include "mem_tracker.h"

#include "testing_suite.h"

using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;
using namespace std;


class MemTrackerToolsTestSuite : public TestSuite {
  public:
    MemTrackerToolsTestSuite() {
      REG_TEST(&MemTrackerToolsTestSuite::test_init)
      REG_TEST(&MemTrackerToolsTestSuite::test_one_new_delete)
      REG_TEST(&MemTrackerToolsTestSuite::test_many_new_delete)
      REG_TEST(&MemTrackerToolsTestSuite::test_many_new_delete_leaking)
    }

    size_t many;

    virtual void setup() { 
      many = 3000;
    }

    virtual void tear_down() { }

    MAKE_TEST(init) {
      init_memory_tracker();

      CHECK(CALL_COUNT_NEW == 0);
      CHECK(CALL_COUNT_DELETE == 0);
      CHECK(MEMORY_COUNT_NEW == 0);
      CHECK(MEMORY_COUNT_DELETE == 0);
      CHECK(ALLOCATED_PTRS.size() == 0);
      CHECK(ARCHIVED_PTRS.size() == 0);
      CHECK(USE_MEM_TRACKER == false);
    };

    MAKE_TEST(one_new_delete) {
      PREPARE_WITH(init);
      USE_MEM_TRACKER = true;
      
      char *ptr = new char;
      *ptr = 'x';
      delete ptr;

      USE_MEM_TRACKER = false;
      CHECK(CALL_COUNT_NEW == 1);
      CHECK(CALL_COUNT_DELETE == 1);
      CHECK(MEMORY_COUNT_NEW == sizeof(char));
      CHECK(MEMORY_COUNT_DELETE == sizeof(char));
    }

    MAKE_TEST(many_new_delete) {
      PREPARE_WITH(init);
      USE_MEM_TRACKER = true;
      
      size_t mem_taken = 0;
      size_t mem_calls = 0;
      for(size_t i=0; i<many; ++i) {
        char *ptr = new char[i+1];
        mem_taken += i+1;
        mem_calls++;
        delete ptr;
      }

      USE_MEM_TRACKER = false;
      CHECK(CALL_COUNT_NEW == mem_calls);
      CHECK(CALL_COUNT_DELETE == mem_calls);
      CHECK(MEMORY_COUNT_NEW == mem_taken);
      CHECK(MEMORY_COUNT_DELETE == mem_taken);
      cout << get_memory_tracker_results() << endl;
    }

    MAKE_TEST(many_new_delete_leaking) {
      PREPARE_WITH(init);
      USE_MEM_TRACKER = true;
      
      size_t mem_freed = 0;
      size_t del_calls = 0;
      size_t mem_taken = 0;
      size_t mem_calls = 0;
      for(size_t i=0; i<many; ++i) {
        char* ptr = new char[i+1];
        char* ptr2 = new char[(i+3)*4];
        std::map<int, int>* complex_ptr = new std::map<int, int>;

        mem_taken += i+1 + (i+3)*4 + sizeof(std::map<int, int>);
        mem_calls += 3;
        if ((i%3) == 2 || (i*7)%11 == 2) {
          mem_freed += i+1 + (i+3)*4 + sizeof(std::map<int, int>);
          del_calls += 3;
          delete ptr;
          delete ptr2;
          delete complex_ptr;
        }  
      }
      USE_MEM_TRACKER = false;

      CHECK(CALL_COUNT_NEW == mem_calls);
      CHECK(CALL_COUNT_DELETE == del_calls);
      CHECK(MEMORY_COUNT_NEW == mem_taken);
      CHECK(MEMORY_COUNT_DELETE == mem_freed);

      cout << get_memory_tracker_results() << endl;
    }

};
