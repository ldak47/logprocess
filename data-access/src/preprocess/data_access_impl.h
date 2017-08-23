#ifndef DATAACCESS_ACCESS_IMPL_H
#define DATAACCESS_ACCESS_IMPL_H
#include "libconfig.h++"
#include "range_iterator.h"
#include "client_hook.h"
#include "data_access_pv.h"
#include "data_access_filter.h"
#include "data_preprocess_action.h"
#include <boost/lockfree/queue.hpp>

namespace dataaccess {

class Access_Impl {
    PvStater &pvstater_;
    DataFieldRuler &datafieldruler_;
    AccessFilter &accessfilter_;
    bool &switch_old_;
    bool &switch_new_;
    
public:
    Access_Impl(PvStater &pvstater,
                DataFieldRuler &datafieldruler,
                AccessFilter &accessfilter,
                bool &switch_old,
                bool &switch_new);
    ~Access_Impl(){}

    bool Init(const libconfig::Setting &filter_cfg, const libconfig::Setting &field_cfg);
    
    void SetLogFilterConfig(
                            const manage::SetLogFilterConfigRequest *request,
                            manage::SetLogFilterConfigResponse *response,
                            ::google::protobuf::Closure *done
                            );

    void GetLogFilterConfig(
                            const manage::GetLogFilterConfigRequest *request,
                            manage::GetLogFilterConfigResponse *response,
                            ::google::protobuf::Closure *done
                            );

    void SetLogFieldConfig(
                            const manage::SetLogFieldConfigRequest *request,
                            manage::SetLogFieldConfigResponse *response,
                            ::google::protobuf::Closure *done
                            );

    void GetLogFieldConfig(
                           const manage::GetLogFieldConfigRequest *request,
                           manage::GetLogFieldConfigResponse *response,
                           ::google::protobuf::Closure *done
                           );

    void GetLogTransmitStat(
                            const manage::GetLogTransmitStatRequest *request,
                            manage::GetLogTransmitStatResponse *response,
                            ::google::protobuf::Closure *done
                            );
    void AddLogTransmitStat(
                            const manage::AddLogTransmitStatRequest *request,
                            manage::AddLogTransmitStatResponse *response,
                            ::google::protobuf::Closure *done
                            );
    void PullSwitchConfig(
                            const manage::PullSwitchConfigRequest *request,
                            manage::PullSwitchConfigResponse *response,
                            ::google::protobuf::Closure *done
                         );
    void Retransmit(
                    const manage::RetransmitRequest *request,
                    manage::RetransmitResponse *response,
                    ::google::protobuf::Closure *done
                    );
};

};

#endif
