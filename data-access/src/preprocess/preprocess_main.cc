#include "data_access_mgr.h"
#include "data_preprocess.h"
#include "data_preprocess_action.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

using namespace dataaccess;

DEFINE_string(base_dir, "/home/work/data-access/", "project dir");
DEFINE_string(conf_dir, "conf/", "project conf dir");
DEFINE_string(conffile, "data-access.cfg", "project conf file");

bool CheckExist (const std::string path) {
    return access(path.c_str(), F_OK) != -1;
}

bool LoadConfig (libconfig::Config &cfg, std::vector<std::string> &types, std::vector<std::string> &process_servers) {
    std::string path = FLAGS_base_dir + FLAGS_conf_dir + FLAGS_conffile;
    if (!CheckExist(path)) {
        LOG(ERROR) << path << " not exists";
        return false;
    }
    try {
        cfg.readFile(path.c_str());
        libconfig::Setting &root = cfg.getRoot();
        std::string alltypes, allservers;
        root.lookupValue("supportcfg", alltypes);
        root.lookupValue("process_server_cfg", allservers);
        boost::algorithm::split(types, alltypes, boost::is_any_of("|"));
        boost::algorithm::split(process_servers, allservers, boost::is_any_of("|"));
    } catch (std::exception &err) {
        LOG(ERROR) << "parse " << path << " fail with err " << err.what();
        return false;
    }

    return true;
}

int main (int argc, char *argv[]) {
    libconfig::Config cfg;
    std::vector<std::string> types, process_servers;
    if (!LoadConfig(cfg, types, process_servers)) {
        exit(-1);
    }
    libconfig::Setting &root = cfg.getRoot();

    DataFieldRuler datafieldruler;
    datafieldruler.LoadConfig(root["preprocess_action"], types, root["srcid_cfg"]);
    //datafieldruler will be move rightvalue, don't use later!
    DataPreProcess datapreprocesser(root["preprocess"], datafieldruler, root["redis_cfg"], process_servers);
    DataAccesserMgr dataaccessmgr(root["access"], types, root["srcid_cfg"]);
    while(1);
    return 0;
}
