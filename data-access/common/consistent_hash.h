#ifndef DATAACCESS_COMMON_CONSISTENTHASH_H
#define DATAACCESS_COMMON_CONSISTENTHASH_H
#include <map>
#include <string>
#include <vector>
#include <assert.h>
#include <random>

namespace common {

unsigned int FNV1_32_HASH (const std::string &str);
    
class ConsistentHash {
    std::vector<std::string> servers_;
    std::map<int, std::string> server_nodes_vnodes_;
    int node_num_;
    int vnode_num_;
    
public:
    ConsistentHash(const std::vector<std::string> &servers, int node_num, int vnode_num): node_num_(node_num),vnode_num_(vnode_num) {
        assert(node_num_ == servers.size());
        Init(servers);
    }
    ~ConsistentHash() {
        server_nodes_vnodes_.clear();
    }
    unsigned int murMurHash2 (const void *key, int len);
    unsigned int FNV1_32_HASH(const char *key, int len);

    void Init(const std::vector<std::string> &servers);
    
    std::string GetServerNode(const std::string &key);
    void DeleteServerNode(const std::string &server);
    void AddServerNode(const std::string &server);
};


};
#endif
