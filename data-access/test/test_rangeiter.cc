#include "glog/logging.h"
#include "gtest/gtest.h"
#include "range_iterator.h"
#include <vector>

TEST (test_common, test_common_rangeiter) {
    for (auto i: common::Range(1, 10)) {
        LOG(INFO) << i;
    }
    LOG(INFO) << "seemed as php foreach ($v as &$i), modify reference of every element, ";
    std::vector<int> a = {1, 2, 3, 4, 5};
    for (auto& i: a) {
        LOG(INFO) << "a: " << i;
        i += 10;
    }
    LOG(INFO) << "after modify reference, ";
    for (auto& i: a) {
        LOG(INFO) << "a: " << i;
    }
}

int main (int argc, char *argv[]) {
    google::SetStderrLogging(google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
