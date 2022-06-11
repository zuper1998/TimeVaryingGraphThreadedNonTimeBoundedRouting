#pragma once
#include <vector>
#include <string>
#include "Edge.h"
/// <summary>
/// Time-varying graph 
/// Since there is no need for the edges to be in order in time we can use a more handy representation: One edge between two nodes, which contains all visibility intervals
/// </summary>
class Node 
{
	/// <summary>
	/// Reference to the edges, Edges are shared between objects and are bidirectional. 
	/// </summary>
	std::vector<Edge*> edges; 
	/// <summary>
	/// Name of the node
	/// </summary>
	std::string name;
public:
	Node(std::string name);
	/// <summary>
	/// Should not be used directly, when adding edges use the one in the TVG!
	/// </summary>
	/// <param name="e"></param>
	void addEdge(Edge*e) {
		edges.push_back(e);
	}
	std::string getName() const {
		return name;
	}
	
	std::vector<Edge*> getEdges() {
		return edges;
	}

    [[nodiscard]]
	bool operator ==(const Node& outer) const {
		return name == outer.name;
	}

    std::vector<VisibilityInterval> getAllIntervals(){
        std::vector<VisibilityInterval> ret;
        for(auto&p:edges){
            for(auto&iv :p->getVisIntervalVal()){
                ret.push_back(iv);
            }
        }
        return ret;
    }

	friend std::ostream& operator << ( std::ostream& os, Node n);
};


