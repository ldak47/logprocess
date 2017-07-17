#include "process_impl.h"

namespace dataprocess {

Process_Impl::Process_Impl (const libconfig::Setting &dbcfg) {
    
}

void Process_Impl::CheckAbnormal (const std::string &type, const process::TransmitRequest *request) {
    std::map<std::string, std::vector<checklist_item *>>::iterator it = checklists.find(type);
    if (it != checklists.end()) {
        for (auto i: it->second) {
            if (!i->check(request)) {
                //async curl to platform
                LOG(WARNING) << "detect abnormal by checklist ";
            }
        }
    }
}

std::map<std::string, std::string> Process_Impl::request2kv (const process::TransmitRequest *request, bool debug) {
    std::map<std::string, std::string> result;
    int fieldnum = request->values_size();
    std::string debug_info("");
    for (int i: common::Range(0, fieldnum)) {
        process::MapEntry kv = request->values(i);
        std::string key = kv.key(), value = kv.value();
        if (debug) {
            debug_info += key;
            debug_info += ": ";
            debug_info += (value + "    ");
        }
        result.insert(make_pair(key, value));
    }
    
    if (debug) {
        LOG(INFO) << "debug: [" << debug_info << "]";
    }
    return result;
}

void Process_Impl::DataReceive (const process::TransmitRequest *request,
                                process::TransmitResponse *response,
                                ::google::protobuf::Closure *done) {
    std::map<std::string, std::string> kv = request2kv(request, true);
    //here actual process...
    //1 checklist process
    CheckAbnormal(kv["application"], request);
    
    response->set_status(process::TransmitResponse_Res_Status_SUCCEED);
    done->Run();
}

void Process_Impl::SetCheckListConfig (const process::SetCheckListConfigRequest *request,
                                       process::SetCheckListConfigResponse *response,
                                       ::google::protobuf::Closure *done) {
    std::string application = request->application();
    process::CheckListCond conds = request->cond();

    checklist_item *newitem;
    try {
        newitem = new checklist_item();
    } catch (std::exception &err) {
        LOG(ERROR) << "SetCheckListConfig alloc new checklist_item fail with reason " << err.what();
        response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
        response->set_error("addrule_srcid alloc new checklist_item fail");
        done->Run();
    }
    
    if (conds.srcids_size()) {
        std::vector<process::SrcidConfig> src_cfg;
        for (size_t i = 0; i < conds.srcids_size(); i++) {
            src_cfg.push_back(conds.srcids(i));
        }
        if (!newitem->addrule_srcid(src_cfg)) {
            response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
            response->set_error("addrule_srcid fail");
            done->Run();
        }
    }
    if (conds.sewords_size()) {
        std::vector<process::ScanCondSeWord> seword_cfg;
        for (size_t i = 0; i < conds.sewords_size(); i++) {
            seword_cfg.push_back(conds.sewords(i));
        }
        if (!newitem->addrule_seword(seword_cfg)) {
            response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
            response->set_error("addrule_seword fail");
            done->Run();
        }
    }
    if (conds.lbs_size()) {
        std::vector<process::LocationInfo> lbs_cfg;
        for (size_t i = 0; i < conds.lbs_size(); i++) {
            lbs_cfg.push_back(conds.lbs(i));
        }
        if (!newitem->addrule_lbs(lbs_cfg)) {
            response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
            response->set_error("addrule_lbs fail");
            done->Run();
        }
    }
    if (conds.cookie_size()) {
        std::vector<process::CookieConfig> cookie_cfg;
        for (size_t i = 0; i < conds.cookie_size(); i++) {
            cookie_cfg.push_back(conds.cookie(i));
        }
        if (!newitem->addrule_cookie(cookie_cfg)) {
            response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
            response->set_error("addrule_cookie fail");
            done->Run();
        }
    }
    if (conds.cluster_size()) {
        std::vector<process::ClusterConfig> cluster_cfg;
        for (size_t i = 0; i < conds.cluster_size(); i++) {
            cluster_cfg.push_back(conds.cluster(i));
        }
        if (!newitem->addrule_cluster(cluster_cfg)) {
            response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
            response->set_error("addrule_scluster fail");
            done->Run();
        }
    }
    if (conds.res_srcid_size()) {
        std::vector<process::SrcidConfig> res_srcid_cfg;
        for (size_t i = 0; i < conds.res_srcid_size(); i++) {
            res_srcid_cfg.push_back(conds.res_srcid(i));
        }
        if (!newitem->addrule_res_srcid(res_srcid_cfg)) {
            response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
            response->set_error("addrule_res_srcid fail");
            done->Run();
        }
    }
    
    AddCheckList(application, newitem);
    response->set_status(process::SetCheckListConfigResponse_Res_Status_SUCCEED);
    done->Run();
}

void Process_Impl::GetCheckListConfig (const process::GetCheckListConfigRequest *request,
                                       process::GetCheckListConfigResponse *response,
                                       ::google::protobuf::Closure *done) {
    
}

void Process_Impl::GetLogProcessStat (const process::GetLogProcessStatRequest *request,
                                      process::GetLogProcessStatResponse *response,
                                      ::google::protobuf::Closure *done) {
    
}

};
