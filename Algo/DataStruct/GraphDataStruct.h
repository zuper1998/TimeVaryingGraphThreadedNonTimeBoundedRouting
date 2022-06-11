//
// Created by Narcano on 6/11/2022.
//

#ifndef TVGGRAPHCALC_GRAPHDATASTRUCT_H
#define TVGGRAPHCALC_GRAPHDATASTRUCT_H

#include <sstream>
#include <unordered_map>
#include <map>
#include <iostream>
#include <filesystem>
#include <vector>
#include <sstream>
#include <chrono>
#include <fstream>
#include "../../Graph/IntervalPath.h"
#include "../../Graph/Path.h"

#include "DataNode.h"
#include "../../Utility.h"

class GraphDataStruct {
    /*
     * Key is the node name (start + interval times) --> all data can be contained withing one bestroute
     */
    std::map<std::string, DataNode> data_map;

public:
    void addPath(const IntervalPath &, Path const &);
    void addPath(std::tuple< IntervalPath , Path> data){
        addPath(std::get<0>(data),std::get<1>(data));
    }


    double getBestForNode(std::string const &edge_name, const std::string& destination);

    void concat(const GraphDataStruct &aStruct);
};


#endif //TVGGRAPHCALC_GRAPHDATASTRUCT_H
