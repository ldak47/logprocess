#include "gtest/gtest.h"
#include "client_hook.h"
#include "glog/logging.h"
#include <chrono>

void InitCallback (sofa::pbrpc::RpcController *cntl, const baidu::minos::InitSubscriberBrokerRequest *request,
                   baidu::minos::InitSubscriberBrokerResponse *response) {
    LOG(INFO) << "cntl resp: " << cntl->ErrorText() << ", ErrorCode: " << cntl->ErrorCode();
    if (response->has_status()) {
        LOG(INFO) << "status: " << (uint32_t)(response->status());
    } else {
        LOG(INFO) << "status is not exist ";
    }
    if (response->has_error_message()) {
        LOG(INFO) << "error_message: " << response->error_message();
    } else {
        LOG(INFO) << "error_message is not exist ";
    }
    cntl->Reset();
    delete cntl;
    cntl = 0;
    return;
}

int64_t GetSubprogressByPartition (RpcAdaptClient::PullDataRpcClient::PullClientHook &client, int partition) {
    baidu::minos::GetPartitionGroupProgressRequest request;
    baidu::minos::GetPartitionGroupProgressResponse response;
    request.set_topic_id(70029159);
    request.set_partition_group_id(partition);
    request.mutable_account()->set_token("minos");

    bool res = client.GetPartitionProgress(&request, &response, 0);
    while (!res || response.status() != baidu::minos::GetPartitionGroupProgressResponse::SUCCEED) {
        LOG(INFO) << "res: " << res << ", status: " << response.status() << ", errormsg: " << response.error_message();
        std::chrono::seconds duration(1);
        std::this_thread::sleep_for(duration);
        res = client.GetPartitionProgress(&request, &response, 0);
    }
    
    return response.pg_progress().cur_data_package_id();
}

bool Subscribe (RpcAdaptClient::PullDataRpcClient::PullClientHook &client, int partition, int64_t packageid = 1) {
    int64_t package_id = packageid;
    while (1) {
        baidu::minos::SubscribeRecordRequest request;
        baidu::minos::SubscribeRecordResponse response;
        request.set_topic_id(70029159);
        request.set_partition_group_id(partition);
        request.mutable_account()->set_token("minos");
        request.set_data_package_id(package_id);
        request.set_record_count(1);
        
        bool res = client.Subscribe(&request, &response, 0);
        if (!res || response.status() != baidu::minos::SubscribeRecordResponse::SUCCEED) {
            if (!res) {
                LOG(INFO) << "subscribe request fail, errormessage";
            } else {
                LOG(INFO) << "subscribe request get data fail, errormessage: " << response.error_message() << ", requestmsg: " << request.ShortDebugString() << ", errorcode: " << response.status() << ", cur_data_package_id: " << response.cur_data_package_id();
            }
            continue;
        }

        for (int i = 0; i < response.data_size(); i++) {
            const baidu::minos::PbRecord record = response.data(i);
            const baidu::minos::PbRecordHeader head = record.header();
            int pipelet_id = head.partition_id();
            uint64_t seq_id = head.record_id();
            int body_len = head.body_length();

            std::string result = "";
            for (int i = 0; i < body_len; i++) {
                if (record.body()[i] == '\n' || record.body()[i] == '\0') {
                    result += ' ';
                } else {
                    result += record.body()[i];
                }
            }
            LOG(INFO) << "result: " << result;
        }
        package_id = response.cur_data_package_id() + 1;
        LOG(INFO) << "package_id: " << package_id << "################################################################";
    }
}

bool Init(RpcAdaptClient::PullDataRpcClient::PullClientHook &client, int partition) {
    baidu::minos::InitSubscriberBrokerRequest init_request;
    baidu::minos::InitSubscriberBrokerResponse init_response;
    init_request.set_topic_id(70029159);
    init_request.set_partition_group_id(partition);
    init_request.set_need_progress(false);
    init_request.mutable_account()->set_token("minos");
    init_request.set_type(baidu::minos::InitSubscriberBrokerRequest_Type_TIMESTAMP);
    init_request.set_start_timestamp(time(0));
    init_request.set_start_record_type(baidu::minos::InitSubscriberBrokerRequest::NEWEST_RECORD);

    bool res = client.InitSub(&init_request, &init_response, 0);
    int try_times = 10;
    while (!res || init_response.status() != baidu::minos::InitSubscriberBrokerResponse::SUCCEED) {
        if (res && init_response.status() == baidu::minos::InitSubscriberBrokerResponse::STARTING) {
            if (!(try_times--)) {
                break;
            }
            std::chrono::seconds duration(10);
            std::this_thread::sleep_for(duration);
            res = client.InitSub(&init_request, &init_response, 0);
            continue;
        }
        std::chrono::seconds duration(1);
        std::this_thread::sleep_for(duration);
        LOG(WARNING) << "retry... rpc: " << res << ", requestmsg: [" << init_request.ShortDebugString() << "], partition: " << partition << ", status: " << init_response.status() << ", errormessage: " << init_response.error_message();
        init_request.set_partition_group_id(partition);
        res = client.InitSub(&init_request, &init_response, 0);
    }
    return res;
}

