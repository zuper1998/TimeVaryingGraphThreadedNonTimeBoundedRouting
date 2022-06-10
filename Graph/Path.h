#pragma once
#include <utility>
#include <vector>
#include "Edge.h"
#include "Node.h"
#include "IntervalPath.h"

#define INIT    init =true;


/// <summary>
/// Wrapper object for path's between sats
/// </summary>
class Path
{
public:
	/// <summary>
	/// References to edges
	/// </summary>
	std::vector<Edge*> edges;
	std::vector<Node*> visitedInOrder;
    bool init = false;
	Path() = default;
    explicit Path( std::vector<Edge*>  e) :edges(std::move(e)) {
        INIT;
	}
	Path(Edge* e, Node* n) {
		edges.push_back(e);
		visitedInOrder.push_back(n);
		visitedInOrder.push_back(e->start != getLastNode() ? e->start : e->end);
        INIT;
	}
	Path(Path const&p, Edge* e) : edges(p.edges) {

		edges.push_back(e);
		visitedInOrder = p.visitedInOrder;
		visitedInOrder.push_back(e->start != p.getLastNode() ? e->start : e->end);
        INIT;
	}


	/// <summary>
	/// checks both start and end, doesnt check last edge otherwise all edges would count as visited
	/// </summary>
	/// <param name="n"></param>
	/// <returns></returns>
	bool containNode(Node * const  n) const {
		return std::any_of(visitedInOrder.begin(), visitedInOrder.end(), [&](Node* const _n) {return n == _n; });
	}
	[[nodiscard]] Node * getLastNode() const {
		return visitedInOrder.back();
	}
	/// <summary>
	/// Can give edge where he is an end
	/// </summary>
	/// <returns></returns>
	[[nodiscard]] std::vector<Edge*> getLastNodeEdges() const {
		return getLastNode()->getEdges();
	}
	
	void RemoveVisIntervals(IntervalPath const&,double);

	friend std::ostream& operator << (std::ostream& os, const Path& p );
};



