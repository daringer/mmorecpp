#include <unistd.h>

#include <vector>
#include <iterator>

template <typename T, int max_slots, typename C = std::vector<T>>
class CircularBuffer {
 public:
  // typical (value centric) STL-aliases
  typedef T value_type;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef unsigned int size_type;

 private:
  // this is me-and-myself, STL-style baby
  typedef CircularBuffer<T, max_slots, C> class_type;

  // forbid copy-contruction (as inside private)
  CircularBuffer(const class_type& cls);
  class_type& operator=(const class_type& cls);

  // underlying-item-container and ptr-vars
  C items;

  // maximum capacity
  const size_type max_items;

 public:
  // create CircularBuffer directly with its max slot count
  CircularBuffer() : max_items(max_slots) { }

  // is empty ?
  bool empty() const {
    return items.empty();
  }
  // return container item count
  size_type size() const {
    return items.size();
  }
  // return maximum capacity
  size_type capacity() const {
    return max_items;
  }

  // front item inspect (const & non-const)
  reference front() {
    return items.front();
  }
  const_reference front() const {
    return items.front();
  }
  
  // pop and throw away front item 
  void pop() {
    if (!empty())
      items.erase(items.begin());
  }
  
  // back item inspect 
  reference back() {
    return items.back();
  }
  const_reference back() const {
    return items.back(); 
  }

  // push new item at back and maintain circular property 
  void push(const value_type& item) {
    if(items.size() == max_items)
      pop();
    items.push_back(item);
  }
};