TEST(test_proto, test_proto_init) {
    RpcAdaptClient::PullDataRpcClient::PullClientHook client("127.0.0.1:425", 3000);
    //EXPECT_EQ(true, Init(client, 1));
    //EXPECT_EQ(true, Subscribe(client, 2));
    //Init(client, 2);
    //int64_t package_id = GetSubprogressByPartition(client, 19);
    //LOG(INFO) << "package_id: " << package_id;
    //Init(client, 17);
    Subscribe(client, 17);
}

TEST(test_proto, test_process) {
    std::unique_ptr<RpcAdaptClient::PushDataRpcClient::PushClientHook> client;
    client.reset(new RpcAdaptClient::PushDataRpcClient::PushClientHook("127.0.0.1:20000", 3000));

    process::SetCheckListConfigRequest request;
    std::string application = "weather";
    process::CheckListCond cond;
    process::SrcidConfig *src_cfg = cond.add_srcids();
    src_cfg->set_srcid("4178");
    src_cfg->set_cond(process::condEqual);
    request.set_application(application);
    request.set_allocated_cond(&cond);
    
    process::SetCheckListConfigResponse response;
    bool res = client->SetCheckListConfig(&request, &response, 0);
    request.release_cond();

    EXPECT_EQ(res, true);
    EXPECT_EQ(response.status(), process::TransmitResponse_Res_Status_SUCCEED);
}

TEST(test, get) {
    std::unique_ptr<RpcAdaptClient::PushDataRpcClient::PushClientHook> client;
    client.reset(new RpcAdaptClient::PushDataRpcClient::PushClientHook("127.0.0.1:20002", 100000));
    
    process::GetLogProcessStatRequest request;
    process::GetLogProcessStatResponse response;
    //std::string application = "weather";
    std::string application = "express";
    request.set_application(application);
    request.set_startime(1500447900);
    request.set_endtime(1500539400);
    request.set_endtime(1500451500);
    request.set_getfrom(process::GetLogProcessStatRequest_SourceFrom_Rocksdb);

    bool res = client->GetLogProcessStat(&request, &response, 0);
    EXPECT_EQ(res, true);
    for (int i = 0; i < response.iteminfos_size(); i++) {
        process::MapEntry *item = (process::MapEntry *)&response.iteminfos(i);
        LOG(INFO) << item->key() << ": " << item->value();
        item->release_key();
        item->release_value();
    }
}

void add_field (std::string k, std::string v, process::TransmitRequest &request) {
    process::MapEntry *fields = request.add_values();
    fields->set_key(k);
    fields->set_value(v);
}

TEST(test, fakein) {
    std::unique_ptr<RpcAdaptClient::PushDataRpcClient::PushClientHook> client;
    client.reset(new RpcAdaptClient::PushDataRpcClient::PushClientHook("10.99.196.105:8539", 3000));

    process::TransmitRequest request;
    process::TransmitResponse response;
    add_field("query", "广州天气", request);
    add_field("resno", "9", request);
    add_field("sid", "107411", request);
    add_field("application", "weather", request);
    add_field("baiduid", "", request);
    add_field("cluster", "nj02", request);
    add_field("cookieid", "BB7E8EDD12B162939FE08F82E425823F", request);
    add_field("qid", "10683170331606402479", request);
    add_field("srcid", "4514", request);
    add_field("type", "new", request);
    add_field("timestamp", "2017-07-17 21:25:00  ", request);
    
    bool res = client->Transmit(&request, &response, 0);
    EXPECT_EQ(res, true);
    for (int i = 0; i < request.values_size(); i++) {
        process::MapEntry *fields = (process::MapEntry *)&request.values(i);
        fields->release_key();
        fields->release_value();
    }

    process::GetLogProcessStatRequest request2;
    process::GetLogProcessStatResponse response2;
    request2.set_getfrom(process::GetLogProcessStatRequest_SourceFrom_QueryTrie);
    request2.set_querystr("南京天气");
    request2.set_srcid("4514");
    request2.set_startime(1501838040);
    request2.set_endtime(1502116500);

    res = client->GetLogProcessStat(&request2, &response2, 0);
    EXPECT_EQ(res, true);
    LOG(INFO) << response2.status() << ", " << response2.count();
}

