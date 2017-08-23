#include "process_chklist.h"

namespace dataprocess {

using namespace process;

void checklist_srcid::init (const std::vector<process::SrcidConfig> &cfg) {
    checklist_base<process::SrcidConfig>::init(cfg);
    fieldname = {"srcid"};
}
    
bool checklist_srcid::check (const process::TransmitRequest *request) {
    std::string _srcid("");
    for (size_t i = 0; i < request->values_size(); i++) {
        process::MapEntry kv = request->values(i);
        if (kv.key() == fieldname[0]) {
            _srcid = kv.value();
            break;
        }
    }
    if (_srcid == "") {
        return false;
    }
    
    for (auto i: cfg_) {
        bool res = srcid_check(_srcid, i);
        if (res) {
            return res;
        }
    }

    return false;
}

void checklist_seword::init (const std::vector<process::ScanCondSeWord> &cfg) {
    checklist_base<process::ScanCondSeWord>::init(cfg);
    fieldname = {"query"};
}

bool checklist_seword::check (const process::TransmitRequest *request) {
    std::string _query("");
    for (size_t i = 0; i < request->values_size(); i++) {
        process::MapEntry kv = request->values(i);
        if (kv.key() == fieldname[0]) {
            _query = kv.value();
            break;
        }
    }
    if (_query == "") {
        return false;
    }
    
    for (auto i: cfg_) {
        bool res = seword_check(_query, i);
        if (res) {
            return res;
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
        bool res = lbs_check(_prov, _city, i);
        if (res) {
            return res;
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
            break;
        }
    }
    
    for (auto i: cfg_) {
        bool res = cookie_check(_cookie, i);
        if (res) {
            return res;
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
            break;
        }
    }
    
    if (_cluster == "") {
        return false;
    }

    for (auto i: cfg_) {
        bool res = cluster_check(_cluster, i);
        if (res) {
            return res;
        }
    }

    return false;
}

void checklist_resno::init (const std::vector<process::ResnoConfig> &cfg) {
    checklist_base<process::ResnoConfig>::init(cfg);
    fieldname = {"resno"};
}

bool checklist_resno::check (const process::TransmitRequest *request) {
    std::string _resno("");
    for (size_t i = 0; i < request->values_size(); i++) {
        process::MapEntry kv = request->values(i);
        if (kv.key() == fieldname[0]) {
            _resno = kv.value();
            break;
        }
    }

    for (auto i: cfg_) {
        bool res = resno_check(_resno, i);
        if (res) {
            return res;
        }
    }

    return false;
}

void checklist_sid::init (const std::vector<process::SidConfig> &cfg) {
    checklist_base<process::SidConfig>::init(cfg);
    fieldname = {"sid"};
}

bool checklist_sid::check (const process::TransmitRequest *request) {
    std::string _sid("");
    for (size_t i = 0; i < request->values_size(); i++) {
        process::MapEntry kv = request->values(i);
        if (kv.key() == fieldname[0]) {
            _sid = kv.value();
            break;
        }
    }

    for (auto i: cfg_) {
        bool res = sid_check(_sid, i);
        if (res) {
            return res;
        }
    }

    return false;
}

void checklist_type::init (const std::vector<process::TypeConfig> &cfg) {
    checklist_base<process::TypeConfig>::init(cfg);
    fieldname = {"type"};
}

bool checklist_type::check (const process::TransmitRequest *request) {
    std::string _type("");
    for (size_t i = 0; i < request->values_size(); i++) {
        process::MapEntry kv = request->values(i);
        if (kv.key() == fieldname[0]) {
            _type = kv.value();
            break;
        }
    }

    for (auto i: cfg_) {
        bool res = type_check(_type, i);
        if (res) {
            return res;
        }
    }

    return false;
}

};
