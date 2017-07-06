#ifndef DATAACCESS_COMMON_CONFIG_H
#define DATAACCESS_COMMON_CONFIG_H
#include "libconfig.h++"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include <mutex>
#include <string>

DEFINE_string(base_dir, "/home/work/data-access/", "project dir");
DEFINE_string(conf_dir, "conf/", "config-file dir");
DEFINE_string(conffile, "data-access.cfg", "data-access conf file");

DEFINE_string(FileReadIOError, "I/O error while reading file.", "catch wrong when reading cfgfile");
DEFINE_string(ParseError, "Parse error.", "catch wrong when parsing cfgfile");
DEFINE_string(FileWriteIOError, "I/O error while writing file.", "catch wrong when writing cfgfile");

namespace common {

class Config {
    std::string cfgFile_;
    std::mutex mtx_;
    libconfig::Config cfg_;
    libconfig::Setting &root_;
    std::string failure_;
public:
    Config(std::string cfgfile = FLAGS_base_dir + FLAGS_conf_dir + FLAGS_conffile);
    ~Config(){}

    libconfig::Setting &GetEntry(libconfig::Setting &entry, std::string field);
    
    void AddGroup(libconfig::Setting &entry, std::string field);
    void AddInt(libconfig::Setting &entry, std::string field, int value);
    void AddFloat(libconfig::Setting &entry, std::string field, float value);
    void AddString(libconfig::Setting &entry, std::string field, std::string);
};




};
#endif