TEST(test, set_chklist) {
    std::unique_ptr<RpcAdaptClient::PushDataRpcClient::PushClientHook> client;
    client.reset(new RpcAdaptClient::PushDataRpcClient::PushClientHook("127.0.0.1:20002", 3000));
    process::SetCheckListConfigRequest request;
    process::SetCheckListConfigResponse response;

    process::CheckListCond cond;

    process::ResnoConfig *resno_cfg = cond.add_resno();
    resno_cfg->set_resno("");
    resno_cfg->set_cond(process::condEqual);
    
    process::SrcidConfig *srcid_cfg = cond.add_srcids();
    srcid_cfg->set_srcid("4178");
    srcid_cfg->set_cond(process::condEqual);
    
    process::SidConfig *sid_cfg = cond.add_sid();
    sid_cfg->set_sid("107411");
    sid_cfg->set_cond(process::condEqual);
    
    request.set_allocated_cond(&cond);
    request.set_application("weather");

    bool res = client->SetCheckListConfig(&request, &response, 0);

    EXPECT_EQ(res, true);
    EXPECT_EQ(response.status(), process::TransmitResponse_Res_Status_SUCCEED);
    request.release_cond();
}

TEST(test, select) {
    std::unique_ptr<RpcAdaptClient::PushDataRpcClient::PushClientHook> client;
    client.reset(new RpcAdaptClient::PushDataRpcClient::PushClientHook("127.0.0.1:20003", 100000));
    
    process::GetLogProcessStatRequest request;
    process::GetLogProcessStatResponse response;
    //std::string application = "weather";
    std::string application = "weather";
    request.set_application(application);
    request.set_startime(1500447900);
    request.set_endtime(1500539400);
    request.set_endtime(1500451500);
    request.set_endtime(1500458700);
    request.set_getfrom(process::GetLogProcessStatRequest_SourceFrom_Rocksdb);
    process::CheckListCond cond;

    process::ResnoConfig *resno_cfg = cond.add_resno();
    resno_cfg->set_resno("");
    resno_cfg->set_cond(process::condEqual);
    
    process::SrcidConfig *srcid_cfg = cond.add_srcids();
    srcid_cfg->set_srcid("4178");
    srcid_cfg->set_cond(process::condEqual);
/*    
    for (auto i: {"114177", "102065"}) {
        process::SidConfig *sid_cfg = cond.add_sid();
        sid_cfg->set_sid(i);
        sid_cfg->set_cond(process::condIn);
    }
*/
    request.set_allocated_cond(&cond);
 
    request.add_selectfields("query");
    request.add_selectfields("cluster");
    request.add_selectfields("timestamp");

    request.set_start(0);
    request.set_end(99);
    request.set_sortfield("timestamp");
    

    bool res = client->GetLogProcessStat(&request, &response, 0);
    EXPECT_EQ(res, true);
    EXPECT_EQ(response.status(), process::TransmitResponse_Res_Status_SUCCEED);
    LOG(INFO) << response.iteminfos_size();
    for (int i = 0; i < response.iteminfos_size(); i++) {
        process::MapEntries *item = (process::MapEntries *)&response.iteminfos(i);
        for (int j = 0; j < item->item_size(); j++) {
            process::MapEntry *field = (process::MapEntry *)&item->item(j);
            LOG(INFO) << field->key() << ": " << field->value();
            field->release_key();
            field->release_value();
        }
        LOG(INFO) << "-----------------------------------------";
    }
    request.release_cond();
}

int main (int argc, char *argv[]) {
    google::SetStderrLogging(google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
/*
    google::SetLogDestination(google::GLOG_INFO, "/home/work/test_proto.log.info");
    google::SetLogDestination(google::GLOG_FATAL, "/home/work/test_proto.log.fatal");
    google::SetLogDestination(google::GLOG_ERROR, "/home/work/test_proto.log.err");
    google::SetLogDestination(google::GLOG_WARNING, "/home/work/test_proto.log.wf");
*/
    ::testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}
