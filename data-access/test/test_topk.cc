#include "gtest/gtest.h"
#include "glog/logging.h"
#include "timer.h"
#include "heap_func.h"
#include <fstream>
#include <random>
#include <functional>
#include <iostream>
#include <boost/algorithm/string.hpp>

int cmp (uint64_t a, uint64_t b) {
    return a > b;
}
std::function<int (uint64_t, uint64_t)> compare = std::bind(cmp, std::placeholders::_1, std::placeholders::_2);

TEST (test, topk) {
#if 0
    std::ofstream ofs("test.bigdata", std::ios::out|std::ios::trunc);
    std::random_device rd;
    for (int i = 0; i < 100 * 1000 * 1000; i++) {
        ofs << rd() << std::endl;
    }
    ofs.close();
#endif


    common::Heap<uint64_t> hp(compare);
    std::vector<uint64_t> hpdata;
    std::string line;
    std::ifstream ifs("test.bigdata");
    uint32_t i = 0;
    while (!ifs.eof()) {
        getline(ifs, line);
        if (line == "") {
            continue;
        }
        
        boost::algorithm::trim_if(line, boost::algorithm::is_any_of("\n"));

        if (i < 100) {
            hpdata.push_back(stoull(line));
        } else if (i == 100) {
            LOG(INFO) << "init siez " << hpdata.size();
            hp.Init(hpdata);
            hp.Setop(stoull(line));
        } else if (i > 100) {
            hp.Setop(stoull(line));
        }
        i++;
    }

    hp.Sort();
    hp.Show();
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
