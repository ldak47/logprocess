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
    srcid_desc = src_cfg;
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
    seword_desc = seword_cfg;
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
    lbs_desc = lbs_cfg;
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
    cookie_desc = cookie_cfg;
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
    cluster_desc = cluster_cfg;
    return true;
}

bool checklist_item::addrule_sid (const std::vector<process::SidConfig> &sid_cfg) {
    sid_chk_.reset(new checklist_sid());
    if (!sid_chk_.get()) {
        return false;
    }

    sid_chk_->init(sid_cfg);
    std::function<bool (const process::TransmitRequest *)> func = std::bind(&checklist_sid::check, sid_chk_.get(), std::placeholders::_1);
    chk_.push_back(func);
    sid_desc = sid_cfg;
    return true;
}

bool checklist_item::addrule_resno (const std::vector<process::ResnoConfig> &resno_cfg) {
    resno_chk_.reset(new checklist_resno());
    if (!resno_chk_.get()) {
        return false;
    }
    
    resno_chk_->init(resno_cfg);
    std::function<bool (const process::TransmitRequest *)> func = std::bind(&checklist_resno::check, resno_chk_.get(), std::placeholders::_1);
    chk_.push_back(func);
    resno_desc = resno_cfg;
    return true;
}

bool checklist_item::addrule_type (const std::vector<process::TypeConfig> &type_cfg) {
    type_chk_.reset(new checklist_type());
    if (!type_chk_.get()) {
        return false;
    }

    type_chk_->init(type_cfg);
    std::function<bool (const process::TransmitRequest *)> func = std::bind(&checklist_type::check, type_chk_.get(), std::placeholders::_1);
    chk_.push_back(func);
    type_desc = type_cfg;
    return true;
}

//every checklist entrance(return false: no abnormal, return true: abnormal)
bool checklist_item::check (const process::TransmitRequest *request, std::string &descript) {
    descript = "";
    for (auto& i: chk_) {
        if (false == i((process::TransmitRequest *)request)) {
            //descript = description_;
            return false;
        }
    }

    descript = description_;
    return true;
}

};
