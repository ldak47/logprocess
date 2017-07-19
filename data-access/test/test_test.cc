#include "glog/logging.h"
#include "gtest/gtest.h"
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "time_trans.h"

//here is used for temp test anything at anytime.
TEST (test_test, test_test_1) {
    std::string s = "wwwttt";
    std::vector<std::string> v;
    boost::algorithm::split(v, s, boost::algorithm::is_any_of("|"));
    for (auto i: v) {
        LOG(INFO) << i;
    }

    char *file_path_getcwd;
    file_path_getcwd = (char *)malloc(80);
    getcwd(file_path_getcwd, 80);
    LOG(INFO) << file_path_getcwd;
}

TEST(test, time) {
    std::string strtime = "2017-7-29 22:41:00";
    std::string r = std::to_string(strTime2Unixtime(strtime));
    LOG(INFO) << r;
    LOG(INFO) << strTime2Unixtime(strtime);

    //LOG(INFO) << unixTime2Str(1501339260);
}

int main (int argc, char *argv[]) {
    google::SetStderrLogging(google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
