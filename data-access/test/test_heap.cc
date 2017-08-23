#include "gtest/gtest.h"
#include "glog/logging.h"
#include "heap_func.h"
#include <vector>
#include <random>

//TEST (test, map) {
//    std::vector<std::map<std::string, std::string>> 
//}


int compare_int (int a, int b) {
    return a < b;
}

TEST (test, heap) {
    std::vector<int> raw({10, 7, 2, 12, 54, 32, 23, 100, 5, 7, 19, 3, 71, 90, 20, 29, 55});
    

    for (int i = 0; i < 30; i++) {
        std::random_device rd;
        raw.push_back(rd() % 100);
    }

    common::Heap<int> hp(raw, compare_int);
    LOG(INFO) << hp.Getop();
    hp.Sort();
    hp.Show();
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
