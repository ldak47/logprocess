#ifndef DATAACCESS_PROCESS_SORT_H
#define DATAACCESS_PROCESS_SORT_H
#include <map>
#include <functional>

namespace dataprocess {

using kvpairs = std::map<std::string, std::string>;
using CompareOperator = std::function<int (kvpairs &, kvpairs &)>;

extern std::function<int (kvpairs &, kvpairs &)> QueryCompare;
extern std::function<int (kvpairs &, kvpairs &)> SrcidCompare;
extern std::function<int (kvpairs &, kvpairs &)> TimestampCompare;

};
#endif
