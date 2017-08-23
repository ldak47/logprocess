#include "data_access_filter.h"

namespace dataaccess {

AccessFilter::AccessFilter (const libconfig::Setting &filter_cfg, const std::vector<std::string> &support_type):err(false) {
    for (auto i: support_type) {
        std::string srcids;
        try {
            filter_cfg.lookupValue(i.c_str(), srcids);
            if (srcids != "") {
                std::vector<std::string> tmp;
                boost::algorithm::split(tmp, srcids, boost::algorithm::is_any_of("|"));
                for (auto j: tmp) {
                    useful_srcids.insert(j);
                }
            } else {
                LOG(WARNING) << "DataAccesser ParseLegalSrcids fail with cfg is null";
                err = true;
            }
        } catch (std::exception &error) {
            LOG(WARNING) << "DataAccesser ParseLegalSrcids fail with " << error.what();
            err = true;
        }
    }
}







};
