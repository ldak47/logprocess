#ifndef DATAACCESS_PROCESS_CHECK_ACTION_H
#define DATAACCESS_PROCESS_CHECK_ACTION_H
#include "client_hook.h"
#include "bitmap.h"
#include <regex>

namespace dataprocess {

bool srcid_check(const std::string &srcid, const process::SrcidConfig &cfg);
bool seword_check(const std::string &query,const process::ScanCondSeWord &cfg);
bool lbs_check(const std::string &prov, const std::string &city, const process::LocationInfo &cfg);
bool cookie_check(const std::string &cookie, const process::CookieConfig &cfg);
bool cluster_check(const std::string &cluster, const process::ClusterConfig &cfg);
bool resno_check(const std::string &resno, const process::ResnoConfig &cfg);
bool sid_check(const std::string &sid, const process::SidConfig &cfg);
bool type_check(const std::string &type, const process::TypeConfig &cfg);

bool all_check(const process::CheckListCond &conds, const std::map<std::string, std::string> &item);

};


#endif
