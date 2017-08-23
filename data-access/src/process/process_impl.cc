#include "process_impl.h"

namespace dataprocess {

Process_Impl::Process_Impl (const libconfig::Setting &dbcfg, 
                            const libconfig::Setting &rdscfg, 
                            const std::vector<std::string> &types, 
                            int port,
                            const std::string &accessid,
                            const std::string &dbpath,
                            const std::string &plathttpaddr): plathttpaddr_(plathttpaddr) {
    if (!StaterInit(rdscfg)) {
        LOG(ERROR) << "StaterInit fail.";
        return;
    }
    alarmsender.reset(new ThreadPool(2));
    if (!alarmsender.get()) {
        LOG(ERROR) << "alarmsender reset fail.";
        return;
    }
    
    std::string curpath = GetCurrPath();
    for (auto type: types) {
        std::string datapath = dbpath + type + "." + accessid + "." + std::to_string(port);
        //std::vector<std::string> cfs;
        //boost::algorithm::split(cfs, cf, boost::is_any_of("|"));
        //temply do so..... for use mergeoperator, forsake columnfamily, so clear cfs here.
        LOG(INFO) << datapath;
        StorageImpl *storageimpler = new StorageImpl(datapath);
        storager.insert(make_pair(type, storageimpler));
    }
}

bool Process_Impl::StaterInit (const libconfig::Setting &rdscfg) {
    //cookie stat temply not impl!
    stater.reset(new ProcessStater(rdscfg));
    if (!stater.get() || "" != stater->GetError()) {
        return false;
    }
 
    stater->AddStater("query frequence stat", QueryFreqStat::UpdataQueryFreqStat);
    //stater->AddStater("Cookie detail stat", CookieStat::UpdateCookieStat);
    return true;
}

std::string Process_Impl::MakeAlarmInfo (const std::map<std::string, std::string> &kv,
                                         const std::string &description) {
    char *tmp;
    std::string jsonstream, result("");
    cJSON *root = cJSON_CreateObject();
    std::map<std::string, std::string>::const_iterator iter = kv.begin();
    for (; iter != kv.end(); iter++) {
        std::string k = iter->first, v = iter->second;
        cJSON_AddStringToObject(root, k.c_str(), v.c_str());
    }
    cJSON_AddStringToObject(root, "description", description.c_str());
    tmp = cJSON_Print(root);
    jsonstream = tmp;
    cJSON_Delete(root);
    free(tmp);
    for (size_t i = 0; i < jsonstream.length(); i++) {
        if (jsonstream[i] != '\n' && jsonstream[i] != '\t') {
            result += jsonstream[i];
        }
    }
        
    return result;
}

void Process_Impl::CheckAbnormal (std::string type, const process::TransmitRequest *request, const std::map<std::string, std::string> &kv) {
    std::map<std::string, std::vector<checklist_item *>>::iterator it = checklists.find(type);
    std::string description = "";
    if (it != checklists.end()) {
        for (auto i: it->second) {
            if (i->check(request, description)) {
                //asyanc to curl to platform
                auto task = std::function<void () >([this, kv, description] {
                    std::string alarminfo = MakeAlarmInfo(kv, description), response("");
                    uint32_t alarmid = common::FNV1_32_HASH(alarminfo);
                    int rescode = 0, try_times = 20;
                    LOG(ERROR) << "detect abnormal by checklist alarminfo[" << alarmid << "]\t" << alarminfo;

                    common::HttpClient cli;
                    std::string plathttpaddr_alarm = plathttpaddr_ + "/alarm";
                    while ((rescode = cli.Post(plathttpaddr_alarm, alarminfo, response)) && try_times) {
                        --try_times;
                    }
                    if (rescode) {
                        LOG(ERROR) << "send alarm " << plathttpaddr_alarm << " to platform fail with LIBCURL errcode " << rescode;
                    }
                });

                alarmsender->AddTask(task);
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

    return {};
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
                    if (!root) {
                        continue;
                    }
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
    //common::TimeCalcer tc;
    std::map<std::string, std::string> kv = request2kv(request);

    //here actual process...
    //1 checklist process
    CheckAbnormal(kv["application"], request, kv);

    //2 storage process
    Storage(kv["application"], std::to_string(strTime2Unixtime(kv["timestamp"])), kv);
    
    //3 stat process
    stater->Run(kv);

    if (kv["srcid"] == "4514" && kv["resno"] == "") {
        uint32_t cnt = 0;
        std::string q = kv["query"], s = kv["srcid"];
        QueryFreqStat::SearchExists(q, s, cnt);
        if (!cnt) {
            auto task = std::function<void () >([this, q] {
                common::HttpClient cli;
                std::string plathttpaddr_alarm = plathttpaddr_ + "/kuozhao", response;
                char *tmp;
                std::string jsonstream;
                cJSON *root = cJSON_CreateObject();
                cJSON_AddStringToObject(root, "query", q.c_str());
                tmp = cJSON_Print(root);
                jsonstream = tmp;
                cJSON_Delete(root);
                free(tmp);
                cli.Post(plathttpaddr_alarm, jsonstream, response);
            });

            alarmsender->AddTask(task);
        }
    }
    
    response->set_status(process::TransmitResponse_Res_Status_SUCCEED);
    //double tm = tc.Calc();
    //LOG(INFO) << "cost " << tm;
    done->Run();
}

checklist_item *Process_Impl::MakeCheckListItem (const process::CheckListCond &conds,
                                                 process::SetCheckListConfigResponse *response) {
    std::string description = conds.description();
    checklist_item *newitem;
    try {
        newitem = new checklist_item(description);
    } catch (std::exception &err) {
        LOG(ERROR) << "SetCheckListConfig alloc new checklist_item fail with reason " << err.what();
        if (response) {
            response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
            response->set_error("addrule_srcid alloc new checklist_item fail");
        }
        return nullptr;
    }
    
    if (conds.srcids_size()) {
        std::vector<process::SrcidConfig> src_cfg;
        for (size_t i = 0; i < conds.srcids_size(); i++) {
            src_cfg.push_back(conds.srcids(i));
        }
        if (!newitem->addrule_srcid(src_cfg)) {
            if (response) {
                response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
                response->set_error("addrule_srcid fail");
            }
            if (newitem) {
                delete newitem;
                return nullptr;
            }
        }
    }
    if (conds.sewords_size()) {
        std::vector<process::ScanCondSeWord> seword_cfg;
        for (size_t i = 0; i < conds.sewords_size(); i++) {
            seword_cfg.push_back(conds.sewords(i));
        }
        if (!newitem->addrule_seword(seword_cfg)) {
            if (response) {
                response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
                response->set_error("addrule_seword fail");
            }
            if (newitem) {
                delete newitem;
                return nullptr;
            }
        }
    }
    if (conds.lbs_size()) {
        std::vector<process::LocationInfo> lbs_cfg;
        for (size_t i = 0; i < conds.lbs_size(); i++) {
            lbs_cfg.push_back(conds.lbs(i));
        }
        if (!newitem->addrule_lbs(lbs_cfg)) {
            if (response) {
                response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
                response->set_error("addrule_lbs fail");
            }
            if (newitem) {
                delete newitem;
                return nullptr;
            }
        }
    }
    if (conds.cookie_size()) {
        std::vector<process::CookieConfig> cookie_cfg;
        for (size_t i = 0; i < conds.cookie_size(); i++) {
            cookie_cfg.push_back(conds.cookie(i));
        }
        if (!newitem->addrule_cookie(cookie_cfg)) {
            if (response) {
                response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
                response->set_error("addrule_cookie fail");
            }
            if (newitem) {
                delete newitem;
                return nullptr;
            }
        }
    }
    if (conds.cluster_size()) {
        std::vector<process::ClusterConfig> cluster_cfg;
        for (size_t i = 0; i < conds.cluster_size(); i++) {
            cluster_cfg.push_back(conds.cluster(i));
        }
        if (!newitem->addrule_cluster(cluster_cfg)) {
            if (response) {
                response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
                response->set_error("addrule_scluster fail");
            }
            if (newitem) {
                delete newitem;
                return nullptr;
            }
        }
    }
    if (conds.resno_size()) {
        std::vector<process::ResnoConfig> resno_cfg;
        for (size_t i = 0; i < conds.resno_size(); i++) {
            resno_cfg.push_back(conds.resno(i));
        }
        if (!newitem->addrule_resno(resno_cfg)) {
            if (response) {
                response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
                response->set_error("addrule_resno fail");
            }
            if (newitem) {
                delete newitem;
                return nullptr;
            }
        }
    }

    if (conds.sid_size()) {
        std::vector<process::SidConfig> sid_cfg;
        for (size_t i = 0; i < conds.sid_size(); i++) {
            sid_cfg.push_back(conds.sid(i));
        }

        if (!newitem->addrule_sid(sid_cfg)) {
            if (response) {
                response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
                response->set_error("addrule_sid fail");
            }
            if (newitem) {
                delete newitem;
                return nullptr;
            }
        }
    }

    if (conds.type_size()) {
        std::vector<process::TypeConfig> type_cfg;
        for (size_t i = 0; i < conds.type_size(); i++) {
            type_cfg.push_back(conds.type(i));
        }

        if (!newitem->addrule_type(type_cfg)) {
            if (response) {
                response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
                response->set_error("addrule_type fail");
            }
            if (newitem) {
                delete newitem;
                return nullptr;
            }
        }
    }
 
    return newitem;   
}

void Process_Impl::SetCheckListConfig (const process::SetCheckListConfigRequest *request,
                                       process::SetCheckListConfigResponse *response,
                                       ::google::protobuf::Closure *done) {
    bool op = request->op();
    std::string application = request->application();
    
    if (!op) {
        if (request->has_index()) {
            uint32_t index = request->index();
            if (index > 0) {
                DelCheckList(application, index - 1);
                response->set_status(process::SetCheckListConfigResponse_Res_Status_SUCCEED);
            } else {
                response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
                response->set_error("invalid index");
            }
        } else {
            response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
            response->set_error("missing index");
        }
    } else {
        process::CheckListCond conds = request->cond();
        checklist_item *newitem = MakeCheckListItem(conds, response);
        if (!newitem) {
            response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
        } else {
            bool flag(false);
            std::vector<checklist_item *> current = GetCheckList(application);
            for (auto i: current) {
                if (newitem && i && *i == *newitem) {
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                AddCheckList(application, newitem);
                response->set_status(process::SetCheckListConfigResponse_Res_Status_SUCCEED);
            } else {
                delete newitem;
                response->set_status(process::SetCheckListConfigResponse_Res_Status_FAILED);
                response->set_error("item already exist");
            }
        }
    }
        
    done->Run();
}

void Process_Impl::FillCondition (checklist_item *item, process::CheckListCond *cond) {
    std::vector<process::SrcidConfig> srcidcfg = item->getrule_srcid();
    for (auto i: srcidcfg) {
        process::SrcidConfig *cfg = cond->add_srcids();
        cfg->set_srcid(i.srcid());
        cfg->set_cond(i.cond());
    }
    std::vector<process::ScanCondSeWord> sewordcfg = item->getrule_seword();
    for (auto i: sewordcfg) {
        process::ScanCondSeWord *cfg = cond->add_sewords();
        cfg->set_searchword(i.searchword());
        cfg->set_cond(i.cond());
    }
    std::vector<process::LocationInfo> lbscfg = item->getrule_lbs();
    for (auto i: lbscfg) {
        process::LocationInfo *cfg = cond->add_lbs();
        cfg->set_province(i.province());
        cfg->set_city(i.city());
        cfg->set_cond(i.cond());
    }
    std::vector<process::CookieConfig> cookiecfg = item->getrule_cookie();
    for (auto i: cookiecfg) {
        process::CookieConfig *cfg = cond->add_cookie();
        cfg->set_cookie(i.cookie());
        cfg->set_cond(i.cond());
    }
    std::vector<process::ClusterConfig> clustercfg = item->getrule_cluster();
    for (auto i: clustercfg) {
        process::ClusterConfig *cfg = cond->add_cluster();
        cfg->set_cluster(i.cluster());
        cfg->set_cond(i.cond());
    }
    std::vector<process::SidConfig> sidconfig = item->getrule_sid();
    for (auto i: sidconfig) {
        process::SidConfig *cfg = cond->add_sid();
        cfg->set_sid(i.sid());
        cfg->set_cond(i.cond());
    }
    std::vector<process::ResnoConfig> resnocfg = item->getrule_resno();
    for (auto i: resnocfg) {
        process::ResnoConfig *cfg = cond->add_resno();
        cfg->set_resno(i.resno());
        cfg->set_cond(i.cond());
    }
    std::vector<process::TypeConfig> typecfg = item->getrule_type();
    for (auto i: typecfg) {
        process::TypeConfig *cfg = cond->add_type();
        cfg->set_type(i.type());
        cfg->set_cond(i.cond());
    }
}

void Process_Impl::GetCheckListConfig (const process::GetCheckListConfigRequest *request,
                                       process::GetCheckListConfigResponse *response,
                                       ::google::protobuf::Closure *done) {
    std::string application = request->application();
    std::vector<checklist_item *> checklists = GetCheckList(application);

    uint32_t index = request->index();
    if (index > 0 && index < checklists.size()) {
        checklist_item *item = checklists[index - 1];
        process::CheckListCond *cond = response->add_cond();
        FillCondition(item, cond);
        response->set_status(process::GetCheckListConfigResponse_Res_Status_SUCCEED);
        done->Run();
    } else if (index == 0) {
        for (auto i: checklists) {
            process::CheckListCond *cond = response->add_cond();
            FillCondition(i, cond);
        }
        response->set_status(process::GetCheckListConfigResponse_Res_Status_SUCCEED);
        done->Run();
    }
}

void Process_Impl::ParseSelectField (const process::GetLogProcessStatRequest *request, 
                                     std::vector<std::string> &selectfields) {
    int selectfieldnum = request->selectfields_size();
    if (!selectfieldnum) {
        selectfields.clear();
    } else {
        for (int i = 0; i < selectfieldnum; i++) {
            std::string selectfield = request->selectfields(i);
            selectfields.push_back(selectfield);
        }
    }
}

void Process_Impl::SelectByConds (const std::vector<std::map<std::string, std::string>> &raw,
                                  std::vector<std::map<std::string, std::string>> &res,
                                  const process::CheckListCond &conds, 
                                  std::vector<std::string> selectfield) {
    for (auto i: raw) {
        if (all_check(conds, i)) {
            if (selectfield.size()) {
                std::map<std::string, std::string> newitem;
                for (auto j: selectfield) {
                    std::map<std::string, std::string>::iterator it = i.find(j);
                    if (it != i.end()) {
                        newitem.insert(make_pair(j, it->second));
                    }
                }
                if (newitem.size() == selectfield.size()) {
                    res.push_back(newitem);
                }
            } else {
                if (i.size()) {
                    res.push_back(i);
                }
            }
        }
    }
}

void Process_Impl::GetLogProcessStat (const process::GetLogProcessStatRequest *request,
                                      process::GetLogProcessStatResponse *response,
                                      ::google::protobuf::Closure *done) {
    LOG(INFO) << "ParseSelectField size: " << request->selectfields_size();
    common::TimeCalcer tc;
    double tm;
    //judge request type(from rocksdb/redis/trie/......)
    process::GetLogProcessStatRequest_SourceFrom source = request->getfrom();

    //process...
    if (source == process::GetLogProcessStatRequest_SourceFrom_Rocksdb) {
        //1 get raw result by timeval first. if raw result is empty, return at once.
        std::string application = request->application();
        time_t st = request->startime(), ed = request->endtime();
        std::vector<std::map<std::string, std::string>> raw_result = ReadFromRocksdb(application, st, ed);
        tm = tc.Calc();
        LOG(INFO) << std::fixed << "raw size: " << raw_result.size() << " cost " << tm;
        if (!raw_result.size()) {
            response->set_count(0);
            response->set_status(process::GetLogProcessStatResponse_Res_Status_SUCCEED);
            done->Run();
            return;
        }

        //2 parse selectfields. if selectfields is empty means *(select )
        tc.restart();
        std::vector<std::string> selectfields;
        ParseSelectField(request, selectfields);
        tm = tc.Calc();
        LOG(INFO) << std::fixed << "ParseSelectField  cost " << tm;
        LOG(INFO) << "ParseSelectField size: " << selectfields.size();

        //3 select by filter from conds, and select fields.(select xxx from xxx where )
        tc.restart();
        std::vector<std::map<std::string, std::string>> res_result;
        SelectByConds(raw_result, res_result, request->cond(), selectfields);
        tm = tc.Calc();
        LOG(INFO) << std::fixed << "res_result size: " << res_result.size() << ", SelectByConds  cost " << tm;
        raw_result.clear();

        //3.5 maybe need sort(order by )
        tc.restart();
        uint32_t start = -1, end = -1;
        std::vector<std::map<std::string, std::string>> sorted_result;
        if (request->has_sortfield()) {
            std::string sortfield = request->sortfield();
            if (request->has_start() && request->has_end()) {
                start = request->start();
                end = request->end();
            }
            
            if (selectfields.size() && selectfields.end() == std::find(selectfields.begin(), selectfields.end(), sortfield)) {
                sorted_result = res_result;
            } else if (end > 100000 && res_result.size() > 100000) {
                //topk
                if (sortfield == "query") {
                    Topk topk(res_result, end, QueryCompare);
                    sorted_result = topk.Run("query");
                } else if (sortfield == "srcid") {
                    Topk topk(res_result, end, SrcidCompare);
                    sorted_result = topk.Run("srcid");
                } else if (sortfield == "timestamp") {
                    Topk topk(res_result, end, TimestampCompare);
                    sorted_result = topk.Run("timestamp");
                }
            } else {
                //qsort
                LOG(INFO) << "SORT";
                if (sortfield == "query") {
                    std::sort(res_result.begin(), res_result.end(), QueryCompare);
                } else if (sortfield == "srcid") {
                    std::sort(res_result.begin(), res_result.end(), SrcidCompare);
                } else if (sortfield == "timestamp") {
                    std::sort(res_result.begin(), res_result.end(), TimestampCompare);
                }
                reverse(res_result.begin(), res_result.end());
                sorted_result = res_result;
            }
        } else {
            sorted_result = res_result;
        }
        res_result.clear();
        //3.6 maybe need cut [st, ed](limit )
        if (start != -1 && end != -1) {
            if (0 <= start && start <= end && end < sorted_result.size()) {
                std::vector<std::map<std::string, std::string>> tmp;
                for (size_t i = start; i <= end; i++) {
                    tmp.push_back(sorted_result[i]);
                }
                sorted_result = tmp;
            }
        }
        tm = tc.Calc();
        LOG(INFO) << std::fixed << "sorted_result size: " << sorted_result.size() << ", Sort and limit  cost " << tm;
        
        
        //4 field response
        LOG(INFO) << "res size: " << sorted_result.size();
        tc.restart();
        response->set_count(sorted_result.size());
        response->set_status(process::GetLogProcessStatResponse_Res_Status_SUCCEED);
        for (auto i: sorted_result) {
            std::map<std::string, std::string> item = i;
            std::map<std::string, std::string>::iterator it = item.begin();
            process::MapEntries *newitem = response->add_iteminfos();
            for (; it != item.end(); it++) {
                process::MapEntry *field = newitem->add_item();
                field->set_key(it->first);
                field->set_value(it->second);
            }
        }
        tm = tc.Calc();
        LOG(INFO) << std::fixed << "field response  cost " << tm;
    } else if (source == process::GetLogProcessStatRequest_SourceFrom_QueryTrie) {
        if (!request->has_querystr() || !request->has_srcid()) {
            response->set_status(process::GetLogProcessStatResponse_Res_Status_FAILED);
            response->set_error("has not querystr or srcid");
        } else {
            std::string query = request->querystr();
            std::string srcid = request->srcid();
            uint32_t cnt = 0;
            QueryFreqStat::SearchExists(query, srcid, cnt);
            response->set_count(static_cast<uint64_t>(cnt));
            response->set_status(process::GetLogProcessStatResponse_Res_Status_SUCCEED);
        }
        done->Run();
        return;
    } else if (source == process::GetLogProcessStatRequest_SourceFrom_Redis) {
        if (!request->has_cookieid()) {
            response->set_status(process::GetLogProcessStatResponse_Res_Status_FAILED);
            response->set_error("has not cookieid");
        } else {
            uint64_t st = request->startime(), ed = request->endtime();
            std::string cookieid = request->cookieid();
            std::vector<query_record> records;
            CookieStat::Search(cookieid, records, static_cast<time_t>(st), static_cast<time_t>(ed));
            for (auto i: records) {
                time_t tm = std::get<0>(i);
                std::string strtime = unixTime2Str(tm);
                std::string query = std::get<1>(i);
                process::MapEntries *newitem = response->add_iteminfos();
                process::MapEntry *field = newitem->add_item();
                field->set_key(strtime);
                field->set_value(query);
            }
            response->set_count(static_cast<uint64_t>(records.size()));
            response->set_status(process::GetLogProcessStatResponse_Res_Status_SUCCEED);
        }
    }

    done->Run();
}

};
