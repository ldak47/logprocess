#ifndef DATAACCESS_PROCESS_STORAGE_H
#define DATAACCESS_PROCESS_STORAGE_H
#include "rocksdb.h"

namespace dataprocess {

class StorageImpl {
    std::unique_ptr<common::Rocksdb> rdb_;
    
    std::string path_;
    std::vector<std::string> cfs_;
    bool err_;
    
public:
    StorageImpl(const std::string path, std::vector<std::string> cfs = {});
    ~StorageImpl();
    bool GetError(){
        return err_;
    }
    
    bool Write(const std::string key, const std::string value, std::string cf = "");
    bool SingleGet(const std::string key, std::string &value, std::string cf = "");
    bool RangeGet(const std::string &start, const std::string &end, std::vector<std::string> &value, std::string cf = "");
    bool MultiGet(std::vector<std::string> &keys, std::vector<std::string> *values, const std::vector<std::string> cfs);
    bool Delete(const std::string &key);
    bool RangeDelete(const std::string &start, const std::string &end, std::string cf = "");
};







};
#endif
