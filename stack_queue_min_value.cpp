
#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include <string.h>
#include <sstream>
#include <stdio.h>
#include <iostream>

#include <algorithm>
#include <stdint.h>
#include <assert.h>
#include <algorithm>
#include <cctype>
#include <map>
#include <stack>


/* In a normal container which allow insert and remove, will do get_min() on O(n)
but if in stack, it can do on O(1) because if remove an element, we can use the last
min value and not to calculate again
*/

struct stack_t
{
  //std::deque<uint32_t> data_;
  // the deque is a list of vector container

  std::vector<uint32_t> data_;
  std::stack<uint32_t> min_index_;


  void clear()
  {
    data_.clear();
    for (; !min_index_.empty();)
    {
      min_index_.pop();
    }
  }

  bool empty()
  {
    return data_.empty();
  }

  void push(uint32_t v)
  {
    if (min_index_.empty())
    {
      min_index_.push(0);
    }
    else
    {
      uint32_t t;
      get_min(&t);
      if (v < t) {
        min_index_.push(data_.size());
      }
    }

    data_.push_back(v);
  }

  uint32_t top()
  {
    return data_.back();
  }

  void  pop()
  {
    data_.pop_back();
    if (data_.size() == min_index_.top()) {
      min_index_.pop();
    }
  }


  /* get the min value of the stack */
  int get_min(uint32_t * v)
  {
    if (data_.empty()) {
      return -1;
    }
    uint32_t t = min_index_.top();
    *v = data_[t];
    return 0;
  }

};


int test_stack_min1()
{
  stack_t v;
  uint32_t t;

  v.clear();

  v.push(3);
  v.get_min(&t);
  assert(t == 3);

  v.push(5);
  v.get_min(&t);
  assert(t == 3);
  v.push(1);
  v.get_min(&t);
  assert(t == 1);
  v.push(2);
  v.get_min(&t);
  assert(t == 1);


  v.pop();
  v.get_min(&t);
  assert(t == 1);
  v.pop();
  v.get_min(&t);
  assert(t == 3);

  v.pop();
  v.get_min(&t);
  assert(t == 3);

  v.pop();
  v.get_min(&t);

  return 0;
}

/* if the container is a queue, we only can do O(n) to get_min() */
struct queue_t
{
  std::vector<uint32_t> data_;
  std::queue<uint32_t> min_index_;


  void clear()
  {
    data_.clear();
    for (; !min_index_.empty();)
    {
      min_index_.pop();
    }
  }

  void push(uint32_t v)
  {
    data_.push_back(v);
  }

  uint32_t front()
  {
    return data_.front();
  }

  void pop()
  {
    data_.erase(data_.begin(), data_.begin() + 1);
  }


  /* get the min value of the queue */
  int get_min(uint32_t * out)
  {
    std::vector<uint32_t>::const_iterator it = std::min_element(data_.begin(), data_.end());
    if (it != data_.end()) {
      *out = *it;
      return 0;
    }
    return -1;
  }


};

int test_queue_min1()
{
  queue_t v;
  uint32_t t;

  v.clear();

  v.push(3);
  v.get_min(&t);
  assert(t == 3);

  v.push(5);
  v.get_min(&t);
  assert(t == 3);
  v.push(1);
  v.get_min(&t);
  assert(t == 1);
  v.push(2);
  v.get_min(&t);
  assert(t == 1);


  v.pop();
  v.get_min(&t);
  assert(t == 1);
  v.pop();
  v.get_min(&t);
  assert(t == 1);

  v.pop();
  v.get_min(&t);
  assert(t == 2);

  v.pop();
  v.get_min(&t);

  return 0;
}


int main(int argc, const char * argv[])
{

  /* compile on MSVC debug, if no assert, then all pass */
  test_stack_min1();
  test_queue_min1();

  return 0;
}