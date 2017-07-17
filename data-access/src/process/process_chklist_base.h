#ifndef DATAACCESS_PROCESS_CHECKLIST_BASE_H
#define DATAACCESS_PROCESS_CHECKLIST_BASE_H
#include "client_hook.h"
#include "factory.h"
#include <vector>

namespace dataprocess {
    
template<class Type> class checklist_base {
public:
    std::vector<Type> cfg_;
    checklist_base(){}
    virtual ~checklist_base(){}

    virtual void init(const std::vector<Type> cfg) {
        for (size_t i = 0; i < cfg.size(); i++) {
            cfg_.push_back(cfg[i]);
        }
    }
    virtual bool check(process::TransmitRequest *request){}
};

/*
 * template combound reflect, still need something else to do
 * 
REGISTER_FACTORY(checklist_base);
#define REGISTER_CHECKLIST_TYPE(checklist_item) REGISTER_CLASS(checklist_base, checklist_item)
*/

};
#endif
