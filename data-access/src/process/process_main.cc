#include "process_adapt.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

using namespace dataprocess;

DEFINE_string(base_dir, "/home/work/data-access/", "project dir");
DEFINE_string(conf_dir, "/conf", "project conf dir");
DEFINE_string(data_dir, "/data/", "project data dir");
DEFINE_string(conffile, "/data-access.cfg", "project conf file");

bool CheckExist (const std::string path) {
    return access(path.c_str(), F_OK) != -1;
}

bool LoadConfig (const std::string &project_path, libconfig::Config &cfg, std::vector<std::string> &types, std::string &platformaddr) {
    std::string path = project_path + FLAGS_conf_dir + FLAGS_conffile;
    if (!CheckExist(path)) {
        LOG(ERROR) << path << " not exists";
        return false;
    }

    try {
        cfg.readFile(path.c_str());
        libconfig::Setting &root = cfg.getRoot();
        std::string alltypes;
        root.lookupValue("plathttpaddr", platformaddr);
        root.lookupValue("supportcfg", alltypes);
        boost::algorithm::split(types, alltypes, boost::is_any_of("|"));
    } catch (std::exception &err) {
        LOG(ERROR) << "parse " << path << " fail with err " << err.what();
        return false;
    }

    return true;
}

void glogsetup (const std::string &project_path, const std::string &accessid, const std::string &port) {
    google::InitGoogleLogging("");
    FLAGS_stop_logging_if_full_disk = true;
    FLAGS_logbufsecs = 0;
    google::SetStderrLogging(google::GLOG_FATAL);
    std::string postfix = accessid + "." + port;
    
    std::string log_path = project_path + "/log";
    std::string fatal_log_path = log_path + "/log_fatal.process." + postfix;
    std::string error_log_path = log_path + "/log_error.process." + postfix;
    std::string warning_log_path = log_path + "/log_warning.process." + postfix;
    std::string info_log_path = log_path + "/log_info.process." + postfix;
    google::SetLogDestination(google::GLOG_FATAL, fatal_log_path.c_str());
    google::SetLogDestination(google::GLOG_ERROR, error_log_path.c_str());
    google::SetLogDestination(google::GLOG_WARNING, warning_log_path.c_str());
    google::SetLogDestination(google::GLOG_INFO, info_log_path.c_str());
}

void ParseArg (int argc, char *argv[], std::string &project_path, std::string &datapath, std::string &port, std::string &accessid, std::string &rpcaddr) {
    project_path = argv[1];
    datapath = project_path + FLAGS_data_dir;
    port = argv[2];
    accessid = argv[3];
    rpcaddr = argv[4];
    rpcaddr += ":";
    rpcaddr += port;
}

int main (int argc, char *argv[]) {
    std::string project_path;
    std::string platformaddr;
    std::string datapath;
    std::string accessid;
    std::string port;
    std::string rpcaddr;
    ParseArg(argc, argv, project_path, datapath, port, accessid, rpcaddr);
    assert(project_path != "");
    assert(datapath != "");
    glogsetup(project_path, accessid, port);
    
    std::vector<std::string> types;    
    libconfig::Config cfg;
    if (!LoadConfig(project_path, cfg, types, platformaddr)) {
        exit(-1);
    }

    libconfig::Setting &root = cfg.getRoot();
    LOG(INFO) << rpcaddr;
    ProcessAdapt process_adapter(root["process"], root["stat_redis_cfg"], accessid, rpcaddr, port, datapath, types, platformaddr);
    if (process_adapter.GetErrFlag()) {
        exit(-1);
    }

    while (1) {
        //should be do healthy monitor here...
        std::chrono::seconds duration(1);
        std::this_thread::sleep_for(duration);
    }
    return 0;
}
