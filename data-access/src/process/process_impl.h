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
#include "process_chklist_item.h"
#include "rocksdb.h"

namespace dataprocess {
    
    class Process_Impl {
        std::mutex mtx;
        std::map<std::string, std::vector<checklist_item *>> checklists;
        void CheckAbnormal(const std::string &type,
                           const process::TransmitRequest *request);
        
        std::map<std::string, std::vector<std::unique_ptr<common::Rocksdb>>> dbs;
        
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
        Process_Impl(const libconfig::Setting &dbcfg);
        ~Process_Impl(){
            std::map<std::string, std::vector<checklist_item *>>::iterator it = checklists.begin();
            for (; it != checklists.end(); it++) {
                std::vector<checklist_item *> chks = it->second;
                for (auto checklist: chks) {
                    delete checklist;
                }
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
