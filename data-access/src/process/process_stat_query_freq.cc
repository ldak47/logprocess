#include "process_stat_query_freq.h"

DEFINE_string(basedir, "/home/work/data-access/", "project dir");
DEFINE_string(confdir, "conf/", "project conf dir");
DEFINE_string(conf_file, "data-access.cfg", "project conf file");
DEFINE_string(datafile_field, "datarelativepath", "data relative path");
DEFINE_string(datafile_key, "query_stat", "datafile name");

namespace dataprocess {

static uint32_t FNV1_32_HASH (std::string key, int len) {
    int p = 16777619;
    int hash = (int)2166136261L;
    for (int i = 0; i < len; i++) {
        hash = (hash ^ key[i]) * p;
    }

    hash += hash << 13;
    hash ^= hash >> 7;
    hash += hash << 3;
    hash ^= hash >> 17;
    hash += hash << 5;
    if (hash < 0) {
        hash = (0 - hash);
    }

    return hash;
}

std::mutex QueryFreqStat::qfmtx;
std::unique_ptr<common::Trie> QueryFreqStat::qftree = std::make_unique<common::Trie>();
    
void QueryFreqStat::UpdataQueryFreqStat (const std::map<std::string, std::string> &kv) {
    std::unique_lock<std::mutex> lock(qfmtx);
    std::map<std::string, std::string>::const_iterator qit = kv.find("query"), sit = kv.find("srcid"), rit = kv.find("resno");
    
    if (qit != kv.end() && sit != kv.end() && rit != kv.end() && rit->second == "") {
        //trick for test new weather
        if (sit->second != "4514") {
            //std::string str = qit->second + "_" + sit->second;
            std::string str = qit->second;
            qftree->Insert(FNV1_32_HASH(str, str.length()));
        }
#if 0
        lock.unlock();
        uint32_t cnt;
        SearchExists(qit->second, sit->second, cnt);
        LOG(INFO) << cnt;
#endif
    }
}

bool QueryFreqStat::SearchExists(const std::string &query, const std::string &srcid, uint32_t &cnt) {
    bool res;
    {
        std::unique_lock<std::mutex> lock(qfmtx);
        //std::string str = query + "_" + srcid;
        std::string str = query;
        res = qftree->Search(FNV1_32_HASH(str.c_str(), str.length()), cnt);
    }
    return res;
}

std::string QueryFreqStat::GetDataFile () {
    try {
        std::string path = FLAGS_basedir + FLAGS_confdir + FLAGS_conf_file;
        if (access(path.c_str(), F_OK) != -1) {
            libconfig::Config cfg;
            cfg.readFile(path.c_str());
            libconfig::Setting &root = cfg.getRoot();
    
            std::string field = FLAGS_datafile_field, key = FLAGS_datafile_key;
            libconfig::Setting &setting = root[field.c_str()];
            std::string filename;
            setting.lookupValue(key.c_str(), filename);
            return filename;
        } else {
            LOG(ERROR) << "QueryFreqStat dump fail with conf file can not access.";
        }
    } catch (std::exception &error) {
        LOG(ERROR) << "QueryFreqStat dump fail with err " << error.what();
    }
    
    return "";
}

void QueryFreqStat::Dump () {
    std::unique_lock<std::mutex> lock(qfmtx);
    std::unordered_map<uint32_t, uint32_t> dumpmap;
    qftree->Dfs(dumpmap);
    
    std::string filename = GetDataFile();
    if (filename != "") {
        std::unordered_map<uint32_t, uint32_t>::iterator it = dumpmap.begin();
        std::ofstream ofs(filename.c_str(), std::ios::out|std::ios::trunc);
        for (; it != dumpmap.end(); it++) {
            ofs << it->first << "\t" << it->second;
        }

        ofs.close();
        lock.unlock();
    } else {
        lock.unlock();
    }
}

void QueryFreqStat::Load () {
    std::unique_lock<std::mutex> lock(qfmtx);
    std::string filename = GetDataFile();
    if (filename != "") {
        std::ifstream ifs(filename.c_str());
        std::string line;
        while (!ifs.eof()) {
            getline(ifs, line);
            if (line == "") {
                continue;
            }

            std::vector<std::string> tmp;
            boost::algorithm::split(tmp, line, boost::is_any_of("\t"));
            if (tmp.size() == 2) {
                uint32_t hashcode = static_cast<uint32_t>(stoi(tmp[0]));
                int times = stoi(tmp[1]);
                for (int i = 0; i < times; i++) {
                    qftree->Insert(hashcode);
                }
            }
        }
        ifs.close();
        
        lock.unlock();
    } else {
        lock.unlock();
    }
}

};
