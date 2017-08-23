#include "process_stat.h"

namespace dataprocess {

std::vector<std::string> ProcessStater::GetStaters () {
    std::unique_lock<std::mutex> lock(mtx);
    std::vector<std::string> notices;
    std::map<std::string, std::function<void (std::map<std::string, std::string>)>>::iterator it = staters.begin();
    for (; it != staters.end(); it++) {
        notices.push_back(it->first);
    }
    return notices;
}

void ProcessStater::AddStater (const std::string notice, std::function<void (const std::map<std::string, std::string> &)> stater) {
    std::unique_lock<std::mutex> lock(mtx);
    if (staters.find(notice) == staters.end()) {
        staters.insert(make_pair(notice, stater));
    }
}

void ProcessStater::DelStater (const std::string notice) {
    std::unique_lock<std::mutex> lock(mtx);
    if (staters.find(notice) != staters.end()) {
        staters.erase(staters.find(notice));
    }
}

void ProcessStater::Run (const std::map<std::string, std::string> &kv) {
    std::unique_lock<std::mutex> lock(mtx);
    std::map<std::string, std::function<void (std::map<std::string, std::string>)>>::iterator it = staters.begin();
    for (; it != staters.end(); it++) {
        it->second(kv);
    }
}



};
