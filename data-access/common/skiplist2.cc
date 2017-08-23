#include "skiplist2.h"
#include <assert.h>

namespace common {

SkiplistNode *Skiplist::MakeNode (int level) {
    SkiplistNode *newnode = new SkiplistNode();
    newnode->forward = new SkiplistNode *[MAX_LEVEL];
    //initial forward point of all levels as nullptr, for this newnode.
    for (int i = 0; i < level; i++) {
        newnode->forward[i] = nullptr;
    }
    
    return newnode;
}

int Skiplist::GetInsertLevel () {
    int res = 0;
    while (1) {
        std::random_device rd;
        double p = double(rd() % 100)/double(100);
        if (p < 0.5) {
            ++res;
        } else {
            break;
        }

        if (MAX_LEVEL - 1 <= res) {
            break;
        }
    }

    if (res >= (MAX_LEVEL - 1)) {
        res = (MAX_LEVEL - 1);
    }

    return res;
}
    
Skiplist::Skiplist ():level_(0) {
    head_ = MakeNode(MAX_LEVEL);
    assert(head_);
}

void Skiplist::Modify (const time_t k, const int v) {
    if (!head_) {
        return;
    }
    
    //find the 'insert point' for this k, from high level to low level.
    SkiplistNode *forward_point_array[MAX_LEVEL];
    SkiplistNode *vertical = head_, *horizontal;
    for (int i = level_; i >= 0; i--) {
        //vertical. get the forward point at level i of res.
        horizontal = vertical->forward[i];
        while (horizontal && horizontal->k < k) {
            //horizontal. if fw is not nullptr and fw->k is lower than k, go on forward at this level.
            vertical = horizontal;
            horizontal = horizontal->forward[i];
        }

        //res is the result at level i
        forward_point_array[i] = vertical;
    }

    //now, get the biggest node whose's k lower than k, in every level, forward_point_array. horizontal here is forward_point_array[0], in lowest level.
    //if find node with equally k, update.
    if (horizontal && horizontal->k == k) {
        horizontal->v = v;
        return;
    }

    //random calc the highest level to insert. if bigger than current level, should update level and fill the forward_point_array.
    int insert_level = GetInsertLevel();
    if (insert_level > level_) {
        while (insert_level > level_) {
            ++level_;
            forward_point_array[level_] = head_;
        }
    }

    //create a new node and set k and v.
    SkiplistNode *newnode = MakeNode(insert_level);
    newnode->k = k;
    newnode->v = v;

    //update the list of every level.
    for (int i = 0; i <= insert_level; i++) {
        newnode->forward[i] = forward_point_array[i]->forward[i];
        forward_point_array[i]->forward[i] = newnode;
    }
}

void Skiplist::Remove (const time_t k) {
    if (!head_) {
        return;
    }
    SkiplistNode *vertical = head_, *horizontal;
    
    SkiplistNode *forward_point_array[MAX_LEVEL];
    for (int i = level_; i >= 0; i--) {
        horizontal = vertical->forward[i];
        while (horizontal && horizontal->k < k) {
            vertical = horizontal;
            horizontal = horizontal->forward[i];
        }

        forward_point_array[i] = vertical;
    }

    if (horizontal && horizontal->k == k) {
        for (int i = 0; i <= level_ && forward_point_array[i]->forward[i] == horizontal; i++) {
            forward_point_array[i]->forward[i] = horizontal->forward[i];
        }

        delete[] horizontal->forward;
        delete horizontal;
        horizontal = nullptr;

        while (head_->forward[level_] == nullptr && level_) {
            --level_;
        }
    }
}

bool Skiplist::Search (const time_t k, int &v) {
    if (!head_) {
        return false;
    }

    SkiplistNode *vertical = head_, *horizontal;
    for (int i = level_; i >= 0; i--) {
        horizontal = vertical->forward[i];
        while (horizontal && horizontal->k < k) {
            vertical = horizontal;
            horizontal = horizontal->forward[i];
        }
    }

    if (horizontal && horizontal->k == k) {
        v = horizontal->v;
        return true;
    }

    return false;
}

bool Skiplist::RangeGet (const time_t st, const time_t ed, std::vector<int> &v) {
    if (!head_) {
        return false;
    }
    v.clear();

    SkiplistNode *vertical = head_, *horizontal;
    for (int i = level_; i >= 0; i--) {
        horizontal = vertical->forward[i];
        while (horizontal && horizontal->k < st) {
            vertical = horizontal;
            horizontal = horizontal->forward[i];
        }
    }

    if (horizontal) {
        if (horizontal->k <= ed) {
            while (horizontal && horizontal->k <= ed) {
                v.push_back(horizontal->v);
                horizontal = horizontal->forward[0];
            }
        }
    }
    
    return true;
}

bool Skiplist::RangeCount (const time_t st, const time_t ed, size_t &cnt) {
    if (!head_) {
        return false;
    }
    cnt = 0;

    SkiplistNode *vertical = head_, *horizontal;
    for (int i = level_; i >= 0; i--) {
        horizontal = vertical->forward[i];
        while (horizontal && horizontal->k < st) {
            vertical = horizontal;
            horizontal = horizontal->forward[i];
        }
    }

    if (horizontal) {
        if (horizontal->k <= ed) {
            while (horizontal && horizontal->k <= ed) {
                ++cnt;
                horizontal = horizontal->forward[0];
            }
        }
    }

    return true;
}








};
