#ifndef DATAACCESS_COMMON_SINGLETON_H
#define DATAACCESS_COMMON_SINGLETON_H
#include "noncopyable.h"
#include <pthread.h>
//#include <iostream>

namespace common {

class deleter {
public:
    template<class T> void operator()(T *p) {
        //std::cout << "delete " << p << std::endl;
        delete p;
    }
};
    
template<class T> class Singleton : public noncopyable {
    //use static std::unique_ptr, because os will call desstruct function of all static variables when prccess complete, and unique_ptr will be destruct itself automaticlly. notice the class deleter is not necessary, but only for explicitly see.
    static std::unique_ptr<T, deleter> instance_;
    static pthread_once_t _p_once;
    
    //called from get(). notice that: even if many threads call get to get a singleton, only one thread could call here because of pthread_once.
    static void _new() {
        if (!instance_.get()) {
            instance_.reset(new T());
        }
    }
    Singleton(){}
    ~Singleton(){}

public:
    static T *get() {
        pthread_once(&_p_once, &Singleton::_new);
        return instance_.get();
    }
};

template<class T> pthread_once_t Singleton<T>::_p_once = PTHREAD_ONCE_INIT;
template<class T> std::unique_ptr<T, deleter> Singleton<T>::instance_;
};
#endif
