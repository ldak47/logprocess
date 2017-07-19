#ifndef DATAACCESS_COMMON_ROCKSDB_H
#define DATAACCESS_COMMON_ROCKSDB_H
#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "rocksdb/merge_operator.h"
#include "rocksdb/compaction_filter.h"
#include <vector>
#include <mutex>
#include "glog/logging.h"

namespace common {

struct WriteBatcher {
    rocksdb::WriteBatch batch;
    void add_action(std::string action, std::vector<std::string> args, std::string &err, rocksdb::ColumnFamilyHandle *cf = nullptr){
        err = "";
        if (action == "clear") {
            batch.Clear();
            return;
        }
        if (action == "delete") {
            std::string key;
            try {
                std::vector<std::string>::iterator it = args.begin();
                key = *it;
            } catch (std::exception &e) {
                err = e.what();
                return;
            }
            batch.Delete(cf, rocksdb::Slice(key.c_str()));
            return;
        }
        if (action == "put") {
            std::string key, value;
            try {
                std::vector<std::string>::iterator itk = args.begin(), itv = itk + 1;
                key = *itk;
                value = *itv;
            } catch (std::exception &e) {
                err = e.what();
                return;
            }
            batch.Put(cf, rocksdb::Slice(key.c_str()), rocksdb::Slice(value.c_str()));
            return;
        }
        if (action == "deleterange") {
            std::string key_start, key_end;
            try {
                std::vector<std::string>::iterator itks = args.begin(), itke = itks + 1;
                key_start = *itks;
                key_end = *itke;
            } catch (std::exception &e) {
                err = e.what();
                return;
            }
            batch.DeleteRange(cf, rocksdb::Slice(key_start.c_str()), rocksdb::Slice(key_end.c_str()));
            return;
        }
        if (action == "merge") {
            std::string key, value;
            try {
                std::vector<std::string>::iterator itk = args.begin(), itv = itk + 1;
                key = *itk;
                value = *itv;
            } catch (std::exception &e) {
                err = e.what();
                return;
            }
            batch.Merge(cf, rocksdb::Slice(key.c_str()), rocksdb::Slice(value.c_str()));
        }
    }
    bool do_action(rocksdb::DB *db) {
        rocksdb::Status s = db->Write(rocksdb::WriteOptions(), &batch);
        return s.ok() ? true : false;
    }
};

class ProcessMerge : public rocksdb::MergeOperator {
public:
    virtual bool FullMergeV2 (const MergeOperationInput &merge_in,
                              MergeOperationOutput *merge_out) const override {
        merge_out->new_value.clear();
        if (merge_in.existing_value != nullptr) {
            merge_out->new_value.assign(merge_in.existing_value->data(), merge_in.existing_value->size());
        }

        for (const rocksdb::Slice& m : merge_in.operand_list) {
            if (merge_out->new_value != "") {
                merge_out->new_value.append("|");
            }
            merge_out->new_value.append(m.data(), m.size());
        }

        return true;
    }

    const char* Name() const override {
        return "ProcessMerge";
    }
};

class Rocksdb {
public:
    rocksdb::DB *db_;
    rocksdb::Options options_;
    void default_setup(const char *dir, std::vector<std::string> columnfamily);

    std::vector<rocksdb::ColumnFamilyHandle *> cfs_;
    std::map<std::string, rocksdb::ColumnFamilyHandle *> cfmap_;
    
    int err_flag_;
    Rocksdb(const char *dir, std::vector<std::string> columnfamily = {});
    ~Rocksdb();
    
    bool Put(const std::string key, const std::string value, std::string cf = "");
    bool Get(const std::string key, std::string &value, std::string cf = "");
    bool RangeScan(const std::string key_start, const std::string key_end, std::vector<std::string> &value, std::string cf = "");
    bool MultiGet(std::vector<std::string> keys, std::vector<std::string> *values, const std::vector<std::string> cfs = {});
    bool Delete(const std::string key);
    bool DeleteRange(const std::string key_start, const std::string key_end, std::string cf = "");

    WriteBatcher *BatchInit();
    void BacthAddAction(WriteBatcher *handle, std::string action, std::vector<std::string> args, std::string &err, rocksdb::ColumnFamilyHandle *cf = nullptr);
    bool BatchDoAction(WriteBatcher *handle);

    bool CreateColumnFamily(const std::string cf);
    bool DropColumnFamily(const std::string cf);
    rocksdb::ColumnFamilyHandle *GetColumnFamily(const std::string cf);
};









};
#endif
