#include "gtest/gtest.h"
#include "glog/logging.h"
#include <string>

template<class C> auto add (C a, C b) {
    return a + b;
}

TEST(test_decltype, test1) {
    auto a = 0;
    decltype(a) b;
    b = 10;
}

TEST(test_decltype, test2) {
    auto a = 1;
    decltype(a) b = 2;
    decltype(b) c = add(a, b);
    LOG(INFO) << c;
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
