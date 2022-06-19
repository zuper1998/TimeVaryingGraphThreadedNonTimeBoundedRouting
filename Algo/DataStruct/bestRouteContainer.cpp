//
// Created by Narcano on 6/11/2022.
//

#include "GraphDataStruct.h"


void print_edge(std::ostream &os, const IntervalPath &ip, const Path &path, int i) {
    Edge * edge  = path.edges.at(i);
    VisibilityInterval visibilityInterval = ip.intervals.at(i);
    os << edge->start->getName() << " | " << edge->end->getName() << " | " << visibilityInterval.start.getTime()  << " | " << visibilityInterval.end.getTime() << " | ";
    for(double tr : visibilityInterval.transmittance){
        os << tr << " ";
    }
    os<<std::endl;
}

/*
 * Data format:
 * TR: throughput
 * Start node | End node | start time | end time | data data data data
 * Start node | End node | start time | end time | data data data data
 * Start node | End node | start time | end time | data data data data
 */
void printToFile(std::ostream &os,IntervalPath const& ip, Path const& path){
    os << "TR: " << ip.getThrougput() << std::endl;

    for(int i = 0; i < path.edges.size();i++){
        print_edge(os, ip, path, i);
    }


}


void GraphDataStruct::addPath( IntervalPath const&intervalPath, Path const &path) {
    if(intervalPath.intervals.empty()) return;

    std::string start_name = path.visitedInOrder.front()->getName() + Utility::EdgeToStr(intervalPath.intervals.front());
    std::string end_name = path.visitedInOrder.back()->getName() + Utility::EdgeToStr(intervalPath.intervals.back());
    data_map.try_emplace(start_name, DataNode(start_name));
    data_map[start_name].addEdge(end_name, {intervalPath, path});

}

double
GraphDataStruct::getBestForNode(std::string const &edge_name, const std::string &destination, std::ostream &dataFile) {

    std::vector<std::string> used;
    double tr = 0;

    for (auto[key,val] : data_map){
        auto[loc_name, i_path , path] = val.getEdge(used, destination);
        used.push_back(loc_name);
        tr+=i_path.getThrougput();
        printToFile(dataFile, i_path,path);
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

