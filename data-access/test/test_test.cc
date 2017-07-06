#include "glog/logging.h"
#include "gtest/gtest.h"
#include <string>

//here is used for temp test anything at anytime.
TEST (test_test, test_test_1) {
}

int main (int argc, char *argv[]) {
    google::SetStderrLogging(google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
