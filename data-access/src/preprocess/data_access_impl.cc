#include "data_access_impl.h"

namespace dataaccess {

extern std::shared_ptr<boost::lockfree::queue<void *, boost::lockfree::fixed_sized<true>>> lockfreequeue_;
extern std::shared_ptr<boost::lockfree::queue<void *, boost::lockfree::fixed_sized<true>>> lockfreequeue_new_;

Access_Impl::Access_Impl (PvStater &pvstater,
                          DataFieldRuler &datafieldruler,
                          AccessFilter &accessfilter,
                          bool &switch_old,
                          bool &switch_new): pvstater_(pvstater),
                                             datafieldruler_(datafieldruler),
                                             accessfilter_(accessfilter),
                                             switch_old_(switch_old),
                                             switch_new_(switch_new) {
    
}

void Access_Impl::SetLogFilterConfig (
                                const manage::SetLogFilterConfigRequest *request,
                                manage::SetLogFilterConfigResponse *response,
                                ::google::protobuf::Closure *done
                                ) {
    bool op = request->op();
    std::string srcid = request->srcid();
    if (!op) {
        accessfilter_.Del(srcid);
    } else {
        accessfilter_.Add(srcid);
    }

    response->set_status(manage::SetLogFilterConfigResponse_Res_Status_SUCCEED);
    done->Run();
}

void Access_Impl::GetLogFilterConfig(
                                const manage::GetLogFilterConfigRequest *request,
                                manage::GetLogFilterConfigResponse *response,
                                ::google::protobuf::Closure *done
                               ) {
    std::set<std::string> srcids = accessfilter_.Get();
    for (auto i: srcids) {
        response->add_srcids(i);
    }
    
    response->set_status(manage::GetLogFilterConfigResponse_Res_Status_SUCCEED);
    done->Run();
}

void Access_Impl::SetLogFieldConfig (
                                const manage::SetLogFieldConfigRequest *request,
                                manage::SetLogFieldConfigResponse *response,
                                ::google::protobuf::Closure *done
                               ) {
    bool op = request->op();
    std::string application = request->application();
    manage::FiledConfig item = request->fieldconfigs();

    std::string field = item.field(), regex = item.regex();
    bool istrans = item.istrans();

    datafieldruler_.UpdateConfig(application, field, op, regex, istrans);
    response->set_status(manage::SetLogFieldConfigResponse_Res_Status_SUCCEED);
    done->Run();
}

void Access_Impl::GetLogFieldConfig (
                               const manage::GetLogFieldConfigRequest *request,
                               manage::GetLogFieldConfigResponse *response,
                               ::google::protobuf::Closure *done
                               ) {
    std::string application = request->application(), field("");
    if (request->has_field() && request->field() != "") {
        field = request->field();
    }

    std::map<std::string, std::string> regexmap = datafieldruler_.GetRule(application);
    std::map<std::string, bool> istransmap = datafieldruler_.GetIsTrans(application);
    if (regexmap.size() != istransmap.size()) {
        response->set_status(manage::GetLogFieldConfigResponse_Res_Status_FAILED);
        response->set_error("istransmap size not equal regexmap size");
        done->Run();
    } else {
        if (field == "") {
            int num = regexmap.size();
            std::map<std::string, std::string>::iterator it = regexmap.begin();
            for (; it != regexmap.end(); it++) {
                std::string field = it->first, regex = it->second;
                bool istrans = istransmap.find(field)->second;
                manage::FiledConfig *fcg = response->add_fieldconfigs();
                fcg->set_field(field);
                fcg->set_regex(regex);
                fcg->set_istrans(istrans);
            }
            response->set_status(manage::GetLogFieldConfigResponse_Res_Status_SUCCEED);
            done->Run();
        } else {
            int num = regexmap.size();
            bool flag = false;
            std::map<std::string, std::string>::iterator it = regexmap.begin();
            for (; it != regexmap.end(); it++) {
                std::string _field = it->first, regex = it->second;
                if (field != _field) {
                    continue;
                }
                bool istrans = istransmap.find(_field)->second;
                manage::FiledConfig *fcg = response->add_fieldconfigs();
                fcg->set_field(_field);
                fcg->set_regex(regex);
                fcg->set_istrans(istrans);
                flag = true;
                response->set_status(manage::GetLogFieldConfigResponse_Res_Status_FAILED);
                done->Run();
            }
            if (!flag) {
                response->set_status(manage::GetLogFieldConfigResponse_Res_Status_FAILED);
                std::string errmsg = "wrong field name ";
                errmsg += field;
                response->set_error(errmsg.c_str());
                done->Run();
            }
        }
    }
}

void Access_Impl::GetLogTransmitStat (
                                const manage::GetLogTransmitStatRequest *request,
                                manage::GetLogTransmitStatResponse *response,
                                ::google::protobuf::Closure *done
                                ) {
    std::string application = request->application();
    time_t st = static_cast<time_t>(request->startime());
    int64_t ed = static_cast<time_t>(request->endtime());
    std::map<std::string, int> cnt = pvstater_.Cnt(application, st, ed);

    for (std::map<std::string, int>::iterator it = cnt.begin(); it != cnt.end(); it++) {
        std::string key = it->first, count = std::to_string(it->second);
        manage::MapEntry *item = response->add_cnt();
        item->set_key(key);
        item->set_value(count);
    }
    response->set_status(manage::GetLogTransmitStatResponse_Res_Status_SUCCEED);
    done->Run();
}

void Access_Impl::AddLogTransmitStat (
                                const manage::AddLogTransmitStatRequest *request,
                                manage::AddLogTransmitStatResponse *response,
                                ::google::protobuf::Closure *done
                                ) {
    std::string application = request->application(), newsrcid = request->srcid();
    pvstater_.AddSupportType(application, newsrcid);
    response->set_status(manage::AddLogTransmitStatResponse_Res_Status_SUCCEED);
    done->Run();
}

void Access_Impl::PullSwitchConfig (
                                    const manage::PullSwitchConfigRequest *request,
                                    manage::PullSwitchConfigResponse *response,
                                    ::google::protobuf::Closure *done
                                   ) {
    uint32_t idx = request->idx();
    bool op = request->op();
    if (idx == 0) {
        switch_old_ = op;
    } else if (idx == 1) {
        switch_new_ = op;
    }
    done->Run();
}

void Access_Impl::Retransmit(
                            const manage::RetransmitRequest *request,
                            manage::RetransmitResponse *response,
                            ::google::protobuf::Closure *done
                            ) {
    bool type = request->type();
    std::string msg = request->package();
    RawBody *newbody = new RawBody(msg);
    if (type) {
        lockfreequeue_new_->push((void *)newbody);
    } else {
        lockfreequeue_->push((void *)newbody);
    }

    response->set_status(manage::RetransmitResponse_Res_Status_SUCCEED);
    done->Run();
}

};
