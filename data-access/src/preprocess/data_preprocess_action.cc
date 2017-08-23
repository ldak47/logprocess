#include "data_preprocess_action.h"

namespace dataaccess {
/*
void Data_PreProcess (void *arg) {
    //LOG(INFO) << std::this_thread::get_id() << ": " << reinterpret_cast<struct RawBody *>(arg)->body_;
    std::string body = reinterpret_cast<struct RawBody *>(arg)->body_;
    
    delete (RawBody *)arg;
}
*/

void DataFieldRuler::LoadConfig (const libconfig::Setting &cfg, const std::vector<std::string> &types, const libconfig::Setting &srcid_cfg) {
    for (auto i: common::Range(0, types.size())) {
        std::string tp = types[i], srcids = "";
        
        std::map<std::string, std::string> field_regexes = app_field_regexes_[tp];
        std::map<std::string, bool> field_istrans = app_field_istrans_[tp];
        libconfig::Setting &tpcfg = cfg[tp.c_str()];
        int cnt = tpcfg.getLength();
        for (auto i: common::Range(0, cnt)) {
            std::string field, regex;
            bool IsTrans;
            tpcfg[i].lookupValue("field", field);
            tpcfg[i].lookupValue("regex", regex);
            tpcfg[i].lookupValue("trans", IsTrans);
            field_regexes[field] = regex;
            field_istrans[field] = IsTrans;
        }
        app_field_regexes_[tp] = field_regexes;
        app_field_istrans_[tp] = field_istrans;

        srcid_cfg.lookupValue(tp.c_str(), srcids);
        std::vector<std::string> tmp;
        boost::algorithm::split(tmp, srcids, boost::algorithm::is_any_of("|"));
        for (auto i: tmp) {
            srcid_type[i] = tp;
        }
    }
}

std::map<std::string, std::string> DataFieldRuler::GetRule (std::string app) {
    std::map<std::string, std::string> ruler;
    {
        std::unique_lock<std::mutex> lock(mtx);
        ruler = app_field_regexes_[app];
    }
    return ruler;
}

std::map<std::string, bool> DataFieldRuler::GetIsTrans (std::string app) {
    std::map<std::string, bool> ruler;
    {
        std::unique_lock<std::mutex> lock(mtx);
        ruler = app_field_istrans_[app];
    }
    return ruler;
}

std::string DataFieldRuler::GetType (std::string srcid) {
    std::string type;
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (srcid_type.find(srcid) == srcid_type.end()) {
            lock.unlock();
            return "";
        }
        type = srcid_type[srcid];
    }

    return type;
}

void DataFieldRuler::UpdateConfig (const std::string &type,
                                   const std::string &field,
                                   bool action,
                                   std::string regex,
                                   bool istrans,
                                   std::vector<std::string> newsrcids) {
    if (!action) {
        //delete
        {
            std::unique_lock<std::mutex> lock(mtx);
            std::map<std::string, std::map<std::string, std::string>>::iterator it1 = app_field_regexes_.find(type);
            std::map<std::string, std::map<std::string, bool>>::iterator it2 = app_field_istrans_.find(type);
            if (it1 == app_field_regexes_.end() ||
                it2 == app_field_istrans_.end()) {
                lock.unlock();
                return;
            }

            std::map<std::string, std::string> field_regexes = it1->second;
            std::map<std::string, bool> field_istrans = it2->second;
            
            std::map<std::string, std::string>::iterator fit1 = field_regexes.find(field);
            std::map<std::string, bool>::iterator fit2 = field_istrans.find(field);
            if (fit1 != field_regexes.end() && fit2 != field_istrans.end()) {
                field_regexes.erase(fit1);
                field_istrans.erase(fit2);
                app_field_regexes_[type] = field_regexes;
                app_field_istrans_[type] = field_istrans;
            }
        }
    } else {
        //insert or update
        {
            std::unique_lock<std::mutex> lock(mtx);
            std::map<std::string, std::map<std::string, std::string>>::iterator it1 = app_field_regexes_.find(type);
            std::map<std::string, std::map<std::string, bool>>::iterator it2 = app_field_istrans_.find(type);
            if (it1 == app_field_regexes_.end() ||
                it2 == app_field_istrans_.end()) {
                //insert new type
                for (auto i: newsrcids) {
                    srcid_type[i] = type;
                }
                std::map<std::string, std::string> new_field_regexes;
                new_field_regexes.insert(make_pair(field, regex));
                std::map<std::string, bool> new_field_istrans;
                new_field_istrans.insert(make_pair(field, istrans));
                app_field_regexes_.insert(make_pair(type, new_field_regexes));
                app_field_istrans_.insert(make_pair(type, new_field_istrans));
                lock.unlock();
                return;
            }

            std::map<std::string, std::string> field_regexes = it1->second;
            std::map<std::string, bool> field_istrans = it2->second;
            
            std::map<std::string, std::string>::iterator fit1 = field_regexes.find(field);
            if (fit1 != field_regexes.end()) {
                //update
                field_regexes[field] = regex;
                field_istrans[field] = istrans;
                app_field_regexes_[type] = field_regexes;
                app_field_istrans_[type] = field_istrans;
            } else {
                //only insert item
                field_regexes.insert(make_pair(field, regex));
                app_field_regexes_[type] = field_regexes;
                field_istrans.insert(make_pair(field, istrans));
                app_field_istrans_[type] = field_istrans;
            }
        }
    }
}

};
