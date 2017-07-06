#include "gtest/gtest.h"
#include "glog/logging.h"


TEST(test_virtual, test) {
    class father {
    public:
        int a_;
        int GetA() {return a_;}
        virtual void SetA(int a) {
            a_ = a;
            LOG(INFO) << "father modify a to " << a_;
        }
        //add keyword final to avoid non-anticipated inherit in compling but not errored in running
        //virtual void SetA(int a) final {a_ = a;}
    public:
        father(int a):a_(a) {}
    };

    class Son: public father {
        int b_;
    public:
        Son(int a, int b):father(a), b_(b) {}
        //add keyword override to avoid the error in compling but not errored in running.(eg. 'int SetA(double a){...} override' woule be errored by compiler)
        virtual void SetA(int a) override {
            a_ = a;
            LOG(INFO) << "son modify a to " << a_;
        }
        //virtual void SetA(double a) override {a_ = a;}
    };

    std::unique_ptr<Son> a;
    a.reset(new Son(1, 2));
    if (a.get()) {
        a->SetA(10);
        EXPECT_EQ(10, a->GetA());
    }
    a.reset(nullptr);
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
