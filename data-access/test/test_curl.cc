#include "gtest/gtest.h"
#include "glog/logging.h"
#include "range_iterator.h"
#include "curl.h"
#include <mutex>
#include <chrono>
#include <thread>

TEST (test, curl) {
    std::thread a = std::thread([] () {
        common::HttpClient hc;
        while (1) {
            std::string url = "www.baidu.com", resp = "";
            hc.Get(url, resp);
            LOG(INFO) << resp.length();
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    });
    
    std::thread b = std::thread([] () {
        common::HttpClient hc;
        while (1) {
            std::string url = "www.qq.com", resp = "";
            LOG(INFO) << hc.Get(url, resp);
            LOG(INFO) << resp;
            LOG(INFO) << resp.length();
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    });
    
    a.join();
    b.join();
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
