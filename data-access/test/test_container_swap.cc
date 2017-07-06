#include "gtest/gtest.h"
#include "glog/logging.h"
#include <string>
#include <vector>

/*非string容器, 迭代器也会指向新容器, 即没有失效. 而string容器将失效, 指向旧容器的相对位置*/
TEST(test_container_swap, test_vector) {
    std::vector<int> a = {1, 2, 3, 4, 5}, b = {0, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    std::vector<int>::iterator it1 = a.begin() + 3, it2 = b.begin() + 4;
    LOG(INFO) << "test non-string";
    LOG(INFO) << "a.size(): " << a.size();
    LOG(INFO) << "b.size(): " << b.size();
    LOG(INFO) << "*it1: " << *it1;
    LOG(INFO) << "*it2: " << *it2;

    std::swap(a, b);
    LOG(INFO) << "a.size(): " << a.size();
    LOG(INFO) << "b.size(): " << b.size();
    LOG(INFO) << "*it1: " << *it1;
    LOG(INFO) << "*it2: " << *it2;
}

TEST(test_container_swap, test_string) {
    std::string a = "12345", b = "0987654321";
    std::string::iterator it1 = a.begin() + 3, it2 = b.begin() + 4;
    LOG(INFO) << "test string";
    LOG(INFO) << "a.size(): " << a.size();
    LOG(INFO) << "b.size(): " << b.size();
    LOG(INFO) << "*it1: " << *it1;
    LOG(INFO) << "*it2: " << *it2;

    std::swap(a, b);
    LOG(INFO) << "a.size(): " << a.size();
    LOG(INFO) << "b.size(): " << b.size();
    LOG(INFO) << "*it1: " << *it1;
    LOG(INFO) << "*it2: " << *it2;
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

