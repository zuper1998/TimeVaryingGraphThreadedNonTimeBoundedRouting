#pragma once
#include <vector>
#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include "VisibilityInterval.h"
class Node;
/// <summary>
/// Edge class for the TVG
/// The edges should be bidirectional!
/// </summary>
class Edge
{
	std::vector<VisibilityInterval> vis_intervals = std::vector<VisibilityInterval>();
	mutable std::shared_mutex mutex_;
public:
	Node* start;
	Node* end;
	Edge(Node* n1,Node* n2);
	void addVisInterval(VisibilityInterval);
	//Delete copy ctor
	Edge operator=(Edge other) noexcept = delete;
	bool operator ==(Edge const&)const;
	friend std::ostream& operator << (std::ostream& os, Edge const &e );

	static bool compareVis(VisibilityInterval const& i1, VisibilityInterval const& i2) {return i1.getThrougput() < i2.getThrougput(); }
	/// <summary>
	/// Used to get the target value
	/// </summary>
	/// <returns></returns>
	double getLargestThrougput() {
		auto v1 = std::max_element(vis_intervals.begin(), vis_intervals.end(), compareVis);
		return v1->getThrougput();
	}

	std::vector<VisibilityInterval>  getVisIntervalVal() {
		std::shared_lock lock(mutex_);
		return vis_intervals;
	}
	std::vector<VisibilityInterval>&  getVisIntervalRef() {
		std::unique_lock lock(mutex_);
		return vis_intervals;
	}

	void RemoveVis(VisibilityInterval const& vi, double tr);

};

