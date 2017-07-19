#include "process_storage.h"

namespace dataprocess {

StorageImpl::StorageImpl (const std::string path, std::vector<std::string> cfs):err_(false) {
    rdb_.reset(new common::Rocksdb(path.c_str(), cfs));
    if (rdb_.get()) {
        err_ = true;
    } else {
        path_ = path;
        cfs_ = cfs;
    }
}

StorageImpl::~StorageImpl () {
    cfs_.clear();
}

bool StorageImpl::Write (const std::string key, const std::string value, std::string cf) {
    if (cf != "" || !cfs_.size()) {
        return rdb_->Put(key, value, cf);
    }
    
    for (auto i: cfs_) {
        bool res = rdb_->Put(key, value, i);
        if (!res) {
            rdb_->Delete(key);
            return false;
        }
    }

    return true;
}

bool StorageImpl::SingleGet (const std::string key, std::string &value, std::string cf) {
    return rdb_->Get(key, value, cf);
}

bool StorageImpl::RangeGet (const std::string &start, const std::string &end, std::vector<std::string> &value, std::string cf) {
    return rdb_->RangeScan(start, end, value, cf);
}

bool StorageImpl::MultiGet (std::vector<std::string> &keys, std::vector<std::string> *values, const std::vector<std::string> cfs) {
    return rdb_->MultiGet(keys, values, cfs);
}

bool StorageImpl::Delete (const std::string &key) {
    return rdb_->Delete(key);
}

bool StorageImpl::RangeDelete (const std::string &start, const std::string &end, std::string cf) {
    return rdb_->DeleteRange(start, end, cf);
}


};
