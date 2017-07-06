#include "data_preprocess_action.h"

namespace dataaccess {
/*
void Data_PreProcess (void *arg) {
    //LOG(INFO) << std::this_thread::get_id() << ": " << reinterpret_cast<struct RawBody *>(arg)->body_;
    std::string body = reinterpret_cast<struct RawBody *>(arg)->body_;
    
    delete (RawBody *)arg;
}
*/

void DataFieldRuler::LoadConfig (const libconfig::Setting &cfg, const std::vector<std::string> &types) {
    for (auto i: common::Range(0, types.size())) {
        std::string tp = types[i];
        
        std::map<std::string, std::string> field_regexes = app_field_regexes_[tp];
        libconfig::Setting &tpcfg = cfg[tp.c_str()];
        int cnt = tpcfg.getLength();
        for (auto i: common::Range(0, cnt)) {
            std::string field, regex;
            tpcfg[i].lookupValue("field", field);
            tpcfg[i].lookupValue("regex", regex);
            field_regexes[field] = regex;
        }
        app_field_regexes_[tp] = field_regexes;
    }
}

std::map<std::string, std::string> DataFieldRuler::GetRule (std::string app) {
    return app_field_regexes_[app];
}

};
