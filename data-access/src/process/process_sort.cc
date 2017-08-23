#include "process_sort.h"
#include "glog/logging.h"

namespace dataprocess {

int QueryCmp (kvpairs &item1, kvpairs &item2) {
    return item1.find("query")->second > item2.find("query")->second;
}    
std::function<int (kvpairs &, kvpairs &)> QueryCompare = std::bind(QueryCmp, std::placeholders::_1, std::placeholders::_2);


int SrcidCmp (kvpairs &item1, kvpairs &item2) {
    return item1.find("srcid")->second > item2.find("srcid")->second;
}
std::function<int (kvpairs &, kvpairs &)> SrcidCompare = std::bind(SrcidCmp, std::placeholders::_1, std::placeholders::_2);
   

int TimestampCmp (kvpairs &item1, kvpairs &item2) {
    return item1.find("timestamp")->second > item2.find("timestamp")->second;
}
std::function<int (kvpairs &, kvpairs &)> TimestampCompare = std::bind(TimestampCmp, std::placeholders::_1, std::placeholders::_2);

};
