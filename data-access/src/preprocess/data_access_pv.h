#ifndef DATAACCESS_ACCESS_PV_H
#define DATAACCESS_ACCESS_PV_H
#include "skiplist2.h"
#include "time_trans.h"
#include <sys/time.h>
#include <memory>
#include <mutex>
#include <map>
#include <vector>

namespace dataaccess {

class PvStater {
    std::map<std::string, common::Skiplist *> pvstater;
    bool err_;
    std::mutex mtx;
    std::map<std::string, std::vector<std::string>> srcidcfg;
    
public:
    PvStater(const std::map<std::string, std::vector<std::string>> &support);
    ~PvStater(){
        std::map<std::string, common::Skiplist *>::iterator it = pvstater.begin();
        for (; it != pvstater.end(); it++) {
            common::Skiplist *sk = it->second;
            delete sk;
            sk = nullptr;
            it = pvstater.erase(it);
        }
    }

    void AddSupportType(const std::string &newtype, const std::string &newsrcid);
    void Inc(std::string application, std::string srcid, bool issucc, std::string strTime);
    std::map<std::string, int> Cnt(std::string application, time_t st, time_t ed);
};

};
#endif
