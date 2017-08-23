#include "gtest/gtest.h"
#include "glog/logging.h"
#include "trie.h"
#include <map>
#include <unordered_map>

unsigned int FNV1_32_HASH (const char *key, int len) {
    int p = 16777619;
    int hash = (int)2166136261L;
    for (int i = 0; i < len; i++) {
        hash = (hash ^ key[i]) * p;
    }

    hash += hash << 13;
    hash ^= hash >> 7;
    hash += hash << 3;
    hash ^= hash >> 17;
    hash += hash << 5;
    if (hash < 0) {
        hash = (0 - hash);
    }

    return hash;
}

TEST(test, trie) {
    common::Trie tr;

    tr.Insert(FNV1_32_HASH("北京天气", sizeof("北京天气")));
    tr.Insert(FNV1_32_HASH("南京天气", sizeof("南京天气")));
    tr.Insert(FNV1_32_HASH("天京天气", sizeof("天京天气")));
    tr.Insert(FNV1_32_HASH("西京天气", sizeof("西京天气")));
    tr.Insert(FNV1_32_HASH("华盛顿天气", sizeof("华盛顿天气")));
    tr.Insert(FNV1_32_HASH("北京天气怎么样", sizeof("北京天气怎么样")));
    tr.Insert(FNV1_32_HASH("北京天气未来15天", sizeof("北京天气未来15天")));
    tr.Insert(FNV1_32_HASH("北京天气如何", sizeof("北京天气如何")));
    tr.Insert(FNV1_32_HASH("新疆天气", sizeof("新疆天气")));
    tr.Insert(FNV1_32_HASH("南方天气", sizeof("南方天气")));
    tr.Insert(FNV1_32_HASH("上海天气", sizeof("上海天气")));
    tr.Insert(FNV1_32_HASH("广州天气", sizeof("广州天气")));
    tr.Insert(FNV1_32_HASH("北京天气", sizeof("北京天气")));
    tr.Insert(FNV1_32_HASH("北京天气", sizeof("北京天气")));
    tr.Insert(FNV1_32_HASH("北京天气", sizeof("北京天气")));
    tr.Insert(FNV1_32_HASH("墨尔本天气", sizeof("墨尔本天气")));
    tr.Insert(FNV1_32_HASH("北京天气下雨", sizeof("北京天气下雨")));
    tr.Insert(FNV1_32_HASH("天津天气", sizeof("天津天气")));
    tr.Insert(FNV1_32_HASH("北京天气多少度", sizeof("北京天气多少度")));
    tr.Insert(FNV1_32_HASH("今天多少度", sizeof("今天多少度")));
    tr.Insert(FNV1_32_HASH("天气", sizeof("天气")));
    tr.Insert(FNV1_32_HASH("天气预报", sizeof("天气预报")));
    tr.Insert(FNV1_32_HASH("天气怎么样", sizeof("天气怎么样")));
    tr.Insert(FNV1_32_HASH("还会高温吗", sizeof("还会高温吗")));
    tr.Insert(FNV1_32_HASH("上海天气", sizeof("上海天气")));
    tr.Insert(FNV1_32_HASH("长沙多少度", sizeof("长沙多少度")));
    tr.Insert(FNV1_32_HASH("南昌天气怎么样", sizeof("南昌天气怎么样")));

    uint32_t cnt(0);
    LOG(INFO) << tr.Search(FNV1_32_HASH("北京天气", sizeof("北京天气")), cnt);
    LOG(INFO) << cnt;
    cnt = 0;
    LOG(INFO) << tr.Search(FNV1_32_HASH("北京天气多少度", sizeof("北京天气多少度")), cnt);
    LOG(INFO) << cnt;
    cnt = 0;
    LOG(INFO) << tr.Search(FNV1_32_HASH("北京天气还高温吗", sizeof("北京天气还高温吗")), cnt);
    LOG(INFO) << cnt;
    cnt = 0;

    std::unordered_map<uint32_t, uint32_t> fq;
    tr.Dfs(fq);
    for (std::unordered_map<uint32_t, uint32_t>::iterator it = fq.begin(); it != fq.end(); it++) {
        LOG(INFO) << it->first << ": " << it->second;
    }
}


int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
