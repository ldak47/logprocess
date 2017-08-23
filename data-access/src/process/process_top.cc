#include "process_top.h"

namespace dataprocess {

Topk::Topk (const std::vector<kvpairs> &_data, int _k, std::function<int (kvpairs &, kvpairs &)> &_CmpFunc): data(_data), k(_k), CmpFunc(_CmpFunc) {
}

std::vector<kvpairs> Topk::Run (std::string field) {
    std::vector<kvpairs> topdata;
    for (size_t i = 0; i <= k; ) {
        if (data[i].find(field) != data[i].end()) {
            topdata.push_back(data[i]);
            ++i;
        }
    }
    common::Heap<kvpairs> hp(topdata, CmpFunc);

    for (size_t i = k + 1; i < data.size(); i++) {
        if (data[i].find(field) == data[i].end()) {
            continue;
        }
        hp.Setop(data[i]);
    }

    hp.Sort();
    return hp.GetData();
}








};
