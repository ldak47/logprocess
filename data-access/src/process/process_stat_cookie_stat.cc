#include "process_stat_cookie_stat.h"

DEFINE_string(dataonredis_field, "dataonredis", "data on redis");
DEFINE_string(dataonredis_key, "twemproxyaddr", "twemproxy redis cluster proxy addr");
DEFINE_string(dataonredis_server, "127.0.0.1", "twemproxy redis cluster proxy addr");
DEFINE_int32(dataonredis_port, 6380, "twemproxy redis cluster proxy port");

namespace dataprocess {

std::mutex CookieStat::ckmtx;
std::unique_ptr<common::RedisCli> CookieStat::ckrd_handle = std::make_unique<common::RedisCli>(FLAGS_dataonredis_server, FLAGS_dataonredis_port);
    
void CookieStat::UpdateCookieStat (const std::map<std::string, std::string> &kv) {
    std::unique_lock<std::mutex> lock(ckmtx);
    std::map<std::string, std::string>::const_iterator cit = kv.find("cookieid"), qit = kv.find("query"), tit = kv.find("timestamp");
    if (qit != kv.end() && tit != kv.end() && cit != kv.end()) {
        std::string query = qit->second, timestamp = tit->second, cookie = cit->second;
        int64_t tm = static_cast<int64_t>(strTime2Unixtime(timestamp));
#if 1
        //temply doing it
        std::string q("");
        for (size_t i = 0; i < query.length(); i++) {
            if (query[i] != ' ') {
                q += query[i];
            }
        }
#endif
        bool res = ckrd_handle->Zset(cookie, q, tm);
        if (!res) {
            std::string errmsg;
            ckrd_handle->GetError(errmsg);
            LOG(ERROR) << "storage cookie info " << query << " to redis fail with err " << errmsg;
        }
    }
}

void CookieStat::Search (const std::string &cookie, std::vector<query_record> &records, time_t st, time_t ed) {
    std::unique_lock<std::mutex> lock(ckmtx);
    if (st < ed) {
        std::map<std::string, std::string> tmpres;
        bool res = ckrd_handle->ZrangeByScoreWithScore(cookie, tmpres, std::to_string(st), std::to_string(ed));
        if (res) {
            std::string errmsg;
            ckrd_handle->GetError(errmsg);
            LOG(ERROR) << "storage cookie info to redis fail with err " << errmsg;
        } else if (tmpres.size()) {
            std::map<std::string, std::string>::iterator it = tmpres.begin();
            for (; it != tmpres.end(); it++) {
                time_t tm = static_cast<time_t>(stoi(it->first));
                std::string query = it->second;
                query_record rec(tm, query);
                records.push_back(rec);
            }
        }
    }
}

};
