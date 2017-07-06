#include "gtest/gtest.h"
#include "glog/logging.h"
#include "range_iterator.h"
#include <string>
#include <vector>

std::vector<std::string> v;

/*
 *  std::move主要能解决的问题, 类似于python的深拷贝和浅拷贝, python中的对象赋值和copy.copy都是浅拷贝, 赋值的都是对象的引用, copy.deepcopy则是深拷贝, 参照本目录下的test_copy.py
 *  c++11的std::move, 解决的问题是一个复制效率的问题: 对临时变量(如函数中的参数)的复制, 通过更改对象的所有者(move), 实现免内存搬迁或拷贝(去除深拷贝), 提高"复制"效率(其实不是复制, 仅是更改了对象的所有者)
 * */

TEST(test_cpp11, test_move1) {
    std::string a = "123";
    LOG(INFO) << "at first, std::string a is: " << a;

    /*右值"123", 它的所有者将从原先的左值(变量std::string a), 转移到新的左值(std::vector v)
     *所以, 使用std::move时一定保证, 以前的左值不再真需要了. 典型使用场合就是: (构造)函数的参数, 避免了再复制*/
    v.push_back(std::move(a));
    LOG(INFO) << "after std::move(a), now std::string a is: " << a;

}

TEST(test_cpp11, test_move2) {
    /*右值"123", 因为其所有者变为了新的左值全局变量std::vector<std::string> v, 所以它的生命周期也得到延长*/
    EXPECT_EQ(v[0], "123");
    LOG(INFO) << "after move to global_variable v, now v[0] is: " << v[0];
}

/*
 * 这是一个类构造函数的应用例子. 容器类使用move的正确姿势(将容器内成员, 按右值引用更改所有者)
 * */
TEST(test_cpp11, test_move3) {
    class test {
    public:
        std::vector<std::string> t_;
        test(std::vector<std::string> &tmp) {
            for (auto& i: tmp) {
                //not copy rvalue to t_, only add rvalue reference to t_ and update rvalue's lifecycle
                t_.push_back(std::move(i));
            }
        }
    };

    /*起初, 右值("123", "456", "789", "012", "345")都归属于左值temp*/
    std::vector<std::string> temp = {"123", "456", "789", "012", "345"};
    LOG(INFO) << "before move to object ot, t's size is: " << temp.size();
    for (auto& i: temp) {
        LOG(INFO) << " OLD LVALUE(object temp) element: " << i;
    }

    /*由类test的构造函数, 更改右值的所有者为类test的对象ot*/
    test ot(temp);
    LOG(INFO) << "after move elements of temp to object ot, now ot's size is: " << ot.t_.size();
    for (auto& i: temp) {
        LOG(INFO) << " OLD LVALUE(object temp) element: " << i;
    }
    for (auto& i: ot.t_) {
        LOG(INFO) << " NEW LVALUE(object ot) element: " << i;
    }
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
