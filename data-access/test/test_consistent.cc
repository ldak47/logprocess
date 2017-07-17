#include "gtest/gtest.h"
#include "glog/logging.h"
#include "consistent_hash.h"
#include <fstream>


TEST(test, test1) {
    std::vector<std::string> servers = {"127.0.0.1:20000", "127.0.0.1:20001", "127.0.0.1:20002", "127.0.0.1:20003", "127.0.0.1:20004", 
                                        "127.0.0.1:20005", "127.0.0.1:20006", "127.0.0.1:20007", "127.0.0.1:20008", "127.0.0.1:20009", 
                                        "127.0.0.1:20010", "127.0.0.1:20011", "127.0.0.1:20012", "127.0.0.1:20013", "127.0.0.1:20014", 
                                        "127.0.0.1:20015", "127.0.0.1:20016", "127.0.0.1:20017", "127.0.0.1:20018", "127.0.0.1:20019"};
    int virtual_num_per_server = 100;
    common::ConsistentHash ch(servers, servers.size(), virtual_num_per_server);
    
    std::map<std::string, int> data_map;
    std::ifstream in("/home/work/data-access/src/process/query.txt");
    while (!in.eof()) {
        std::string query;
        getline(in, query);
        if (query == "") {
            continue;
        }
        std::string server = ch.GetServerNode(query);
        if (server == "") {
            LOG(ERROR) << query;
        }

        if (data_map.find(server) == data_map.end()) {
            data_map[server] = 1;
        } else {
            data_map[server] += 1;
        }
    }

    std::map<std::string, int>::iterator it = data_map.begin();
    for (; it != data_map.end(); it++) {
        LOG(INFO) << it->first << ": " << it->second;
    }
    in.close();
}

TEST(test, simple) {
    std::vector<std::string> servers = {"127.0.0.1:20000", "127.0.0.1:20001", "127.0.0.1:20002", "127.0.0.1:20003", "127.0.0.1:20004", 
                                        "127.0.0.1:20005", "127.0.0.1:20006", "127.0.0.1:20007", "127.0.0.1:20008", "127.0.0.1:20009", 
                                        "127.0.0.1:20010", "127.0.0.1:20011", "127.0.0.1:20012", "127.0.0.1:20013", "127.0.0.1:20014", 
                                        "127.0.0.1:20015", "127.0.0.1:20016", "127.0.0.1:20017", "127.0.0.1:20018", "127.0.0.1:20019"};
    int virtual_num_per_server = 100;
    common::ConsistentHash ch(servers, servers.size(), virtual_num_per_server);
    LOG(INFO) << ch.murMurHash2("贵阳黄果树瀑布天气", strlen("贵阳黄果树瀑布天气"));
    LOG(INFO) << ch.GetServerNode("贵阳黄果树瀑布天气");
    LOG(INFO) << ch.murMurHash2("贵阳", strlen("贵阳"));
    LOG(INFO) << ch.GetServerNode("贵阳");
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
