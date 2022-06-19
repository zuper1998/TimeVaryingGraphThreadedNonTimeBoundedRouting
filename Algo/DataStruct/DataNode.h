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
        double l_tr = std::get<0>(data_edge[edge_name]).getThrougput();
        double r_tr = std::get<0>(data).getThrougput();
        if(l_tr< r_tr ){
            data_edge[edge_name] = data;
        }
    }


    /*
     *  Get best edge that is not already used
     */
    std::tuple<std::string,IntervalPath,Path> getEdge(std::vector<std::string> const &used_vals, std::string const& destination){
        std::tuple<std::string,IntervalPath,Path> ret;
        for(auto const& [key,val]: data_edge){
            if(!destinationCheck(used_vals, destination, key)) continue;
            auto[i_path,path] = val;
            double tr = i_path.getThrougput();
            if(std::get<1>(ret).getThrougput() < tr){
                std::string e_name = path.visitedInOrder.back()->getName() + Utility::EdgeToStr(i_path.intervals.back());
                ret = {name,i_path,path};
            }

        }

        return ret;
    }


    [[nodiscard]] static bool
    destinationCheck(const std::vector<std::string> &used_vals, const std::string &destination,
                     const std::string &key) {
        bool is_not_used = std::find(used_vals.begin(), used_vals.end(), key) == used_vals.end();
        bool is_dest = key.find(destination) != std::string::npos;
        return is_not_used && is_dest;
    }

    size_t getMemoryUsage(){
        size_t ret =0;
        for(auto[key,val] : data_edge){
            ret+= sizeof(val);
        }
        return ret;
    }

    void printPaths(){
        std::cout<<name << std::endl;
        for(auto[key,val] : data_edge){
            Path path = std::get<1>(val);

            for(Edge* e: path.edges){
                std::cout << e->start->getName() << " --> " << e->end->getName() << " | ";
            }
            std::cout << std::get<0>(val).getThrougput();
            std::cout<<"\n";
        }
    }

};


#endif //TVGGRAPHCALC_DATANODE_H
