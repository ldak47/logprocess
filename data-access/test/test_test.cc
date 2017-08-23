#include "glog/logging.h"
#include "gtest/gtest.h"
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "time_trans.h"
#include "timer.h"

int cmp (int a, int b) {
    return a > b;
}

std::function<int (int, int)> fcmp = std::bind(cmp, std::placeholders::_1, std::placeholders::_2);

TEST (test, sort_with_stdfunction) {
    std::vector<int> data = {1, 5, 3, 7, 10, 2, 20, 13, 15, 11};
    std::sort(data.begin(), data.end(), fcmp);
    for (auto i: data) {
        LOG(INFO) << i;
    }
}

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
    std::string strtime = "2017-07-19 14:44:00";
    std::string r = std::to_string(strTime2Unixtime(strtime));
    LOG(INFO) << r;
    LOG(INFO) << strTime2Unixtime(strtime);

    //LOG(INFO) << unixTime2Str(1501339260);
}

TEST(test, time_elapse) {
    double t1, t2;
    common::TimeCalcer tc;
    int i = 0;
    while(i++ < 100000);
    t1 = tc.Calc();
    i = 0;
    while(i++ < 100000);
    t2 = tc.Calc();
    LOG(INFO) << std::fixed << t1 << " s";
    LOG(INFO) << std::fixed << t2 << " s";
}

int main (int argc, char *argv[]) {
    google::SetStderrLogging(google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
