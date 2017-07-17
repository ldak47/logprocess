#ifndef DATAACCESS_PROCESS_CHECKLIST_ITEM_H
#define DATAACCESS_PROCESS_CHECKLIST_ITEM_H
#include "process_chklist.h"
#include <functional>

namespace dataprocess {

enum CheckRes {
    COMPLETENOT = 1,
    PARTIS = 2,
    COMPLETEis = 3,
};

class checklist_item {
    std::unique_ptr<checklist_srcid> srcid_chk_;
    std::unique_ptr<checklist_seword> seword_chk_;
    std::unique_ptr<checklist_lbs> lbs_chk_;
    std::unique_ptr<checklist_cookie> cookie_chk_;
    std::unique_ptr<checklist_cluster> cluster_chk_;
    std::unique_ptr<checklist_res_srcid> res_srcid_chk_;

    std::vector<std::function<bool (process::TransmitRequest *)>> chk_;
    
public:
    checklist_item() {}
    ~checklist_item(){}

    bool addrule_srcid(const std::vector<process::SrcidConfig> &src_cfg);
    bool addrule_seword(const std::vector<process::ScanCondSeWord> &seword_cfg);
    bool addrule_lbs(const std::vector<process::LocationInfo> &lbs_cfg);
    bool addrule_cookie(const std::vector<process::CookieConfig> &cookie_cfg);
    bool addrule_cluster(const std::vector<process::ClusterConfig> &cluster_cfg);
    bool addrule_res_srcid(const std::vector<process::SrcidConfig> &res_src_cfg);
    
    bool check(const process::TransmitRequest *request);
};



};
#endif
