#include "data_access_field.h"

namespace dataaccess {

AccessField::AccessField (const libconfig::Setting &field_cfg, const std::vector<std:string> &support_type):err(false) {
    for (auto i: support_type) {
        std::string fields;
        try {
            field_cfg.lookupValue(i.c_str(), fields);
            if (fields != "") {
                boost::algorithm::split(tmp, fields, boost::algorithm::is_any_of("|"));
                for (auto j: tmp) {
                    useful_fields.insert(j);
                }
            } else {
                LOG(WARNING) << "DataAccesser Fields fail with cfg is null";
                err = true;
            }
        } catch (std::exception &err) {
            LOG(WARNING) << "DataAccesser Fields fail with " << err.what();
            err = true;
        }
    }
}







};
