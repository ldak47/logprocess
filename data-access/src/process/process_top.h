#ifndef DATAACCESS_PROCESS_TOPK_H
#define DATAACCESS_PROCESS_TOPK_H
#include "heap_func.h"
#include "process_sort.h"
#include <algorithm>

namespace dataprocess {

class Topk {
    std::vector<kvpairs> data;
    int k;
    CompareOperator &CmpFunc;
    
public:
    Topk(const std::vector<kvpairs> &_data,
         int _k,
         std::function<int (kvpairs &, kvpairs &)> &_CmpFunc);
    ~Topk(){
        data.clear();
    }
    
    std::vector<kvpairs> Run(std::string field);
};

};
#endif
