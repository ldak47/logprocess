#ifndef DATAACCESS_PROCESS_QUERY_FREQ_H
#define DATAACCESS_PROCESS_QUERY_FREQ_H
#include "trie.h"
#include "libconfig.h++"
#include <map>
#include <unordered_map>
#include <mutex>
#include <memory>
#include "glog/logging.h"
#include "gflags/gflags.h"
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

namespace dataprocess {


class QueryFreqStat {
    static std::mutex qfmtx;
    static std::unique_ptr<common::Trie> qftree;
    
public:
    QueryFreqStat(){
        Load();
    }
    ~QueryFreqStat(){}
    static void UpdataQueryFreqStat(const std::map<std::string, std::string> &kv);
    static bool SearchExists(const std::string &query, const std::string &srcid, uint32_t &cnt);
    static std::string GetDataFile();
    static void Dump();
    static void Load();
};

};
#endif
