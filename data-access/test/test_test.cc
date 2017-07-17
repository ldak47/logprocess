#include "glog/logging.h"
#include "gtest/gtest.h"
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

//here is used for temp test anything at anytime.
TEST (test_test, test_test_1) {
    std::string s = "wwwttt";
    std::vector<std::string> v;
    boost::algorithm::split(v, s, boost::algorithm::is_any_of("|"));
    for (auto i: v) {
        LOG(INFO) << i;
    }
}

int main (int argc, char *argv[]) {
    google::SetStderrLogging(google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
