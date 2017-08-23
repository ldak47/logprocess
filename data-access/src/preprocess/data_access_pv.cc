#include "data_access_pv.h"

namespace dataaccess {

PvStater::PvStater (const std::map<std::string, std::vector<std::string>> &support) {
    std::map<std::string, std::vector<std::string>>::const_iterator it = support.begin();
    for (; it != support.end(); it++) {
        std::string application = it->first;
        std::vector<std::string> srcids = it->second;
        
        for (auto i: srcids) {
            std::string key1 = application + i + "succ", key2 = application + i + "fail";
            common::Skiplist *sk_succ = new common::Skiplist();
            pvstater.insert(make_pair(key1, sk_succ));
            common::Skiplist *sk_fail = new common::Skiplist();
            pvstater.insert(make_pair(key2, sk_fail));
        }
    }
    srcidcfg = support;
}

void PvStater::Inc (std::string application, std::string srcid, bool issucc, std::string strTime) {
    time_t logtime = strTime2Unixtime(strTime);
    std::string key = application + srcid;
    if (issucc) {
        key += "succ";
    } else {
        key += "fail";
    }
    std::map<std::string, common::Skiplist *>::iterator it = pvstater.find(key);
    if (it != pvstater.end()) {
        std::unique_lock<std::mutex> lock(mtx);
        common::Skiplist *sk = it->second;
        int pv = 0;
        bool res = sk->Search(logtime, pv);
        if (!res) {
            sk->Modify(logtime, 1);
        } else {
            sk->Modify(logtime, pv + 1);
        }
    }
}

std::map<std::string, int> PvStater::Cnt (std::string application, time_t st, time_t ed) {
    std::map<std::string, std::vector<std::string>>::iterator it = srcidcfg.find(application);
    if (it != srcidcfg.end()) {
        std::map<std::string, int> res;
        std::unique_lock<std::mutex> lock(mtx);
        
        std::vector<std::string> srcids = it->second;
        std::map<std::string, common::Skiplist *>::iterator iter;
        for (auto i: srcids) {
            std::string key1 = application + i + "succ", key2 = application + i + "fail";
            iter = pvstater.find(key1);
            if (iter != pvstater.end()) {
                int result = 0;
                common::Skiplist *sk = iter->second;
                if (st == ed) {
                    bool res = sk->Search(st, result);
                } else {
                    std::vector<int> results;
                    bool res = sk->RangeGet(st, ed, results);
                    for (auto i: results) {
                        result += i;
                    }
                }
                res.insert(make_pair(key1, result));
            }
            iter = pvstater.find(key2);
            if (iter != pvstater.end()) {
                int result = 0;
                common::Skiplist *sk = iter->second;
                if (st == ed) {
                    bool res = sk->Search(st, result);
                } else {
                    std::vector<int> results;
                    bool res = sk->RangeGet(st, ed, results);
                    for (auto i: results) {
                        result += i;
                    }
                }
                res.insert(make_pair(key2, result));
            }
        }
    
        return res;
    }
}

void PvStater::AddSupportType (const std::string &newtype, const std::string &newsrcid) {
    std::unique_lock<std::mutex> lock(mtx);
    if (srcidcfg.find(newtype) != srcidcfg.end()) {
        std::vector<std::string> current_srcids = srcidcfg[newtype];
        if (std::find(current_srcids.begin(), current_srcids.end(), newsrcid) == current_srcids.end()) {
            current_srcids.push_back(newsrcid);
            srcidcfg[newtype] = current_srcids;

            std::string key1 = newtype + newsrcid + "succ", key2 = newtype + newsrcid + "fail";
            common::Skiplist *sk_succ = new common::Skiplist();
            pvstater.insert(make_pair(key1, sk_succ));
            common::Skiplist *sk_fail = new common::Skiplist();
            pvstater.insert(make_pair(key2, sk_fail));
        }
    } else {
        std::vector<std::string> srcids;
        srcids.push_back(newsrcid);
        srcidcfg.insert(make_pair(newtype, srcids));

        std::string key1 = newtype + newsrcid + "succ", key2 = newtype + newsrcid + "fail";
        common::Skiplist *sk_succ = new common::Skiplist();
        pvstater.insert(make_pair(key1, sk_succ));
        common::Skiplist *sk_fail = new common::Skiplist();
        pvstater.insert(make_pair(key2, sk_fail));
    }
}

};
