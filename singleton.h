// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

/* posix singleton  */

#ifndef MUDUO_BASE_SINGLETON_H
#define MUDUO_BASE_SINGLETON_H

#include <assert.h>
#include <stdlib.h> // atexit

namespace base
{

template<typename T>
class singleton_t
{
    singleton_t(const singleton_t &);
    singleton_t &operator = (const singleton_t &);
    singleton_t();
    ~singleton_t();

    static pthread_once_t ponce_;
    static T*             value_;

    static void init()
    {
      if(NULL == value_){
        value_ = new(std::nothrow) T();
        ::atexit(destroy);
      }
    }

    static void destroy()
    {
      typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
      T_must_be_complete_type dummy; (void) dummy;
      delete value_;
      value_ = NULL;
    }

public:
  static T& instance()
  {
    pthread_once(&ponce_, &singleton_t::init);
    assert(value_ != NULL);
    return *value_;
  }

};

template<typename T>
T* singleton_t<T>::value_ = NULL;
template<typename T>
pthread_once_t singleton_t<T>::ponce_ = PTHREAD_ONCE_INIT;
}
#endif
