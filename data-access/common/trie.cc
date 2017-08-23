#include "trie.h"
#include <iostream>

namespace common {

Trie::Trie ():err_(false) {
    root = GetNode();
    if (!root) {
        err_ = true;
    }
}

Trie::~Trie () {
    ClearChildren(root);
    for (int i = 0; i < CHAR_RANGE; i++) {
        root->children[i] = nullptr;
    }
    delete root;
}

void Trie::ClearChildren (TrieNode *cur) {
    if (!cur) {
        return;
    }
    
    for (int i = 0; i < CHAR_RANGE; i++) {
        if (!root->children[i]) {
            continue;
        } else {
            ClearChildren(cur->children[i]);
            delete cur->children[i];
            cur->children[i] = nullptr;
            --cur->childnum;
            if (!cur->childnum) {
                break;
            }
        }
    }
}

TrieNode *Trie::GetNode () {
    TrieNode *newnode = new TrieNode();
    return newnode;
}

void Trie::Insert (uint32_t hashcode) {
    std::string str = std::to_string(hashcode);
    TrieNode *start = root;
    
    for (auto ch: str) {
        int idx = ch - START_INDEX;
        if (!start->children[idx]) {
            start->children[idx] = GetNode();
            assert(start->children[idx]);
            ++start->childnum;
        }
        
        start = start->children[idx];
    }

    if (start && !(start->isLeaf)) {
        ++size_;
    }
    start->isLeaf = true;
    ++start->cnt;
}

bool Trie::Search (uint32_t hashcode, uint32_t &cnt) {
    cnt = 0;
    std::string str = std::to_string(hashcode);
    TrieNode *start = root;

    for (auto ch: str) {
        int idx = ch - START_INDEX;
        if (!start->children[idx]) {
            return false;
        }

        start = start->children[idx];
    }

    if (start || start->isLeaf) {
        cnt = start->cnt;
        return true;
    }
    return false;
}

void Trie::DFS (TrieNode *r, std::unordered_map<uint32_t, uint32_t> &res) {
    if (!r) {
        return;
    } else if (r->isLeaf) {
        uint32_t k = static_cast<uint32_t>(stoi(dfs_str)), v = static_cast<uint32_t>(r->cnt);;
        res.insert(std::make_pair(k, v));
    }
    
    for (int i = 0; i < CHAR_RANGE; i++) {
        if (r->children[i]) {
            dfs_str += std::to_string(i);
            DFS(r->children[i], res);
            dfs_str = dfs_str.substr(0, dfs_str.length() - 1);
        }
    }
}


};
