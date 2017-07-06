#include "process_adapt.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

using namespace dataprocess;

DEFINE_string(base_dir, "/home/work/data-access/", "project dir");
DEFINE_string(conf_dir, "conf/", "project conf dir");
DEFINE_string(conffile, "data-access.cfg", "project conf file");

bool CheckExist (const std::string path) {
    return access(path.c_str(), F_OK) != -1;
}

bool LoadConfig (libconfig::Config &cfg) {
    std::string path = FLAGS_base_dir + FLAGS_conf_dir + FLAGS_conffile;
    if (!CheckExist(path)) {
        LOG(ERROR) << path << " not exists";
        return false;
    }

    try {
        cfg.readFile(path.c_str());
    } catch (std::exception &err) {
        LOG(ERROR) << "parse " << path << " fail with err " << err.what();
        return false;
    }

    return true;
}

int main (int argc, char *argv[]) {
    libconfig::Config cfg;
    if (!LoadConfig(cfg)) {
        exit(-1);
    }
    libconfig::Setting &root = cfg.getRoot();
    ProcessAdapt process_adapter(root["process"]);
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
