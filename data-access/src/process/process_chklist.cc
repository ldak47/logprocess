#include "process_chklist.h"

namespace dataprocess {

using namespace process;

void checklist_srcid::init (const std::vector<process::SrcidConfig> &cfg) {
    checklist_base<process::SrcidConfig>::init(cfg);
    fieldname = {"srcid"};
}
    
bool checklist_srcid::check (const process::TransmitRequest *request) {
    std::string srcid("");
    for (size_t i = 0; i < request->values_size(); i++) {
        process::MapEntry kv = request->values(i);
        if (kv.key() == fieldname[0]) {
            srcid = kv.value();
            break;
        }
    }
    if (srcid == "") {
        return false;
    }
    
    for (auto i: cfg_) {
        std::string _srcid = i.srcid();
        process::CondType cond = i.cond();
        
        if (cond == process::condEqual) {
            if (_srcid == srcid) {
                return true;
            }
        } else {
            continue;
        }
    }

    return false;
}

void checklist_seword::init (const std::vector<process::ScanCondSeWord> &cfg) {
    checklist_base<process::ScanCondSeWord>::init(cfg);
    fieldname = {"query"};
}

bool checklist_seword::check (const process::TransmitRequest *request) {
    std::string query("");
    for (size_t i = 0; i < request->values_size(); i++) {
        process::MapEntry kv = request->values(i);
        if (kv.key() == fieldname[0]) {
            query = kv.value();
            break;
        }
    }
    if (query == "") {
        return false;
    }
    
    for (auto i: cfg_) {
        std::string searchword = i.searchword();
        process::CondType cond = i.cond();

        if (cond == process::condEqual && query == searchword) {
            return true;
        } else if (cond == process::condNotEqual && query != searchword) {
            return true;
        } else if (cond == process::condIn && query.find(searchword) != std::string::npos) {
            return true;
        } else if (cond == process::condNotIn && query.find(searchword) == std::string::npos) {
            return true;
        } else if (cond == process::condIsFront && query.find(searchword) != std::string::npos) {
            std::string regstr = "^" + searchword + "\.+";
            std::regex reg(regstr.c_str());
            try {
                std::smatch res;
                if (regex_search(query, res, reg)) {
                    return true;
                }
            } catch (...) {
                continue;
            }
        } else if (cond == condIsBehind && query.find(searchword) != std::string::npos) {
            std::string regstr = "\.+" + searchword + "$";
            std::regex reg(regstr.c_str());
            try {
                std::smatch res;
                if (regex_search(query, res, reg)) {
                    return true;
                }
            } catch (...) {
                continue;
            }
        }
    }

    return false;
}

void checklist_lbs::init (const std::vector<process::LocationInfo> &cfg) {
    checklist_base<process::LocationInfo>::init(cfg);
    fieldname = {"province_name", "city_name"};
}

bool checklist_lbs::check (const process::TransmitRequest *request) {
    std::string _prov(""), _city("");
    for (size_t i = 0; i < request->values_size(); i++) {
        process::MapEntry kv = request->values(i);
        if (kv.key() == fieldname[0]) {
            _prov = kv.value();
        }
        if (kv.key() == fieldname[1]) {
            _city = kv.value();
        }
    }
    
    for (auto i: cfg_) {
        std::string prov = i.province();
        std::string city = i.city();
        process::CondType cond = i.cond();

        if (cond == process::condNotExist && _prov == "" && _city == "") {
            return true;
        } else if (cond == process::condPartEqual && (_prov == "" || _city != "")) {
            return true;
        } else if (cond == process::condEqual && _prov == prov && _city == city) {
            return true;
        }
    }

    return false;
}

void checklist_cookie::init (const std::vector<process::CookieConfig> &cfg) {
    checklist_base<process::CookieConfig>::init(cfg);
    fieldname = {"cookie"};
}

bool checklist_cookie::check (const process::TransmitRequest *request) {
    std::string _cookie("");
    for (size_t i = 0; i < request->values_size(); i++) {
        process::MapEntry kv = request->values(i);
        if (kv.key() == fieldname[0]) {
            _cookie = kv.value();
        }
    }
    
    for (auto i: cfg_) {
        std::string cookie = i.cookie();
        process::CondType cond = i.cond();

        if (cond == process::condNotExist && _cookie == "") {
            return true;
        } else if (cond == process::condEqual && _cookie == cookie) {
            return true;
        }
    }

    return false;
}

void checklist_cluster::init (const std::vector<process::ClusterConfig> &cfg) {
    checklist_base<process::ClusterConfig>::init(cfg);
    fieldname = {"cluster"};
}

bool checklist_cluster::check (const process::TransmitRequest *request) {
    std::string _cluster("");
    for (size_t i = 0; i < request->values_size(); i++) {
        process::MapEntry kv = request->values(i);
        if (kv.key() == fieldname[0]) {
            _cluster = kv.value();
        }
    }
    
    if (_cluster == "") {
        return false;
    }

    for (auto i: cfg_) {
        std::string cluster = i.cluster();
        process::CondType cond = i.cond();

        if (cond == process::condEqual && cluster == _cluster) {
            return true;
        } else if (cond == process::condNotEqual && cluster != _cluster) {
            return true;
        }
    }

    return false;
}

void checklist_res_srcid::init (const std::vector<process::SrcidConfig> &cfg) {
    checklist_base<process::SrcidConfig>::init(cfg);
    fieldname = {"res_srcid"};
}

bool checklist_res_srcid::check (const process::TransmitRequest *request) {
    std::string _res_srcid("");
    for (size_t i = 0; i < request->values_size(); i++) {
        process::MapEntry kv = request->values(i);
        if (kv.key() == fieldname[0]) {
            _res_srcid = kv.value();
        }
    }

    for (auto i: cfg_) {
        std::string res_srcid = i.srcid();
        process::CondType cond = i.cond();

        if (cond == process::condNotEqual && _res_srcid == "") {
            return true;
        } else if (cond == process::condExist && _res_srcid == res_srcid) {
            return true;
        } else if (cond == process::condNotEqual && _res_srcid != res_srcid) {
            return true;
        }
    }

    return false;
}


};
