#include "gtest/gtest.h"
#include "glog/logging.h"
#include <string>
#include <thread>
#include "rocksdb.h"
#include "range_iterator.h"
#include <random>

using namespace rocksdb;

class ProcessMerge : public rocksdb::MergeOperator {
public:
    virtual bool FullMergeV2 (const MergeOperationInput &merge_in,
                              MergeOperationOutput *merge_out) const override {
        merge_out->new_value.clear();
        if (merge_in.existing_value != nullptr) {
            merge_out->new_value.assign(merge_in.existing_value->data(), merge_in.existing_value->size());
        }

        for (const rocksdb::Slice& m : merge_in.operand_list) {
            merge_out->new_value.append("|");
            merge_out->new_value.append(m.data(), m.size());
        }

        return true;
    }

    const char* Name() const override { return "ProcessMerge"; }
};

TEST(test, merge) {
    //DB* db;
    //rocksdb::Options options;
    //Status s = DB::Open(options, "./rocksdb_test_cf", &db);

    common::Rocksdb rdb("./rocksdb_test_cf");

    //options.create_if_missing = true;
    //options.merge_operator.reset(new ProcessMerge);
    //s = rocksdb::DB::Open(options, "/tmp/rocksmergetest", &db);
    //assert(s.ok());

    std::string value = "aaaaa", k = "1", v;
    rdb.Put(k, value);
    rdb.Get(k, v);
    std::cout << "0: " << v << std::endl;

    value = "bbbbb";
    rdb.Put(k, value);
    rdb.Get(k, v);
    std::cout << "1: " << v << std::endl;
}

TEST(test_rocksdb, sim) { 
  Options options;
  options.create_if_missing = true;
  DB* db;

  Status s = DB::Open(options, "./rocksdb_test_cf", &db);
  assert(s.ok());

  // create column family
  ColumnFamilyHandle *cf1, *cf2;
  s = db->CreateColumnFamily(ColumnFamilyOptions(), "new_cf1", &cf1);
  assert(s.ok());
  s = db->CreateColumnFamily(ColumnFamilyOptions(), "new_cf2", &cf2);
  assert(s.ok());

  // close DB
  delete cf1;
  delete cf2;
  delete db;

  // open DB with two column families
  std::vector<ColumnFamilyDescriptor> column_families;
  // have to open default column family
  column_families.push_back(ColumnFamilyDescriptor(kDefaultColumnFamilyName, ColumnFamilyOptions()));
  // open the new one, too
  column_families.push_back(ColumnFamilyDescriptor("new_cf1", ColumnFamilyOptions()));
  column_families.push_back(ColumnFamilyDescriptor("new_cf2", ColumnFamilyOptions()));
  std::vector<ColumnFamilyHandle*> handles;
  s = DB::Open(DBOptions(), "./rocksdb_test_cf", column_families, &handles, &db);
  assert(s.ok());


  // put and get from non-default column family
  s = db->Put(WriteOptions(), handles[1], Slice("key1"), Slice("value1111111"));
  assert(s.ok());
  s = db->Put(WriteOptions(), handles[1], Slice("key2"), Slice("value1111112"));
  assert(s.ok());
  s = db->Put(WriteOptions(), handles[1], Slice("key3"), Slice("value1111113"));
  assert(s.ok());
  s = db->Put(WriteOptions(), handles[1], Slice("key5"), Slice("value1111114"));
  assert(s.ok());
  s = db->Put(WriteOptions(), handles[2], Slice("key5"), Slice("value1111115"));
  assert(s.ok());
  
    rocksdb::Iterator *it = db->NewIterator(ReadOptions(), handles[1]);
    it->Seek(Slice("key1"));
    printf("valid: %d\n", it->Valid());

    for (it->Seek(Slice("key1")); it->Valid() && it->key().ToString() <= "key5"; it->Next()) {
        printf("key %s, value %s\n", it->key().ToString().c_str(), it->value().ToString().c_str());
    }
 
    s = db->DropColumnFamily(handles[1]);
  assert(s.ok());
    s = db->DropColumnFamily(handles[2]);
  assert(s.ok());
    
   for (auto handle: handles) {
    delete handle;
   } 
   delete db;
}

TEST(test_rocksdb, construct_destruct) {
    common::Rocksdb rdb("./rocksdb_test_rocksdb/", {"cf1", "cf2"});
}

