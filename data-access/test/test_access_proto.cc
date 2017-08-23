#include "gtest/gtest.h"
#include "glog/logging.h"
#include "client_hook.h"
#include <string>

bool OpLegalSrcid (bool op, const std::string &srcid) {
    std::unique_ptr<RpcAdaptClient::ManageRpcClient::ManageClientHook> client;
    client.reset(new RpcAdaptClient::ManageRpcClient::ManageClientHook("127.0.0.1:30000", 100000));
    
    manage::SetLogFilterConfigRequest request;
    manage::SetLogFilterConfigResponse response;
    request.set_op(op);
    request.set_srcid(srcid);

    bool res = client->SetLogFilterConfig(&request, &response, 0);
    LOG(INFO) << response.status();
    return res;
}

bool GetLegalSrcids (std::vector<std::string> &srcids) {
    std::unique_ptr<RpcAdaptClient::ManageRpcClient::ManageClientHook> client;
    client.reset(new RpcAdaptClient::ManageRpcClient::ManageClientHook("127.0.0.1:30000", 100000));
    
    manage::GetLogFilterConfigRequest request;
    manage::GetLogFilterConfigResponse response;

    bool res = client->GetLogFilterConfig(&request, &response, 0);
    for (auto i: response.srcids()) {
        srcids.push_back(i);
    }
    return res;
}

TEST (test, filter) {
    //raw
    std::vector<std::string> legalsrcids;
    LOG(INFO) << GetLegalSrcids(legalsrcids);
    for (auto i: legalsrcids) {
        LOG(INFO) << i;
    }

    //add
    std::string legalsrcid = "4515";
    LOG(INFO) << OpLegalSrcid(true, legalsrcid);
    legalsrcids.clear();
    LOG(INFO) << GetLegalSrcids(legalsrcids);
    for (auto i: legalsrcids) {
        LOG(INFO) << i;
    }

    //del
    legalsrcid = "4178";
    LOG(INFO) << OpLegalSrcid(false, legalsrcid);
    legalsrcids.clear();
    LOG(INFO) << GetLegalSrcids(legalsrcids);
    for (auto i: legalsrcids) {
        LOG(INFO) << i;
    }

    //del
    legalsrcid = "4186";
    LOG(INFO) << OpLegalSrcid(false, legalsrcid);
    legalsrcids.clear();
    LOG(INFO) << GetLegalSrcids(legalsrcids);
    for (auto i: legalsrcids) {
        LOG(INFO) << i;
    }

    //del
    legalsrcid = "4494";
    LOG(INFO) << OpLegalSrcid(false, legalsrcid);
    legalsrcids.clear();
    LOG(INFO) << GetLegalSrcids(legalsrcids);
    for (auto i: legalsrcids) {
        LOG(INFO) << i;
    }

    //del
    legalsrcid = "4166";
    LOG(INFO) << OpLegalSrcid(false, legalsrcid);
    legalsrcids.clear();
    LOG(INFO) << GetLegalSrcids(legalsrcids);
    for (auto i: legalsrcids) {
        LOG(INFO) << i;
    }

    
}

bool GetField (std::vector<std::string> &fields) {
    std::unique_ptr<RpcAdaptClient::ManageRpcClient::ManageClientHook> client;
    client.reset(new RpcAdaptClient::ManageRpcClient::ManageClientHook("127.0.0.1:30000", 100000));
    
    manage::GetLogFieldConfigRequest request;
    manage::GetLogFieldConfigResponse response;
    
    request.set_application("weather");
    bool res = client->GetLogFieldConfig(&request, &response, 0);
    for (int i = 0; i < response.fieldconfigs_size(); i++) {
        manage::FiledConfig item = response.fieldconfigs(i);
        LOG(INFO) << item.field() << ", " << item.regex() << ", " << item.istrans();
    }

    return res;
}

bool SetField (bool op, std::string field, std::string regex, bool istrans) {
    std::unique_ptr<RpcAdaptClient::ManageRpcClient::ManageClientHook> client;
    client.reset(new RpcAdaptClient::ManageRpcClient::ManageClientHook("127.0.0.1:30000", 100000));
    
    manage::SetLogFieldConfigRequest request;
    manage::SetLogFieldConfigResponse response;
    
    request.set_application("weather");
    request.set_op(op);
    request.mutable_fieldconfigs()->set_field(field.c_str());
    request.mutable_fieldconfigs()->set_regex(regex.c_str());
    request.mutable_fieldconfigs()->set_istrans(istrans);
    
    bool res = client->SetLogFieldConfig(&request, &response, 0);
    return res;
}

TEST (test, field) {
    std::vector<std::string> fields;
    LOG(INFO) << GetField(fields);
    for (auto i: fields) {
        LOG(INFO) << i;
    }

    //update
    LOG(INFO) << SetField(true, "qid", "xxxxx", true);
    fields.clear();
    LOG(INFO) << GetField(fields);
    for (auto i: fields) {
        LOG(INFO) << i;
    }

    //Add
    LOG(INFO) << SetField(true, "newadd", "yyyyyxxxxx", false);
    fields.clear();
    LOG(INFO) << GetField(fields);
    for (auto i: fields) {
        LOG(INFO) << i;
    }

    //DEL
    LOG(INFO) << SetField(false, "srcid", "", false);
    fields.clear();
    LOG(INFO) << GetField(fields);
    for (auto i: fields) {
        LOG(INFO) << i;
    }

}

void GetWeatherPv (std::string application) {
    std::unique_ptr<RpcAdaptClient::ManageRpcClient::ManageClientHook> client;
    client.reset(new RpcAdaptClient::ManageRpcClient::ManageClientHook("127.0.0.1:30000", 100000));
    
    manage::GetLogTransmitStatRequest request;
    manage::GetLogTransmitStatResponse response;

    request.set_application(application);
    request.set_startime(1500989700);
    request.set_endtime(1500993300);

    LOG(INFO) << client->GetLogTransmitStat(&request, &response, 0);
    LOG(INFO) << response.status();
    //LOG(INFO) << response.num();
}

TEST (test, pv) {
    GetWeatherPv("weather");
    GetWeatherPv("express");
}

int main (int argc, char *argv[]) {
    google::SetStderrLogging(google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}
