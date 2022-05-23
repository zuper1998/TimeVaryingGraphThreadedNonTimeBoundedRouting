#pragma once
#include "../Graph/Path.h"
#include "../Graph/IntervalPath.h"
class CalcBestPath
{
public:
	/// <summary>
	/// 
	/// </summary>
	/// <param name="p"></param>
	/// <returns>Tuple value of <InternalPath path, double throughput> </returns>
	std::tuple<IntervalPath,double> static calculateBestPath(Path& p) {

		std::vector < IntervalPath> data;
		std::vector < IntervalPath> tmp;
		//Init vector
		for (auto& visEdge : p.edges.front()->getVisIntervalVal()) {

			tmp.emplace_back(visEdge);
		}


		//Try all paths in BFS fashion index from 1 so dont add first edge all over again
		//It goes very quickly to the O(n^n) teritory, optimization is required. 
		for (int i = 1; i < p.edges.size(); i++) {
			data = tmp;
			tmp.clear();
			for (auto& ip : data) {
				for (auto& visEdge : p.edges.at(i)->getVisIntervalVal()) {

					if (ip.canBeAdded(visEdge)) {
						tmp.emplace_back(ip,visEdge);
					}
				}		
			}

		}

		double max=0;
		IntervalPath curBest;
		//Get best path
		for (auto& path : data) {
			if (max < path.getThrougput()) {
				max = path.getThrougput();
				curBest = path;
			}
		}






		return std::make_tuple(curBest,max);


	}
};

