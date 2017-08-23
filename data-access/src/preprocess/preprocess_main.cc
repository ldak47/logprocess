#include <string>
#include "data_access_mgr.h"
#include "data_access_adapt.h"
#include "data_preprocess.h"
#include "data_preprocess_action.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include "health.h"
#include "cJSON/cJSON.h"
#include "curl.h"
#include "redis.h"

using namespace dataaccess;

Health health;

DEFINE_string(base_dir, "/home/work/data-access/", "project dir");
DEFINE_string(conf_dir, "/conf", "project conf dir");
DEFINE_string(conffile, "/data-access.cfg", "project conf file");

void SubmitHealthInfo (const std::string &rpcserver_addr, const std::unique_ptr<common::RedisCli> &redisctx) {
    std::string jsonstream;
    try {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "node", rpcserver_addr.c_str());
        cJSON_AddStringToObject(root, "meminfo", (std::to_string(std::get<0>(health))).c_str());
        cJSON_AddStringToObject(root, "threadinfo", (std::to_string(std::get<1>(health))).c_str());
        cJSON_AddStringToObject(root, "cpuinfo", (std::to_string(std::get<2>(health))).c_str());
        char *tmp = cJSON_Print(root);
        jsonstream = tmp;
        cJSON_Delete(root);
        redisctx->Set(rpcserver_addr, jsonstream, -1);
    } catch (std::exception &err) {
        LOG(ERROR) << "Make HealthInfo fail with " << err.what();
        return;
    }
}

bool CheckExist (const std::string path) {
    return access(path.c_str(), F_OK) != -1;
}

std::vector<std::string> GetSupportTypes (const libconfig::Setting &root) {
    std::string alltypes;
    root.lookupValue("supportcfg", alltypes);
    std::vector<std::string> types;
    boost::algorithm::split(types, alltypes, boost::is_any_of("|"));
    return types;
}

bool LoadConfig (const std::string &project_path, libconfig::Config &cfg, std::vector<std::string> &support_types, std::map<std::string, std::vector<std::string>> &types, std::string &platformaddr) {
    std::string path = project_path + FLAGS_conf_dir + FLAGS_conffile;
    if (!CheckExist(path)) {
        LOG(ERROR) << path << " not exists";
        return false;
    }
    try {
        cfg.readFile(path.c_str());
        libconfig::Setting &root = cfg.getRoot();
        std::string allservers;

        root.lookupValue("plathttpaddr", platformaddr);
        support_types = GetSupportTypes(root);
        const libconfig::Setting &srcid_cfg = root["srcid_cfg"];
        int typecnt = srcid_cfg.getLength();
        for (auto i: common::Range(0, typecnt)) {
            for (auto j: support_types) {
                std::string srcids;
                srcid_cfg.lookupValue(j.c_str(), srcids);
                std::vector<std::string> srcid_vec;
                boost::algorithm::split(srcid_vec, srcids, boost::is_any_of("|"));
                types.insert(make_pair(j, srcid_vec));
            }
        }
        
    } catch (std::exception &err) {
        LOG(ERROR) << "parse " << path << " fail with err " << err.what();
        return false;
    }

    return true;
}

//read by dataaccess, only could be modified by access-rpc
bool swithc_old = true;
bool swithc_new = true;

void glogsetup (const std::string &project_path, const int &accessid) {
    google::InitGoogleLogging("");
    FLAGS_stop_logging_if_full_disk = true;
    FLAGS_logbufsecs = 0;
    google::SetStderrLogging(google::GLOG_FATAL);
    
    std::string log_path = project_path + "/log";
    std::string fatal_log_path = log_path + "/log_fatal.accesser." + std::to_string(accessid) + ".";
    std::string error_log_path = log_path + "/log_error.accesser." + std::to_string(accessid) + ".";
    std::string warning_log_path = log_path + "/log_warning.accesser." + std::to_string(accessid) + ".";
    std::string info_log_path = log_path + "/log_info.accesser." + std::to_string(accessid) + ".";
    google::SetLogDestination(google::GLOG_FATAL, fatal_log_path.c_str());
    google::SetLogDestination(google::GLOG_ERROR, error_log_path.c_str());
    google::SetLogDestination(google::GLOG_WARNING, warning_log_path.c_str());
    google::SetLogDestination(google::GLOG_INFO, info_log_path.c_str());
}

void ParseArg (int argc, char *argv[], std::vector<std::string> &processes_addr, std::string &project_path, int &accessid, std::string &redisaddr, int &redis_port, std::string &serverport, std::string &pullserver, std::string &rpcaddr) {
    project_path = argv[1];
    serverport = argv[2];
    pullserver = argv[3];
    redisaddr = argv[4];
    std::vector<std::string> vec;
    boost::algorithm::split(vec, redisaddr, boost::is_any_of(":"));
    redisaddr = vec[0];
    redis_port = std::stoi(vec[1]);
    accessid = std::stoi(argv[5]);
    rpcaddr = argv[6];
    rpcaddr += ":";
    rpcaddr += argv[2];
    for (int i = 7; i < argc; i++) {
        processes_addr.push_back(argv[i]);
    }
}

int main (int argc, char *argv[]) {
    std::vector<std::string> processes_addr;
    std::string project_path;
    int accessid;
    std::string redisaddr;
    std::string pullserver;
    std::string platformaddr;
    int redis_port;
    std::string serverport;
    std::string rpcaddr; 
    ParseArg(argc, argv, processes_addr, project_path, accessid, redisaddr, redis_port, serverport, pullserver, rpcaddr);
    assert(project_path != "");
    assert(redisaddr != "");
    assert(serverport != "");
    assert(accessid >= 0);
    assert(processes_addr.size() > 0);
    glogsetup(project_path, accessid);
    
    libconfig::Config cfg;
    std::map<std::string, std::vector<std::string>> types;
    std::vector<std::string> support_types;
    if (!LoadConfig(project_path, cfg, support_types, types, platformaddr)) {
        exit(-1);
    }
    libconfig::Setting &root = cfg.getRoot();

    AccessFilter datafilter(root["srcid_cfg"], support_types);
    PvStater pvstater(types);
    DataFieldRuler datafieldruler;
    datafieldruler.LoadConfig(root["preprocess_action"], support_types, root["srcid_cfg"]);
    AccessAdapt accessadapter(root["access"], pvstater, datafieldruler, datafilter, swithc_old, swithc_new, serverport, rpcaddr);
    if (accessadapter.GetError()) {
        exit(-1);
    }

    DataPreProcess datapreprocesser(root["preprocess"], datafieldruler, redisaddr, redis_port, processes_addr);
    DataAccesserMgr dataaccessmgr(root["access"], root["access_new"], support_types, root["srcid_cfg"], datafilter, swithc_old, swithc_new, accessid, pullserver, pvstater, datafieldruler, platformaddr);
    
    std::string rpcserver_addr = accessadapter.GetRpcServerAddr();
    std::unique_ptr<common::RedisCli> redisctx;
    redisctx.reset(new common::RedisCli(redisaddr, redis_port));
    assert(redisctx.get());
    while(1) {
        std::chrono::seconds duration(10);
        std::this_thread::sleep_for(duration);
        health = UpdateHeadlth(getpid());
        SubmitHealthInfo(rpcserver_addr, redisctx);
    }
    return 0;
}
