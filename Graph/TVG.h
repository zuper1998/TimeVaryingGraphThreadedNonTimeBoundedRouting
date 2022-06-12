#pragma once
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <queue>
#include <atomic>
#include <future>
#include "../LoadException.h"
#include "../Lib/thread-pool-2.0.0/thread_pool.hpp"
#include "Node.h"
#include "Path.h"
#include "../Algo/DataStruct/Thread_safe_queue.h"
#include "../Algo/DataStruct/GraphDataStruct.h"
/// <summary>
/// Class for the whole graph
/// </summary>
#define CalculatedIntervals std::vector<std::tuple<IntervalPath, Path>>
using namespace  std::chrono_literals;

struct satTemp {
	std::string name;
	/// <summary>
	/// transmittance vals (separated with ,), dst node, start time from start, end time from start
	/// </summary>
	std::vector<std::string> lines;
};

struct DataHolder {
	/// <summary>
	/// transmittance
	/// </summary>
	std::string values;
	/// <summary>
	/// source node
	/// </summary>
	std::string src;
	/// <summary>
	/// dest node
	/// </summary>
	std::string dst;
	/// <summary>
	/// start time since sim start
	/// </summary>
	std::string start;
	/// <summary>
	/// end time since sim start
	/// </summary>
	std::string end;

};

class TVG
{
	/// <summary>
	/// List of the nodes in the graph
	/// </summary>
	std::vector<Node> nodes;
    std::vector<Edge*> edges;
public:
	thread_pool* pool;



	explicit TVG(const std::string& f) {
		try {
			readFromFile(f);
		}
		catch (const char *e) {
			std::cerr << e; 

		}
		//1 + 4*x
		pool = new thread_pool(6);
	}

    TVG operator =(TVG) = delete;
    TVG (const TVG&) = delete;
    ~TVG(){
        for(Edge *edge : edges){
            delete edge;
        }
    }
	/// <summary>
	/// Add edge to the nodes it contains
	/// </summary>
	/// <param name="e"></param>
	void addEdge(Edge* e);
	void addEdge(const DataHolder&);
	Node* findNode(const std::string&);
	void readFromFile(const std::string&);
	/// <summary>
	/// Check if the edge is added (match between the start and end point)
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	static bool edgeAlreadyAdded(Edge*);
	/// <summary>
	/// Gets the edge where if its a match between the start and end point
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	static Edge* findEdge(Edge*);
    static Edge* findEdge(Node* n1,Node* n2);

    GraphDataStruct
    findRoutesBetween(const std::string &src, const std::vector<std::string>& dests);
    std::vector<std::string> getCitiesWithout(const std::string&);

    std::vector<Node*>  getNodesWithout(Node *pNode);

    static bool edgeAlreadyAdded(Node *n1, Node *n2);
};


