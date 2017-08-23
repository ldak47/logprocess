#include "client_hook.h"
#include "range_iterator.h"
#include "glog/logging.h"
#include "gflags/gflags.h"
#include "libconfig.h++"
#include "thread_pool.h"
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include "rocksdb.h"
#include "cJSON/cJSON.h"
#include "process_storage.h"
#include "process_stat.h"
#include "time_trans.h"
#include "timer.h"
#include "process_chklist_item.h"
#include "process_check_action.h"

#include "consistent_hash.h"
#include "curl.h"
#include "process_stat_query_freq.h"
#include "process_stat_cookie_stat.h"
#include "process_top.h"

namespace dataprocess {
    
    class Process_Impl {
        std::string GetCurrPath () {
            char *file_path_getcwd;
            file_path_getcwd = (char *)malloc(80);
            getcwd(file_path_getcwd, 80);
            std::string curpath = file_path_getcwd;
            free(file_path_getcwd);
            return curpath;
        }

        std::unique_ptr<ProcessStater> stater;
        bool StaterInit(const libconfig::Setting &rdscfg);

        std::map<std::string, StorageImpl *> storager;
        void Storage(std::string type,
                     std::string key,
                     const std::map<std::string, std::string> &kv,
                     bool debug = false);
        std::vector<std::string> Read(std::string type, time_t start, time_t end = -1);
        std::vector<std::map<std::string, std::string>> ReadFromRocksdb(std::string application, time_t start, time_t end);
        void ParseSelectField(const process::GetLogProcessStatRequest *request, std::vector<std::string> &selectfields);
        void SelectByConds (const std::vector<std::map<std::string, std::string>> &raw,
                       std::vector<std::map<std::string, std::string>> &res,
                       const process::CheckListCond &conds,
                       std::vector<std::string> selectfield = {});
        
        std::unique_ptr<ThreadPool> alarmsender;
        std::mutex mtx;
        std::map<std::string, std::vector<checklist_item *>> checklists;
        void CheckAbnormal(std::string type,
                           const process::TransmitRequest *request,
                           const std::map<std::string, std::string> &kv);

        
        std::map<std::string, std::vector<std::unique_ptr<common::Rocksdb>>> dbs;
        
        std::string plathttpaddr_;
        std::string MakeAlarmInfo(const std::map<std::string, std::string> &kv,
                                  const std::string &description);
        checklist_item *MakeCheckListItem (const process::CheckListCond &conds, 
                                           process::SetCheckListConfigResponse *response = nullptr);
        void AddCheckList (const std::string &app, checklist_item *newitem) {
            std::unique_lock<std::mutex> lock(mtx);
            if (checklists.find(app) != checklists.end()) {
                std::vector<checklist_item *> cur = checklists[app];
                cur.push_back(newitem);
                checklists[app] = cur;
            } else {
                std::vector<checklist_item *> empty;
                empty.push_back(newitem);
                checklists[app] = empty;
            }
        }

        void DelCheckList (const std::string &app, const uint32_t index) {
            std::unique_lock<std::mutex> lock(mtx);
            if (checklists.find(app) != checklists.end()) {
                std::vector<checklist_item *> cur = checklists[app];
                std::vector<checklist_item *>::iterator it = cur.begin();
                for (int i = 0; it != cur.end(); i++) {
                    if (i == index) {
                        checklist_item *item = *it;
                        delete item;
                        item = *it = nullptr;
                        it = cur.erase(it);
                    } else {
                        it++;
                    }
                }
                checklists[app] = cur;
            }
        }

        void FillCondition(checklist_item *item, process::CheckListCond *cond);
        std::vector<checklist_item *> GetCheckList (const std::string &app) {
            if (checklists.find(app) == checklists.end()) {
                std::vector<checklist_item *> empty;
                return empty;
            } else {
                return checklists[app];
            }
        }

        std::map<std::string, std::string> request2kv(const process::TransmitRequest *request, bool debug = false);
        
    public:
        Process_Impl(const libconfig::Setting &dbcfg, 
                     const libconfig::Setting &rdscfg, 
                     const std::vector<std::string> &types, 
                     int port,
                     const std::string &accessid,
                     const std::string &dbpath,
                     const std::string &plathttpaddr);
        ~Process_Impl(){
            std::map<std::string, std::vector<checklist_item *>>::iterator it = checklists.begin();
            for (; it != checklists.end(); it++) {
                std::vector<checklist_item *> chks = it->second;
                for (auto checklist: chks) {
                    delete checklist;
                }
            }

            std::map<std::string, StorageImpl *>::iterator iter = storager.begin();
            for (; iter != storager.end(); iter++) {
                StorageImpl *storageimpler = iter->second;
                delete storageimpler;
            }
        }
        void DataReceive(const process::TransmitRequest *request,
                         process::TransmitResponse *response,
                         ::google::protobuf::Closure *done);
        void SetCheckListConfig(const process::SetCheckListConfigRequest *request,
                                process::SetCheckListConfigResponse *response,
                                ::google::protobuf::Closure *done);
        void GetCheckListConfig(const process::GetCheckListConfigRequest *request,
                                process::GetCheckListConfigResponse *response,
                                ::google::protobuf::Closure *done);
        void GetLogProcessStat(const process::GetLogProcessStatRequest *request,
                               process::GetLogProcessStatResponse *response,
                               ::google::protobuf::Closure *done);
    };
};
