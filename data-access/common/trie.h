#ifndef DATAACCESS_COMMON_TRIE_H
#define DATAACCESS_COMMON_TRIE_H
#include <string>
#include <assert.h>
#include <unordered_map>

#define START_INDEX 48
#define CHAR_RANGE 10

namespace common {

struct TrieNode {
    struct TrieNode *children[CHAR_RANGE];
    uint32_t cnt;
    uint32_t childnum;
    bool isLeaf;

    TrieNode():isLeaf(false), cnt(0), childnum(0) {
        for (int i = 0; i < CHAR_RANGE; i++) {
            children[i] = nullptr;
        }
    }
};

class Trie {
    TrieNode *root;
    int size_;
    bool err_;

    TrieNode *GetNode();
    void ClearChildren(TrieNode *cur);

    void DFS(TrieNode *r, std::unordered_map<uint32_t, uint32_t> &res);
    
public:
    Trie();
    ~Trie();

    std::string dfs_str;
    void Dfs(std::unordered_map<uint32_t, uint32_t> &res) {
        dfs_str = "";
        DFS(root, res);
    }
    void Insert(uint32_t hashcode);
    bool Search(uint32_t hashcode, uint32_t &cnt);
};





};
#endif
