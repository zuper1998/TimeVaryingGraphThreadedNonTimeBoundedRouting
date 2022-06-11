//
// Created by Narcano on 6/11/2022.
//

#ifndef TVGGRAPHCALC_DATANODE_H
#define TVGGRAPHCALC_DATANODE_H

#include <string>
#include <vector>
#include <map>
#include "../../Graph/IntervalPath.h"
#include "../../Graph/Path.h"
#include "../../Utility.h"

typedef std::tuple<IntervalPath,Path> edge_data;

typedef std::pair<std::string, edge_data> EdgeDataType;
struct CompareStartDate{
    bool operator()(const EdgeDataType& left,const EdgeDataType& right){
        auto [l_i,l_p] = left.second;
        auto [r_i,r_p] = right.second;

        return l_i.intervals.front().start < r_i.intervals.front().start;
    }
};

class DataNode {

    std::string name;
    std::map<std::string,edge_data> data_edge;

public:
    DataNode() = default;
    [[nodiscard]]
    std::vector<edge_data> getEdges() const{
        std::vector<edge_data> ret;
        for(auto [key,value]:data_edge){
            ret.push_back(value);
        }

        return ret;
    }

    /*
     * start node + last edge time
     */
    explicit DataNode(std::string name) : name(std::move(name)){

    }

    /*
     * end node + last edge time
     */
    void addEdge(const std::string& edge_name, edge_data const&data){
        data_edge.try_emplace(edge_name,data);

        if(std::get<0>(data_edge[edge_name]).getThrougput()< std::get<0>(data).getThrougput()){
            data_edge[edge_name] = data;
        }
    }

    std::tuple<std::string,double> getThroughput(){
        EdgeDataType ret = *std::min_element(data_edge.begin(),data_edge.end(),CompareStartDate());
        auto[i_path,path] = ret.second;
        return {ret.first,i_path.getThrougput()};
    }

    /*
     * Used vals are edges that are already used up
     */
    std::tuple<std::string, double> getThroughput(std::vector<std::string> const &used_vals, std::string const& destination) {
        std::tuple<std::string, double> ret;
        TVG_TIME curbest;
        for(auto const& [key,val]: data_edge){
            if(destinationCheck(used_vals, destination, key)) continue;
            auto[i_path,path] = val;
            TVG_TIME cur = i_path.intervals.end()->start;
            if(curbest.getTime()==-1 || curbest<cur){
                curbest = cur;
                std::string e_name = path.visitedInOrder.back()->getName() + Utility::EdgeToStr(i_path.intervals.back());
                ret = {e_name, i_path.getThrougput()};
            }

        }

        return ret;

    }

    [[nodiscard]] static bool
    destinationCheck(const std::vector<std::string> &used_vals, const std::string &destination,
                     const std::string &key) {
        bool is_already_used = std::find(used_vals.begin(), used_vals.end(), key) != used_vals.end();
        bool is_dest = key.find(destination) != std::string::npos;
        return is_already_used || is_dest;
    }


};


#endif //TVGGRAPHCALC_DATANODE_H
