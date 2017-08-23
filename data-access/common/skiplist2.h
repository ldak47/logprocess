#ifndef DATAACCESS_COMMON_SKIPLIST_H
#define DATAACCESS_COMMON_SKIPLIST_H
#include <random>
#include <vector>
#include <sys/time.h>
#include "glog/logging.h"

#define MAX_LEVEL 10

namespace common {

/*
 * about forward:
 *  every skipnode no matter in any one level, 
 *  it has a forward point to every level.
 *  this is core of skiplist
 * */
struct SkiplistNode {
    time_t k;
    int v;
    SkiplistNode **forward;
};

class Skiplist {
    int level_;
    SkiplistNode *head_;

    SkiplistNode *MakeNode(int level);
    int GetInsertLevel();
    
public:
    Skiplist();
    ~Skiplist() {
        SkiplistNode *left = head_->forward[0];
        while (left && left->forward[0]) {
            SkiplistNode *next = left->forward[0];
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

    void Modify(const time_t k, const int v);
    void Remove(const time_t k);
    bool Search(const time_t k, int &v);
    bool RangeGet(const time_t st, const time_t ed, std::vector<int> &v);
    bool RangeCount(const time_t st, const time_t ed, size_t &cnt);
};
    

};
#endif
