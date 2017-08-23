#include "heap.h"
#include <iostream>

namespace common {

template<class T> Heap<T>::Heap (const std::vector<T> &_data, std::function<int (T &, T &)> _cmpfunc): cmpfunc(_cmpfunc) {
    data.clear();
    for (auto i: _data) {
        data.push_back(i);
    }

    for (int i = data.size()/2; i >= 0; i--) {
        Adjust(i, data.size());
    }
}

template<class T> void Heap<T>::Init (const std::vector<T> &_data) {
    data.clear();
    for (auto i: _data) {
        data.push_back(i);
    }

    for (int i = data.size()/2; i >= 0; i--) {
        Adjust(i, data.size());
    }
}
    
template<class T> void Heap<T>::Adjust (int i, size_t size) {
    if (i < size/2) {
        T cur = data[i];
        int max = i;
        if (left(i) < size && cmpfunc(data[max], data[left(i)]) == 0) {
            max = left(i);
        }

        if (right(i) < size && cmpfunc(data[max], data[right(i)]) == 0) {
            max = right(i);
        }

        if (max != i) {
            swap(max, i);
            Adjust(max, size);
        }
    }
}

template<class T> void Heap<T>::Sort () {
    for (int i = data.size() - 1; i > 0; i--) {
        swap(0, i);
        Adjust(0, i);
    }
}

template<class T> void Heap<T>::Show () {
    for (auto i: data) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}
    
};
