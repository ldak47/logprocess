#include "process_check_action.h"

namespace dataprocess {

#define SRCID 0
#define SEWORD 1
#define LBS 2
#define COOKIE 3
#define CLUSTER 4
#define RESNO 5
#define SID 6
#define TYPE 7
#define CURRENT_MAX 8
    
bool srcid_check (const std::string &srcid, const process::SrcidConfig &cfg) {
    std::string _srcid = cfg.srcid();
    process::CondType cond = cfg.cond();
    
    if (cond == process::condEqual && srcid == _srcid) {
        return true;
    } else if (cond == process::condNotEqual && srcid != _srcid) {
        return true;
    }

    return false;
}

bool seword_check (const std::string &query, const process::ScanCondSeWord &cfg) {
    std::string searchword = cfg.searchword();
    process::CondType cond = cfg.cond();

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
            return false;
        }
    } else if (cond == process::condIsBehind && query.find(searchword) != std::string::npos) {
        std::string regstr = "\.+" + searchword + "$";
        std::regex reg(regstr.c_str());
        try {
            std::smatch res;
            if (regex_search(query, res, reg)) {
                return true;
            }
        } catch (...) {
            return false;
        }
    }
       
    return false;
}

bool lbs_check (const std::string &prov, const std::string &city, const process::LocationInfo &cfg) {
    std::string _prov = cfg.province();
    std::string _city = cfg.city();
    process::CondType cond = cfg.cond();
    
    if (cond == process::condNotExist && _prov == "" && _city == "") {
        return true;
    } else if (cond == process::condPartEqual && (_prov == "" || _city != "")) {
        return true;
    } else if (cond == process::condEqual && _prov == prov && _city == city) {
        return true;
    }

    return false;
}

bool cookie_check (const std::string &cookie, const process::CookieConfig &cfg) {
    std::string _cookie = cfg.cookie();
    process::CondType cond = cfg.cond();

    if (cond == process::condNotExist && _cookie == "") {
        return true;
    } else if (cond == process::condEqual && _cookie == cookie) {
        return true;
    }

    return false;
}

bool cluster_check (const std::string &cluster, const process::ClusterConfig &cfg) {
    std::string _cluster = cfg.cluster();
    process::CondType cond = cfg.cond();

    if (cond == process::condEqual && cluster == _cluster) {
        return true;
    } else if (cond == process::condNotEqual && cluster != _cluster) {
        return true;
    }

    return false;
}

bool sid_check (const std::string &sid, const process::SidConfig &cfg) {
    std::string _sid = cfg.sid();
    process::CondType cond = cfg.cond();

    if (cond == process::condEqual && sid == _sid) {
        return true;
    } else if (cond == process::condNotIn && sid.find(_sid) == std::string::npos) {
        return true;
    } else if (cond == process::condIn && sid.find(_sid) != std::string::npos) {
        return true;
    }

    return false;
}

bool resno_check (const std::string &resno, const process::ResnoConfig &cfg) {
    std::string _resno = cfg.resno();
    process::CondType cond = cfg.cond();

    if (cond == process::condEqual && resno == _resno) {
        return true;
    } else if (cond == process::condNotEqual && resno != _resno) {
        return true;
    }

    return false;
}

bool type_check (const std::string &type, const process::TypeConfig &cfg) {
    std::string _type = cfg.type();
    process::CondType cond = cfg.cond();

    if (cond == process::condEqual && type == _type) {
        return true;
    } else if (cond == process::condNotEqual && type != _type) {
        return true;
    }

    return false;
}

bool all_check (const process::CheckListCond &conds, const std::map<std::string, std::string> &item) {
    common::Bitmap bm1(0), bm2(0);
    
    if (conds.srcids_size() && item.find("srcid") != item.end()) {
        bm1.set(SRCID);
        std::string srcid = item.find("srcid")->second;
        int i = 0, j = 0;
        for (; i < conds.srcids_size(); i++) {
            if (srcid_check(srcid, conds.srcids(i))) {
                j++;
            }
        }
        if (j == i) {
            bm2.set(SRCID);
        }
    }
    
    if (conds.sewords_size() && item.find("query") != item.end()) {
        bm1.set(SEWORD);
        std::string query = item.find("query")->second;
        int i = 0, j = 0;
        for (; i < conds.sewords_size(); i++) {
            if (seword_check(query, conds.sewords(i))) {
                j++;
            }
        }
        if (j == i) {
            bm2.set(SEWORD);
        }
    }
    
    if (conds.lbs_size() && item.find("prov") != item.end() && item.find("city") != item.end()) {
        bm1.set(LBS);
        std::string prov = item.find("prov")->second;
        std::string city = item.find("city")->second;
        int i = 0, j = 0;
        for (; i < conds.lbs_size(); i++) {
            if (!lbs_check(prov, city, conds.lbs(i))) {
                j++;
            }
        }
        if (j == i) {
            bm2.set(LBS);
        }
    }
    
    if (conds.cookie_size() && item.find("cookie") != item.end()) {
        bm1.set(COOKIE);
        std::string cookie = item.find("cookie")->second;
        int i = 0, j = 0;
        for (; i < conds.cookie_size(); i++) {
            if (cookie_check(cookie, conds.cookie(i))) {
                j++;
            }
        }
        if (j == i) {
            bm2.set(COOKIE);
        }
    }
    
    if (conds.cluster_size() && item.find("cluster") != item.end()) {
        bm1.set(CLUSTER);
        std::string cluster = item.find("cluster")->second;
        int i = 0, j = 0;
        for (; i < conds.cluster_size(); i++) {
            if (cluster_check(cluster, conds.cluster(i))) {
                j++;
            }
        }
        if (j == i) {
            bm2.set(CLUSTER);
        }
    }
    
    if (conds.resno_size() && item.find("resno") != item.end()) {
        bm1.set(RESNO);
        std::string resno = item.find("resno")->second;
        int i = 0, j = 0;
        for (; i < conds.resno_size(); i++) {
            if (resno_check(resno, conds.resno(i))) {
                j++;
            }
        }
        if (j == i) {
            bm2.set(RESNO);
        }
    }

    if (conds.sid_size() && item.find("sid") != item.end()) {
        bm1.set(SID);
        std::string sid = item.find("sid")->second;
        int i = 0, j = 0;
        for (; i < conds.resno_size(); i++) {
            if (!sid_check(sid, conds.sid(i))) {
                j++;
            }
        }
        if (j == i) {
            bm2.set(SID);
        }
    }
    
    if (conds.type_size() && item.find("type") != item.end()) {
        bm1.set(TYPE);
        std::string type = item.find("type")->second;
        int i = 0, j = 0;
        for (; i < conds.type_size(); i++) {
            if (!type_check(type, conds.type(i))) {
                j++;
            }
        }
        if (j == i) {
            bm2.set(TYPE);
        }
    }
    
    for (int i = 0; i < CURRENT_MAX; i++) {
        if (bm1.get(i) != bm2.get(i)) {
            return false;
        }
    }
    return true;
}






};
