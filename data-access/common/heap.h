#ifndef DATAACCESS_COMMON_HEAP_H
#define DATAACCESS_COMMON_HEAP_H
#include <vector>
#include <functional>
#include "glog/logging.h"

namespace common {

template<class T> class Heap {
    std::vector<T> data;
    std::function<int (T &, T &)> cmpfunc;
    
    int left (int i) {
        return i * 2;
    }
    int right (int i) {
        return i * 2 + 1;
    }
    int parent (int i) {
        return i/2;
    }

    void swap (int i, int j) {
        T tmp = data[i];
        data[i] = data[j];
        data[j] = tmp;
    }
    void Adjust(int i, size_t size);
    
public:
    Heap(std::function<int (T &, T &)> _cmpfunc): cmpfunc(_cmpfunc) {}
    Heap (const std::vector<T> &_data, std::function<int (T &, T &)> cmpfunc);
    ~Heap () {
        data.clear();
    }
    void Init(const std::vector<T> &_data);

    void Setop(T _data) {
        if (cmpfunc(_data, data[0]) > 0) {
            return;
        }
        data[0] = _data;
        Adjust(0, data.size());
    }
    T Getop () {
        return data[0];
    }

    void Show();
    void Sort();
    std::vector<T> GetData() {
        return data;
    }
};

};
#endif
