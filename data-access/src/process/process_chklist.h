#ifndef DATAACCESS_PROCESS_CHECKLIST_H
#define DATAACCESS_PROCESS_CHECKLIST_H
#include "process_chklist_base.h"
#include "process_check_action.h"
#include <string>
#include <regex>

namespace dataprocess {

class checklist_srcid : public checklist_base<process::SrcidConfig> {
    std::vector<std::string> fieldname;
public:
    checklist_srcid() {}
    virtual ~checklist_srcid(){}
    
    virtual void init(const std::vector<process::SrcidConfig> &cfg);
    virtual bool check(const process::TransmitRequest *request);
};

class checklist_seword : public checklist_base<process::ScanCondSeWord> {
    std::vector<std::string> fieldname;
public:
    checklist_seword() {}
    virtual ~checklist_seword(){}

    virtual void init(const std::vector<process::ScanCondSeWord> &cfg);
    virtual bool check(const process::TransmitRequest *request);
};

class checklist_lbs : public checklist_base<process::LocationInfo> {
    std::vector<std::string> fieldname;
public:
    checklist_lbs() {}
    virtual ~checklist_lbs(){}

    virtual void init(const std::vector<process::LocationInfo> &cfg);
    virtual bool check(const process::TransmitRequest *request);
};

class checklist_cookie : public checklist_base<process::CookieConfig> {
    std::vector<std::string> fieldname;
public:
    checklist_cookie() {}
    virtual ~checklist_cookie(){}

    virtual void init(const std::vector<process::CookieConfig> &cfg);
    virtual bool check(const process::TransmitRequest *request);
};

class checklist_cluster : public checklist_base<process::ClusterConfig> {
    std::vector<std::string> fieldname;
public:
    checklist_cluster() {}
    virtual ~checklist_cluster(){}

    virtual void init(const std::vector<process::ClusterConfig> &cfg);
    virtual bool check(const process::TransmitRequest *request);
};

class checklist_sid : public checklist_base<process::SidConfig> {
    std::vector<std::string> fieldname;
public:
    checklist_sid() {}
    virtual ~checklist_sid(){}

    virtual void init(const std::vector<process::SidConfig> &cfg);
    virtual bool check(const process::TransmitRequest *request);
};

class checklist_resno : public checklist_base<process::ResnoConfig> {
    std::vector<std::string> fieldname;
public:
    checklist_resno() {}
    virtual ~checklist_resno(){}
    
    virtual void init(const std::vector<process::ResnoConfig> &cfg);
    virtual bool check(const process::TransmitRequest *request);
};

class checklist_type : public checklist_base<process::TypeConfig> {
    std::vector<std::string> fieldname;
public:
    checklist_type() {}
    virtual ~checklist_type(){}

    virtual void init(const std::vector<process::TypeConfig> &cfg);
    virtual bool check(const process::TransmitRequest *request);
};


};
#endif
