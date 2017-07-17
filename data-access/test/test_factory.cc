#include "gtest/gtest.h"
#include "glog/logging.h"
#include "factory.h"
#include <vector>

/* test_factory_baseclass is factory base class */
class test_factory_baseclass {
public:
    test_factory_baseclass(){
        LOG(INFO) << "base class construct";
    }
    virtual ~test_factory_baseclass(){
        LOG(INFO) << "base class destruct";
    }

    virtual void action_1(){
        LOG(INFO) << "base class action1";
    }

    virtual void action_2(){
        LOG(INFO) << "base class action2";
    }

    virtual void action_3(){
        LOG(INFO) << "base class action3";
    }
};
REGISTER_FACTORY(test_factory_baseclass);
#define REGISTER_TEST_FACTORY(sub_class) REGISTER_CLASS(test_factory_baseclass, sub_class)

/* test_factory_subclass1 is a derived class which based on test_factory_baseclass */
class test_factory_subclass1 : public test_factory_baseclass {
public:
    test_factory_subclass1(){
        LOG(INFO) << "sub class 1 construct";
    }
    virtual ~test_factory_subclass1() {
        LOG(INFO) << "sub class 1 destruct";
    }
    
    virtual void action_1(){
        LOG(INFO) << "sub class1 action1";
    }

    virtual void action_2(){
        LOG(INFO) << "sub class1 action2";
    }

    virtual void action_3(){
        LOG(INFO) << "sub class1 action3";
    }
};

/* test_factory_subclass2 is another derived class which based on test_factory_baseclass */
class test_factory_subclass2 : public test_factory_baseclass {
public:
    test_factory_subclass2(){
        LOG(INFO) << "sub class 2 construct";
    }
    virtual ~test_factory_subclass2() {
        LOG(INFO) << "sub class 2 destruct";
    }
        
    virtual void action_1(){
        LOG(INFO) << "sub class2 action1";
    }
    
    virtual void action_2(){
        LOG(INFO) << "sub class2 action2";
    }
    
    virtual void action_3(){
        LOG(INFO) << "sub class2 action3";
    }
};

void register_product_1 () {
    REGISTER_TEST_FACTORY(test_factory_subclass1);
}

void register_product_2 () {
    REGISTER_TEST_FACTORY(test_factory_subclass2);
}

void register_factory () {
    REGISTER_TEST_FACTORY(test_factory_baseclass);
}

TEST(test_factory, test1) {
    register_product_1();
    register_product_2();
    register_factory();

    /* use reflect to create derived-class object by class-name! */
    std::unique_ptr<test_factory_subclass1> test_factory_subclass1_instance;
    test_factory_subclass1_instance.reset(reinterpret_cast<test_factory_subclass1 *>(test_factory_baseclass_Factory::get_instance("test_factory_subclass1")));
    LOG(INFO) << test_factory_subclass1_instance.get();
    test_factory_subclass1_instance->action_1();
    test_factory_subclass1_instance->action_2();
    test_factory_subclass1_instance->action_3();
    
    std::unique_ptr<test_factory_subclass2> test_factory_subclass2_instance;
    test_factory_subclass2_instance.reset(reinterpret_cast<test_factory_subclass2 *>(test_factory_baseclass_Factory::get_instance("test_factory_subclass2")));
    LOG(INFO) << test_factory_subclass2_instance.get();
    test_factory_subclass2_instance->action_1();
    test_factory_subclass2_instance->action_2();
    test_factory_subclass2_instance->action_3();

    std::unique_ptr<test_factory_baseclass> test_factory_baseclass_instance;
    test_factory_baseclass_instance.reset(reinterpret_cast<test_factory_baseclass *>(test_factory_baseclass_Factory::get_instance("test_factory_baseclass")));
    LOG(INFO) << test_factory_baseclass_instance.get();
    test_factory_baseclass_instance->action_1();
    test_factory_baseclass_instance->action_2();
    test_factory_baseclass_instance->action_3();
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
