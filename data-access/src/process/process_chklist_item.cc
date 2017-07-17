#include "process_chklist_item.h"

namespace dataprocess {

bool checklist_item::addrule_srcid (const std::vector<process::SrcidConfig> &src_cfg) {
    srcid_chk_.reset(new checklist_srcid());
    if (!srcid_chk_.get()) {
        return false;
    }

    srcid_chk_->init(src_cfg);
    std::function<bool (const process::TransmitRequest *)> func = std::bind(&checklist_srcid::check, srcid_chk_.get(), std::placeholders::_1);
    chk_.push_back(func);
    return true;
}

bool checklist_item::addrule_seword (const std::vector<process::ScanCondSeWord> &seword_cfg) {
    seword_chk_.reset(new checklist_seword());
    if (!seword_chk_.get()) {
        return false;
    }
    
    seword_chk_->init(seword_cfg);
    std::function<bool (const process::TransmitRequest *)> func = std::bind(&checklist_seword::check, seword_chk_.get(), std::placeholders::_1);
    chk_.push_back(func);
    return true;
}

bool checklist_item::addrule_lbs (const std::vector<process::LocationInfo> &lbs_cfg) {
    lbs_chk_.reset(new checklist_lbs());
    if (!lbs_chk_.get()) {
        return false;
    }
    
    lbs_chk_->init(lbs_cfg);
    std::function<bool (const process::TransmitRequest *)> func = std::bind(&checklist_lbs::check, lbs_chk_.get(), std::placeholders::_1);
    chk_.push_back(func);
    return true;
}

bool checklist_item::addrule_cookie (const std::vector<process::CookieConfig> &cookie_cfg) {
    cookie_chk_.reset(new checklist_cookie());
    if (!cookie_chk_.get()) {
        return false;
    }
    
    cookie_chk_->init(cookie_cfg);
    std::function<bool (const process::TransmitRequest *)> func = std::bind(&checklist_cookie::check, cookie_chk_.get(), std::placeholders::_1);
    chk_.push_back(func);
    return true;
}
    
bool checklist_item::addrule_cluster (const std::vector<process::ClusterConfig> &cluster_cfg) {
    cluster_chk_.reset(new checklist_cluster());
    if (!cluster_chk_.get()) {
        return false;
    }
    
    cluster_chk_->init(cluster_cfg);
    std::function<bool (const process::TransmitRequest *)> func = std::bind(&checklist_cluster::check, cluster_chk_.get(), std::placeholders::_1);
    chk_.push_back(func);
    return true;
}

bool checklist_item::addrule_res_srcid (const std::vector<process::SrcidConfig> &res_src_cfg) {
    res_srcid_chk_.reset(new checklist_res_srcid());
    if (!res_srcid_chk_.get()) {
        return false;
    }
    
    res_srcid_chk_->init(res_src_cfg);
    std::function<bool (const process::TransmitRequest *)> func = std::bind(&checklist_res_srcid::check, res_srcid_chk_.get(), std::placeholders::_1);
    chk_.push_back(func);
    return true;
}

bool checklist_item::check (const process::TransmitRequest *request) {
    for (auto& i: chk_) {
        if (false == i((process::TransmitRequest *)request)) {
            return false;
        }
    }

    return true;
}






};
