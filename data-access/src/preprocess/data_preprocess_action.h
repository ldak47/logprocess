#ifndef DATAACCESS_DATAFIELDRULER_H
#define DATAACCESS_DATAFIELDRULER_H
#include "glog/logging.h"
#include "data_util.h"
#include "range_iterator.h"
#include "libconfig.h++"
#include <thread>
#include <regex>
#include <mutex>
#include <map>
#include <boost/algorithm/string.hpp>

using ruler_tuple = std::tuple<std::string, bool>;

namespace dataaccess {

class DataFieldRuler {
    std::map<std::string, std::map<std::string, std::string>> app_field_regexes_;
    std::map<std::string, std::map<std::string, bool>> app_field_istrans_;
    std::map<std::string, std::string> srcid_type;
    
    std::mutex mtx;
    
public:
    DataFieldRuler(){}
    ~DataFieldRuler(){}
    void LoadConfig(const libconfig::Setting &cfg, const std::vector<std::string> &types, const libconfig::Setting &srcid_cfg);
    void UpdateConfig(const std::string &type, 
                      const std::string &field, 
                      bool action,
                      std::string regex = "",
                      bool istrans = false,
                      std::vector<std::string> newsrcids = {});
    //void SetRule(std::string app, std::string field, std::string regex, bool istrans);
    std::map<std::string, std::string> GetRule(std::string app);
    std::map<std::string, bool> GetIsTrans(std::string app);
    std::string GetType(std::string srcid);
    void Cut();
};

};
#endif
