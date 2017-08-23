#ifndef DATAACCESS_PROCESS_CHECKLIST_ITEM_H
#define DATAACCESS_PROCESS_CHECKLIST_ITEM_H
#include "process_chklist.h"
#include <functional>

namespace dataprocess {

class checklist_item {
    std::unique_ptr<checklist_srcid> srcid_chk_;
    std::unique_ptr<checklist_seword> seword_chk_;
    std::unique_ptr<checklist_lbs> lbs_chk_;
    std::unique_ptr<checklist_cookie> cookie_chk_;
    std::unique_ptr<checklist_cluster> cluster_chk_;
    std::unique_ptr<checklist_sid> sid_chk_;
    std::unique_ptr<checklist_resno> resno_chk_;
    std::unique_ptr<checklist_type> type_chk_;

    std::vector<std::function<bool (process::TransmitRequest *)>> chk_;
    std::string description_;

    std::vector<process::SrcidConfig> srcid_desc;
    std::vector<process::ScanCondSeWord> seword_desc;
    std::vector<process::LocationInfo> lbs_desc;
    std::vector<process::CookieConfig> cookie_desc;
    std::vector<process::ClusterConfig> cluster_desc;
    std::vector<process::SidConfig> sid_desc;
    std::vector<process::ResnoConfig> resno_desc;
    std::vector<process::TypeConfig> type_desc;
    
public:
    checklist_item(const std::string &description): description_(description) {}
    ~checklist_item(){}

    void dump();

    bool operator==(checklist_item &other) {
        std::vector<process::SrcidConfig> srcid_desc_other = other.getrule_srcid();
        std::vector<process::ScanCondSeWord> seword_desc_other = other.getrule_seword();
        std::vector<process::LocationInfo> lbs_desc_other = other.getrule_lbs();
        std::vector<process::CookieConfig> cookie_desc_other = other.getrule_cookie();
        std::vector<process::ClusterConfig> cluster_desc_other = other.getrule_cluster();
        std::vector<process::SidConfig> sid_desc_other = other.getrule_sid();
        std::vector<process::ResnoConfig> resno_desc_other = other.getrule_resno();
        std::vector<process::TypeConfig> type_desc_other = other.getrule_type();
        if (srcid_desc_other.size() != srcid_desc.size() || 
            seword_desc_other.size() != seword_desc.size() ||
            lbs_desc_other.size() != lbs_desc.size() ||
            cookie_desc_other.size() != cookie_desc.size() ||
            cluster_desc_other.size() != cluster_desc.size() ||
            sid_desc_other.size() != sid_desc.size() ||
            resno_desc_other.size() != resno_desc.size() ||
            type_desc_other.size() != type_desc.size()) {
            return false;
        } else {
            if (srcid_desc_other.size()) {
                for (size_t i = 0; i < srcid_desc_other.size(); i++) {
                    process::SrcidConfig o1 = srcid_desc_other[i], o2 = srcid_desc[i];
                    if (o1.srcid() != o2.srcid() || o1.cond() != o2.cond()) {
                        return false;
                    }
                }
            }
            if (seword_desc_other.size()) {
                for (size_t i = 0; i < seword_desc_other.size(); i++) {
                    process::ScanCondSeWord o1 = seword_desc_other[i], o2 = seword_desc[i];
                    if (o1.searchword() != o2.searchword() || o1.cond() != o2.cond()) {
                        return false;
                    }
                }
            }
            if (lbs_desc_other.size()) {
                for (size_t i = 0; i < lbs_desc_other.size(); i++) {
                    process::LocationInfo o1 = lbs_desc_other[i], o2 = lbs_desc[i];
                    if (o1.province() != o2.province() || o1.city() != o2.city() || o1.cond() != o2.cond()) {
                        return false;
                    }
                }
            }
            if (cookie_desc_other.size()) {
                for (size_t i = 0; i < cookie_desc_other.size(); i++) {
                    process::CookieConfig o1 = cookie_desc_other[i], o2 = cookie_desc[i];
                    if (o1.cookie() != o2.cookie() || o1.cond() != o2.cond()){
                        return false;
                    }
                }
            }
            if (cluster_desc_other.size()) {
                for (size_t i = 0; i < cluster_desc_other.size(); i++) {
                    process::ClusterConfig o1 = cluster_desc_other[i], o2 = cluster_desc[i];
                    if (o1.cluster() != o2.cluster() || o1.cond() != o2.cond()) {
                        return false;
                    }
                }
            }
            if (sid_desc_other.size()) {
                for (size_t i = 0; i < sid_desc_other.size(); i++) {
                    process::SidConfig o1 = sid_desc_other[i], o2 = sid_desc[i];
                    if (o1.sid() != o2.sid() || o1.cond() != o2.cond()) {
                        return false;
                    }
                }
            }
            if (resno_desc_other.size()) {
                for (size_t i = 0; i < resno_desc_other.size(); i++) {
                    process::ResnoConfig o1 = resno_desc_other[i], o2 = resno_desc[i];
                    if (o1.resno() != o2.resno() || o1.cond() != o2.cond()) {
                        return false;
                    }
                }
            }
            if (type_desc_other.size()) {
                for (size_t i = 0; i < type_desc_other.size(); i++) {
                    process::TypeConfig o1 = type_desc_other[i], o2 = type_desc[i];
                    if (o1.type() != o2.type() || o1.cond() != o2.cond()) {
                        return false;
                    }
                }
            }
        }

        return true;
    }
    std::vector<process::SrcidConfig> getrule_srcid() {
        return srcid_desc;
    }
    std::vector<process::ScanCondSeWord> getrule_seword() {
        return seword_desc;
    }
    std::vector<process::LocationInfo> getrule_lbs() {
        return lbs_desc;
    }
    std::vector<process::CookieConfig> getrule_cookie() {
        return cookie_desc;
    }
    std::vector<process::ClusterConfig> getrule_cluster() {
        return cluster_desc;
    }
    std::vector<process::SidConfig> getrule_sid() {
        return sid_desc;
    }
    std::vector<process::ResnoConfig> getrule_resno() {
        return resno_desc;
    }
    std::vector<process::TypeConfig> getrule_type() {
        return type_desc;
    }

    bool addrule_srcid(const std::vector<process::SrcidConfig> &src_cfg);
    bool addrule_seword(const std::vector<process::ScanCondSeWord> &seword_cfg);
    bool addrule_lbs(const std::vector<process::LocationInfo> &lbs_cfg);
    bool addrule_cookie(const std::vector<process::CookieConfig> &cookie_cfg);
    bool addrule_cluster(const std::vector<process::ClusterConfig> &cluster_cfg);
    bool addrule_sid(const std::vector<process::SidConfig> &sid_cfg);
    bool addrule_resno(const std::vector<process::ResnoConfig> &resno_cfg);
    bool addrule_type(const std::vector<process::TypeConfig> &type_cfg);
    
    bool check(const process::TransmitRequest *request, std::string &descript);
};



};
#endif
