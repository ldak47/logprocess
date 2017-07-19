#include "rocksdb.h"
#include "range_iterator.h"

namespace common {

void Rocksdb::default_setup (const char *dir, std::vector<std::string> columnfamily) {
    const std::string path = dir;
    
    //create the DB if it's not already present
    options_.create_if_missing = true;
    options_.merge_operator.reset(new ProcessMerge);
    rocksdb::Status s = rocksdb::DB::Open(options_, path, &db_);
    if (!s.ok()) {
        err_flag_ = (int)s.code();
        return;
    }
    for (auto i: columnfamily) {
        bool s = CreateColumnFamily(i);
        if (!s) {
            return;
        }
    }
    delete db_;
    db_ = nullptr;

    //have to open default column family
    std::vector<rocksdb::ColumnFamilyDescriptor> column_families;
    column_families.push_back(rocksdb::ColumnFamilyDescriptor(rocksdb::kDefaultColumnFamilyName, rocksdb::ColumnFamilyOptions()));
    for (auto i: columnfamily) {
        column_families.push_back(rocksdb::ColumnFamilyDescriptor(i.c_str(), rocksdb::ColumnFamilyOptions()));
    }
        
    //Optimize RocksDB. This is the easiest way to get RocksDB to perform well
    options_.IncreaseParallelism();
    options_.OptimizeLevelStyleCompaction();
    if (!options_.merge_operator.get()) {
       options_.merge_operator.reset(new ProcessMerge);
    }
    
    if (columnfamily.size()) {
        s = rocksdb::DB::Open(options_, path, column_families, &cfs_, &db_);
    } else {
        s = rocksdb::DB::Open(options_, path, &db_);
    }
    if (!s.ok()) {
        err_flag_ = (int)s.code();
    }

    std::vector<std::string>::iterator it = columnfamily.begin();
    for (auto i: Range(0, columnfamily.size())) {
        cfmap_[*it] = cfs_[i + 1];
        ++it;
    }
}
    
Rocksdb::Rocksdb (const char *dir, std::vector<std::string> columnfamily):err_flag_(0), db_(nullptr) {
    default_setup(dir, columnfamily);
}

Rocksdb::~Rocksdb () {
    for (auto i: cfs_) {
        db_->DropColumnFamily(i);
        delete i;
    }
    delete db_;
}

bool Rocksdb::Put (const std::string key, const std::string value, std::string cf) {
    rocksdb::Status s;
    rocksdb::WriteOptions wopt;
    wopt.disableWAL = 0;
    if (cf == "") {
        s = db_->Merge(wopt, rocksdb::Slice(key.c_str()), rocksdb::Slice(value.c_str()));
    } else {
        std::map<std::string, rocksdb::ColumnFamilyHandle *>::iterator it = cfmap_.find(cf);
        if (cfmap_.end() == it) {
            return false;
        }
        s = db_->Merge(wopt, it->second, rocksdb::Slice(key.c_str()), rocksdb::Slice(value.c_str()));
    }
    err_flag_ = s.code();
    return (true == s.ok()) ? true : false;
}

bool Rocksdb::Get (const std::string key, std::string &value, std::string cf) {
    rocksdb::Status s;
    if (cf == "") {
        s = db_->Get(rocksdb::ReadOptions(), rocksdb::Slice(key.c_str()), &value);
    } else {
        std::map<std::string, rocksdb::ColumnFamilyHandle *>::iterator it = cfmap_.find(cf);
        if (cfmap_.end() == it) {
            return false;
        }
        s = db_->Get(rocksdb::ReadOptions(), it->second, rocksdb::Slice(key.c_str()), &value);
    }
    err_flag_ = s.code();
    return (true == s.ok()) ? true : false;
}

bool Rocksdb::RangeScan (const std::string key_start, const std::string key_end, std::vector<std::string> &values, std::string cf) {
    if (key_start != "" && key_end != "") {
        std::unique_ptr<rocksdb::Iterator> it;
        if (cf == "") {
            it.reset(db_->NewIterator(rocksdb::ReadOptions()));
            for (it->Seek(rocksdb::Slice(key_start.c_str())); it->Valid() && stoll(it->key().ToString()) <= stoll(key_end); it->Next()) {
                values.push_back(it->value().ToString());
            }
            return true;
        } else {
            if (cfmap_.find(cf) != cfmap_.end()) {
                rocksdb::ColumnFamilyHandle *handle = cfmap_.find(cf)->second;
                if (handle) {
                    it.reset(db_->NewIterator(rocksdb::ReadOptions(), handle));
                    for (it->Seek(rocksdb::Slice(key_start.c_str())); it->Valid() && stoll(it->key().ToString()) <= stoll(key_end); it->Next()) {
                        values.push_back(it->value().ToString());
                    }

                    return true;
                }
            }
        }
    }

    return false;
}

bool Rocksdb::MultiGet (std::vector<std::string> keys, std::vector<std::string> *values, const std::vector<std::string> cfs) {
    if (!keys.size() || (cfs.size() && cfs.size() != keys.size())) {
        return false;
    }
    std::vector<rocksdb::Slice> slices;
    std::vector<rocksdb::ColumnFamilyHandle *> handles;
    for (size_t i = 0; i < keys.size(); i++) {
        slices.push_back(keys[i]);
    }
    for (auto cf: cfs) {
        if (cfmap_.find(cf) != cfmap_.end()) {
            handles.push_back(cfmap_.find(cf)->second);
        } else {
            return false;
        }
    }
    
    std::vector<rocksdb::Status> ss;
    rocksdb::ReadOptions ropt;
    if (cfs.size()) {
        ss = db_->MultiGet(ropt, handles, slices, values);
        for (auto s: ss) {
            if (true != s.ok()) {
                err_flag_ = (int)s.code();
                return false;
            }
        }
        return true;
    } else {
        ss = db_->MultiGet(ropt, slices, values);
        for (auto s: ss) {
            if (true != s.ok()) {
                err_flag_ = (int)s.code();
                return false;
            }
        }
        return true;
    }
}

bool Rocksdb::Delete (const std::string key) {
    rocksdb::Status s = db_->Delete(rocksdb::WriteOptions(), rocksdb::Slice(key.c_str()));
    err_flag_ = s.code();
    return (true == s.ok()) ? true : false;
}

bool Rocksdb::DeleteRange (const std::string key_start, const std::string key_end, std::string cf) {
    if (key_start == "" || key_end == "") {
        return false;
    }
    
    if (cf == "") {
        rocksdb::Status s = db_->DeleteRange(rocksdb::WriteOptions(), cfs_[0], rocksdb::Slice(key_start), rocksdb::Slice(key_end));
        err_flag_ = s.code();
        return (true == s.ok()) ? true : false;
    } else {
        if (cfmap_.find(cf) != cfmap_.end()) {
            rocksdb::ColumnFamilyHandle *handle = cfmap_.find(cf)->second;
            rocksdb::Status s = db_->DeleteRange(rocksdb::WriteOptions(), handle, rocksdb::Slice(key_start), rocksdb::Slice(key_end));
            err_flag_ = s.code();
            return (true == s.ok()) ? true : false;
        }
    }

    return false;
}

WriteBatcher *Rocksdb::BatchInit () {
    WriteBatcher *handle = new WriteBatcher();
    return handle;
}

void Rocksdb::BacthAddAction (WriteBatcher *handle, std::string action, std::vector<std::string> args, std::string &err, rocksdb::ColumnFamilyHandle *cf) {
    handle->add_action(action, args, err, cf);
}

bool Rocksdb::BatchDoAction (WriteBatcher *handle) {
    bool s = handle->do_action(db_);
    delete handle;
    return s;
}

bool Rocksdb::CreateColumnFamily (const std::string cf) {
    if (cf == "") {
        return false;
    }
    rocksdb::ColumnFamilyHandle *newcf;
    rocksdb::Status s = db_->CreateColumnFamily(rocksdb::ColumnFamilyOptions(), cf.c_str(), &newcf);
    if (!s.ok()) {
        err_flag_ = s.code();
        return false;
    }

    delete newcf;
    return true;
}

bool Rocksdb::DropColumnFamily (const std::string cf) {
    std::map<std::string, rocksdb::ColumnFamilyHandle *>::iterator it = cfmap_.find(cf);
    if (it == cfmap_.end()) {
        return false;
    }

    rocksdb::Status s = db_->DropColumnFamily(it->second);
    if (s.ok()) {
        cfmap_.erase(it);
    }
    err_flag_ = s.code();
    return (true == s.ok()) ? true : false;
}

rocksdb::ColumnFamilyHandle *Rocksdb::GetColumnFamily (const std::string cf) {
    if (cfmap_.find(cf) != cfmap_.end()) {
        return cfmap_[cf];
    }

    return nullptr;
}









};
