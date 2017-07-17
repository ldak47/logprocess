#include "consistent_hash.h"

namespace common {

unsigned int ConsistentHash::FNV1_32_HASH (const char *key, int len) {
    int p = 16777619;
    int hash = (int)2166136261L;
    for (int i = 0; i < len; i++) {
        hash = (hash ^ key[i]) * p;
    }

    hash += hash << 13;
    hash ^= hash >> 7;
    hash += hash << 3;
    hash ^= hash >> 17;
    hash += hash << 5;
    if (hash < 0) {
        hash = (0 - hash);
    }

    return hash;
}

unsigned int ConsistentHash::murMurHash2 (const void *key, int len) {
    const unsigned int m = 0x5bd1e995;
    const int r = 24;
    const int seed = 97;
    unsigned int h = seed ^ len;

    // Mix 4 bytes at a time into the hash
    const unsigned char *data = (const unsigned char *)key;
    while(len >= 4) {
        unsigned int k = *(unsigned int *)data;
        k *= m;
        k ^= k >> r;
        k *= m;
        h *= m;
        h ^= k;
        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array
    switch(len) {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
        h *= m;
    }
}
    
void ConsistentHash::Init (const std::vector<std::string> &servers) {
    for (int i = 0; i < node_num_; i++) {
        for (int j = 0; j < vnode_num_; j++) {
            std::string virtual_node = servers[i] + ":" + std::to_string(j);
            unsigned int hashcode = FNV1_32_HASH(virtual_node.c_str(), virtual_node.length());
            server_nodes_vnodes_[hashcode] = servers[i];
        }
    }

    servers_ = servers;
}

std::string ConsistentHash::GetServerNode (const std::string &key) {
    unsigned int hashcode = FNV1_32_HASH(key.c_str(), key.length());

    std::map<int, std::string>::iterator it = server_nodes_vnodes_.lower_bound(hashcode);
    if (it == server_nodes_vnodes_.end()) {
        std::random_device rd;
        return servers_[rd() % (servers_.size())];
    } else {
        return it->second;
    }
}

void ConsistentHash::DeleteServerNode (const std::string &server) {
    std::map<int, std::string>::iterator it = server_nodes_vnodes_.begin();
    for (; it != server_nodes_vnodes_.end(); it++) {
        if (server == it->second) {
            server_nodes_vnodes_.erase(it);
            it = server_nodes_vnodes_.begin();
        }
    }

    std::vector<std::string>::iterator vt = servers_.begin();
    for (; vt != servers_.end(); ) {
        if (server == *vt) {
            vt = servers_.erase(vt);
        } else {
            vt++;
        }
    }
}

void ConsistentHash::AddServerNode (const std::string &server) {
    for (int i = 0; i < vnode_num_; i++) {
        std::string virtual_node = server + ":" + std::to_string(i);
        unsigned int hashcode = FNV1_32_HASH(virtual_node.c_str(), virtual_node.length());
        server_nodes_vnodes_[hashcode] = server;
    }

    servers_.push_back(server);
}


};