TEST(test_rocksdb, set_get_without_cf) {
    std::string key = "123", value = "456", val = "";
    std::unique_ptr<common::Rocksdb> rdb;
    rdb.reset(new common::Rocksdb("/home/work/data-access/test/rocksdb_test_rocksdb/", {"cf1", "cf2", "cf3", "cf4", "cf5"}));
    LOG(INFO) << "err_flag_: " << rdb->err_flag_;
/*
    LOG(INFO) << "put: " << rdb.Put(key, value);
    LOG(INFO) << "err_flag_: " << rdb.err_flag_;
    LOG(INFO) << "put: " << rdb.Get(key, val) << ", val: " << val;
    LOG(INFO) << "err_flag_: " << rdb.err_flag_;

    key = "456";
    value = "123";
    LOG(INFO) << "put: " << rdb.Put(key, value);
    LOG(INFO) << "err_flag_: " << rdb.err_flag_;
    LOG(INFO) << "put: " << rdb.Get(key, val) << ", val: " << val;
    LOG(INFO) << "err_flag_: " << rdb.err_flag_;
*/
return;
    
        rdb->Put("2", "b", "cf1");
        rdb->Put("3", "c", "cf1");
        rdb->Put("4", "d", "cf1");
        rdb->Put("5", "e", "cf1");
        rdb->Put("1", "a", "cf2");
        rdb->Put("2", "f", "cf2");
        rdb->Put("1", "a", "cf3");
        rdb->Put("2", "g", "cf3");
        rdb->Put("3", "h", "cf3");
        rdb->Put("1", "j", "cf4");
        rdb->Put("4", "i", "cf4");
        rdb->Put("1", "aa", "cf5");
        rdb->Put("2", "ab", "cf5");
        rdb->Put("3", "ac", "cf5");
        rdb->Put("4", "ad", "cf5");
        rdb->Put("5", "ae", "cf5");
        rdb->Put("6", "fe");
        rdb->Put("7", "af");
        rdb->Put("8", "ag");
        rdb->Put("9", "ah");
        rdb->Put("10", "ak");
    std::vector<std::string> vec1, vec2, vec3;
    LOG(INFO) << rdb->RangeScan("1", "5", vec1, "cf5");
    for (auto v: vec1) {
        LOG(INFO) << v;
    }
    LOG(INFO) << rdb->RangeScan("6", "10", vec2);
    for (auto v: vec2) {
        LOG(INFO) << v;
    }
    LOG(INFO) << rdb->RangeScan("5", "10", vec3);
    for (auto v: vec3) {
        LOG(INFO) << v;
    }

    std::vector<std::string> cf({"cf5", "cf5", "cf5"});
    std::vector<std::string> keys({"1", "2", "3"}), values, vecs;
    LOG(INFO) << rdb->MultiGet(keys, &values, cf);
    LOG(INFO) << rdb->err_flag_;
    LOG(INFO) << values.size();
    for (auto v: values) {
        LOG(INFO) << v;
    }
    keys = {"6", "8", "9"};
    LOG(INFO) << rdb->MultiGet(keys, &vecs);
    LOG(INFO) << vecs.size();
    for (auto v: vecs) {
        LOG(INFO) << v;
    }

    vecs.clear();
    LOG(INFO) << rdb->DeleteRange("7", "10");
    LOG(INFO) << rdb->err_flag_;
    LOG(INFO) << rdb->RangeScan("7", "9", vecs);
    LOG(INFO) << vecs.size();
    for (auto v: vecs) {
        LOG(INFO) << v;
    }

    vecs.clear();
    LOG(INFO) << rdb->DeleteRange("1", "4", "cf1");
    LOG(INFO) << rdb->err_flag_;
    LOG(INFO) << rdb->RangeScan("1", "3", vecs, "cf1");
    for (auto v: vecs) {
        LOG(INFO) << v;
    }
}

TEST(test_rocksdb, mset_get_without_cf) {
    std::vector<std::thread> producers(20);
    std::vector<std::thread> consumers(2);
    common::Rocksdb rdb("/home/work/data-access/test/rocksdb_test_rocksdb/", {"cf1", "cf2"});
    std::string dynamic_cf = "dy_cf";
    
    for (auto i: common::Range(0, 20)) {
        int start = i * 100000, len = 100000;
        producers[i] = std::thread([&rdb, i, len, &dynamic_cf] () {
            int l = len;
            rdb.CreateColumnFamily(dynamic_cf);

            while (l) {
                std::string cf = "cf1";
                /*
                if (l % 2 == 0) {
                    if (l % 3 == 0) {
                        cf = "cf2";
                    } else {
                        cf = dynamic_cf;
                    }
                }
                */
                rdb.Put(std::to_string(i + l), std::to_string(i + l - 1), cf);
                --l;
            }
        });
    }
    for (auto i: common::Range(0, producers.size())) {
        producers[i].join();
    }

    for (auto i: common::Range(0, 2)) {
        int start = i * 100000, len = 100000;
        consumers[i] = std::thread([&rdb, i, len, dynamic_cf] () {
            int l = len;
            rdb.CreateColumnFamily(dynamic_cf);

            while (l) {
                std::string cf = "cf1";
                /*
                if (l % 2 == 0) {
                    if (l % 3 == 0) {
                        cf = "cf2";
                    } else {
                        cf = dynamic_cf;
                    }
                }
                */
                std::string val;
                rdb.Get(std::to_string(i + l), val, cf);
                LOG(INFO) << " key " << i + l << "   value " << val;
                --l;
            }
        });
    }

    for (auto i: common::Range(0, consumers.size())) {
        consumers[i].join();
    }

}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
