#ifndef DATAACCESS_FILTER_H
#define DATAACCESS_FILTER_H
#include "libconfig.h++"
#include <boost/algorithm/string.hpp>
#include <mutex>
#include <set>
#include <vector>
#include "glog/logging.h"

namespace dataaccess {

class AccessFilter {
    std::mutex mtx;
    std::set<std::string> useful_srcids;
    bool err;
    
public:
    AccessFilter(const libconfig::Setting &filter_cfg, const std::vector<std::string> &support_type);
    ~AccessFilter(){}
    bool GetError(){return err;}

    void Add (const std::string &srcid) {
        std::unique_lock<std::mutex> lock(mtx);
        useful_srcids.insert(srcid);
    }
    void Del (const std::string &srcid) {
        std::unique_lock<std::mutex> lock(mtx);
        if (useful_srcids.find(srcid) != useful_srcids.end()) {
            useful_srcids.erase(srcid);
        }
    }
    std::set<std::string> Get() {
        std::unique_lock<std::mutex> lock(mtx);
        return useful_srcids;
    }

    bool Judge (const std::string &srcid) {
        std::unique_lock<std::mutex> lock(mtx);
        if (useful_srcids.find(srcid) != useful_srcids.end()) {
            return true;
        } else {
            return false;
        }
    }
};

};
#endif
