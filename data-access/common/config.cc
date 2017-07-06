#include "config.h"

namespace common {

Config::Config (std::string cfgfile) {
    cfgFile_ = cfgfile;
    try {
        cfg_ = cfgFile_.readFile();
    } catch (const FileIOException &fioex) {
        LOG(ERROR) << "readFile fail, with error " << FLAGS_FileReadIOError;
        failure_ = FLAGS_FileReadIOError;
        return;
    } catch (const ParseException &pex) {
        failure_ = FLAGS_ParseError;
        LOG(ERROR) << "pares fail, with error " << FLAGS_ParseError;
        return;
    }

    root_ = cfg_.getRoot();
}
 
libconfig::Setting &Config::GetEntry (libconfig::Setting &entry, std::string field) {
    

}

};
