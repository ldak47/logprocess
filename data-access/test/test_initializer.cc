#include "gtest/gtest.h"
#include "glog/logging.h"
#include <string>

struct Testclass {
public:
    Testclass(std::string a, int b, bool c):a_(a), b_(b), c_(c) {}
    std::string GetA(){return a_;}
    int GetB(){return b_;}
    bool GetC(){return c_;}
private:
    std::string a_;
    int b_;
    bool c_;
};

Testclass TestFunc () {
    return {"test2", 1, true};
}

TEST(test_initializer, test1) {
    std::vector<int> a = {1, 2, 3, 4, 5};
    for (auto& i: a) {
        LOG(INFO) << "a: " << i;
        i += 10;
    }
    LOG(INFO) << "direct init by std::initializer, as dynamic language, ";
    Testclass t1{"test1", 10, false};
    EXPECT_EQ("test1", t1.GetA());
    EXPECT_EQ(10, t1.GetB());
    EXPECT_EQ(false, t1.GetC());
    LOG(INFO) << t1.GetA();
    LOG(INFO) << t1.GetB();
    LOG(INFO) << t1.GetC();
    
    LOG(INFO) << "direct copy object from TestFunc, which direct implicitly construct object, ";
    t1 = TestFunc();
    EXPECT_EQ("test2", t1.GetA());
    EXPECT_EQ(1, t1.GetB());
    EXPECT_EQ(true, t1.GetC());
    LOG(INFO) << t1.GetA();
    LOG(INFO) << t1.GetB();
    LOG(INFO) << t1.GetC();
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
