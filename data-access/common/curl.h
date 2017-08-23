#ifndef DATAACCESS_COMMON_LIBCURL_H
#define DATAACCESS_COMMON_LIBCURL_H
#include "curl/curl.h"
#include "glog/logging.h"
#include <string>

namespace common {

class HttpClient {
    std::string err_;
    bool err_flag_;

    static size_t OnWriteData (void *buffer, size_t size, size_t nmemb, void *lpVoid) {
        std::string *str = dynamic_cast<std::string*>((std::string *)lpVoid);
        if (!str || !buffer) {
            return -1;
        }

        char* pData = (char*)buffer;
        str->append(pData, size * nmemb);
        return nmemb;
    }

public:
    HttpClient(): err_flag_(false), err_("") {}
    ~HttpClient(){}

    int Post(const std::string &url, const std::string &content, std::string &response);
    int Get(const std::string &url, std::string &response);
    //https. temply not impl.
    int Posts(const std::string &url, const std::string &content, std::string &response);
    int Gets(const std::string &url, std::string &response);
};

};
#endif
