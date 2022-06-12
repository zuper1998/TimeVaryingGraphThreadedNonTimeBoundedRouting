//
// Created by Narcano on 6/11/2022.
//

#include "GraphDataStruct.h"




void GraphDataStruct::addPath( IntervalPath const&intervalPath, Path const &path) {
    if(intervalPath.intervals.empty()) return;

    std::string start_name = path.visitedInOrder.front()->getName() + Utility::EdgeToStr(intervalPath.intervals.front());
    std::string end_name = path.visitedInOrder.back()->getName() + Utility::EdgeToStr(intervalPath.intervals.back());
    data_map.try_emplace(start_name, DataNode(start_name));
    data_map[start_name].addEdge(end_name, {intervalPath, path});

}

double GraphDataStruct::getBestForNode(std::string const &edge_name, const std::string& destination) {

    std::vector<std::string> used;
    double tr = 0;

    for (auto[key,val] : data_map){
        auto[loc_name,loc_tr] = val.getThroughput(used, destination);
        used.push_back(loc_name);
        tr+=loc_tr;
    }

    return tr;
}



void GraphDataStruct::concat(const GraphDataStruct &aStruct) {
    for(auto[key,val] : aStruct.data_map){
        for(auto& edge : val.getEdges()){
            auto[i,p] = edge;
            addPath(i,p);
        }
    }

}

