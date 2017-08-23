#ifndef DATAACCESS_COMMON_SKIPLIST_H
#define DATAACCESS_COMMON_SKIPLIST_H
#include <random>
#include <vector>
#include "glog/logging.h"

#define MAX_LEVEL 10

namespace common {

/*
 * about forward:
 *  every skipnode no matter in any one level, 
 *  it has a forward point to every level.
 *  this is core of skiplist
 * */
template<class KEY, class VALUE> struct SkiplistNode {
    KEY k;
    VALUE v;
    SkiplistNode **forward;
};

template<class KEY, class VALUE> class Skiplist {
    int level_;
    SkiplistNode<KEY, VALUE> *head_;

    SkiplistNode<KEY, VALUE> *MakeNode(int level);
    int GetInsertLevel();
    
public:
    Skiplist();
    ~Skiplist() {
        SkiplistNode<KEY, VALUE> *left = head_->forward[0];
        while (left && left->forward[0]) {
            SkiplistNode<KEY, VALUE> *next = left->forward[0];
            delete[] left->forward;
            delete left;
            left = nullptr;
            left = next;
        }
        if (left) {
            delete[] left->forward;
            delete left;
            left = nullptr;
        }
        delete[] head_->forward;
        delete head_;
        head_ = nullptr;
    }

    void Modify(const KEY k, const VALUE v);
    void Remove(const KEY k);
    bool Search(const KEY k, VALUE &v);
    bool RangeGet(const KEY st, const KEY ed, std::vector<VALUE> &v);
    bool RangeCount(const KEY st, const KEY ed, size_t &cnt);
};
    

};
#endif
