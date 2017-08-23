#ifndef DATAACCESS_FIELD_H
#define DATAACCESS_FIELD_H
#include "libconfig.h++"
#include <boost/algorithm/string.hpp>
#include <mutex>
#include <set>

namespace dataaccess {

class AccessField {
    std::mutex mtx;
    std::set<std::string> useful_fields;
    bool err;
    
public:
    AccessField(const libconfig::Setting &field_cfg, const std::vector<std::string> &support_type);
    ~AccessField(){}
    bool GetError(){return err;}

    void Add (const std::string &field) {
        std::unique_lock<std::mutex> lock(mtx);
        useful_fields.insert(field);
    }
    void Del (const std::string &field) {
        std::unique_lock<std::mutex> lock(mtx);
        if (useful_fields.find(field) != useful_fields.end()) {
            useful_fields.erase(field);
        }
    }
    std::set<std::string> Get() {
        std::unique_lock<std::mutex> lock(mtx);
        return useful_fields;
    }

};

};
#endif









