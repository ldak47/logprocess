#include "process_impl.h"

namespace dataprocess {

Process_Impl::Process_Impl (const libconfig::Setting &dbcfg, const std::vector<std::string> &types, int port) {
    std::string curpath = GetCurrPath();
    for (auto type: types) {
        std::string storagecfg_typename = "storage_" + type;
        const libconfig::Setting &cfg = dbcfg[storagecfg_typename.c_str()];
        std::string cf, dbpath;
        cfg.lookupValue("cf", cf);
        cfg.lookupValue("data_path", dbpath);
        dbpath = dbpath + "_" + std::to_string(port);
        std::vector<std::string> cfs;
        boost::algorithm::split(cfs, cf, boost::is_any_of("|"));
        //temply do so..... for use mergeoperator, forsake columnfamily, so clear cfs here.
        StorageImpl *storageimpler = new StorageImpl(dbpath);
        storager.insert(make_pair(type, storageimpler));
    }
}

void Process_Impl::CheckAbnormal (std::string type, const process::TransmitRequest *request) {
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

void Process_Impl::Storage (std::string type, std::string key, const std::map<std::string, std::string> &kv, bool debug) {
    std::map<std::string, StorageImpl *>::iterator it = storager.find(type);
    if (it != storager.end()) {
        StorageImpl *storager = it->second;
        if (!storager) {
            return;
        }
        
        char *tmp;
        std::string jsonstream, result("");
        cJSON *root = cJSON_CreateObject();
        std::map<std::string, std::string>::const_iterator iter = kv.begin();
        for (; iter != kv.end(); iter++) {
            std::string k = iter->first, v = iter->second;
            cJSON_AddStringToObject(root, k.c_str(), v.c_str());
        }
        tmp = cJSON_Print(root);
        jsonstream = tmp;
        cJSON_Delete(root);
        free(tmp);
        for (size_t i = 0; i < jsonstream.length(); i++) {
            if (jsonstream[i] != '\n' && jsonstream[i] != '\t') {
                result += jsonstream[i];
            }
        }
        
        if (debug) {
            LOG(INFO) << key;
            LOG(INFO) << result;
        }
        storager->Write(key, result);
    }
}

std::vector<std::string> Process_Impl::Read (std::string type, time_t start, time_t end) {
    std::map<std::string, StorageImpl *>::iterator it = storager.find(type);
    if (it != storager.end()) {
        StorageImpl *storager = it->second;
        if (!storager) {
            return {};
        }

        std::vector<std::string> res;
        if (end == -1) {
            std::string result;
            storager->SingleGet(std::to_string(start), result);
            if (result != "") {
                res.push_back(result);
            }
        } else {
            std::vector<std::string> results;
            storager->RangeGet(std::to_string(start), std::to_string(end), results);
            res = results;
        }

        return res;
    }
}

std::vector<std::map<std::string, std::string>> Process_Impl::ReadFromRocksdb (std::string application, time_t start, time_t end) {
    std::vector<std::map<std::string, std::string>> kvs;

    std::vector<std::string> res = Read(application, start, end);
    if (res.size()) {
        for (auto r: res) {
            std::vector<std::string> items;
            boost::algorithm::split(items, r, boost::is_any_of("|"));
            try {
                for (auto i: items) {
                    std::map<std::string, std::string> kv;
                    cJSON *root = cJSON_Parse(i.c_str());
                    int size = cJSON_GetArraySize(root);
                    for (int j = 0; j < size; j++) {
                        cJSON *item = cJSON_GetArrayItem(root, j);
                        std::string key = item->string, value;
                        if (item) {
                            value = cJSON_Print(item);
                            value = value.substr(1, value.length() - 2);
                            kv.insert(make_pair(key, value));
                        }
                    }

                    cJSON_Delete(root);
                    kvs.push_back(kv);
                }
            } catch (std::exception &err) {
                LOG(WARNING) << "ReadFromRocksdb " << start << " to " << end << " fail with err " << err.what();
                std::vector<std::map<std::string, std::string>> empty = {};
                return empty;
                    
            }
        }
    }

    return kvs;
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
    //from proto trans to map
    std::map<std::string, std::string> kv = request2kv(request);

    //here actual process...
    //1 checklist process
    CheckAbnormal(kv["application"], request);

    //2 storage process
    LOG(INFO) << kv["timestamp"];
    LOG(INFO) << strTime2Unixtime(kv["timestamp"]);
    Storage(kv["application"], std::to_string(strTime2Unixtime(kv["timestamp"])), kv, true);
    
    //3 stat process
    
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
    time_t st = request->startime(), ed = request->endtime();
    std::vector<std::map<std::string, std::string>> result = ReadFromRocksdb("weather", st, ed);
    for (auto r: result) {
        std::map<std::string, std::string>::iterator it = r.begin();
        for (; it != r.end(); it++) {
            LOG(INFO) << it->first << ": " << it->second;
        }
    }

    done->Run();
}

};
