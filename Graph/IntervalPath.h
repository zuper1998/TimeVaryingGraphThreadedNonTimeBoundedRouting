#pragma once
#include <vector>
#include "VisibilityInterval.h"
#include "../DefValues.h"
class IntervalPath
{
	void addPath(const VisibilityInterval& vi) {
		curMax = curMax > vi.end ? curMax : vi.end;
		curMin = curMin < vi.start ? curMin : vi.start;
		intervals.push_back(vi);
	}
public:
	std::vector<VisibilityInterval> intervals;
	/// <summary>
	/// The current min and max time of intervals
	/// </summary>
	TVG_TIME curMax, curMin;

	IntervalPath() {
		curMax = curMin = 0;
	}

	explicit IntervalPath(const VisibilityInterval& vi) : curMax(vi.end), curMin(vi.start) {
		addPath(vi);
	}

	IntervalPath(const IntervalPath& outer)  = default;


	explicit IntervalPath(IntervalPath const& parent, VisibilityInterval const& vi) {
		*this = parent;
		addPath(vi);
	}






	/// <summary>
	/// Left to right ---> time
	/// See Algo for more info 
	/// </summary>
	/// <param name="vi"></param>
	/// <returns></returns>
	[[nodiscard]] bool canBeAdded(VisibilityInterval vi) const {
		TVG_TIME avg = getAvarage(curMin, curMax);
		//The tester interval
		double startTime = std::max( avg.getTime() - DefValues::maxWindow / 2,0.0); 
		double endTime = startTime + DefValues::maxWindow;
		VisibilityInterval tester = VisibilityInterval(TVG_TIME(startTime), TVG_TIME(endTime), std::vector < double >());

		if (tester.diRelation(vi)) {
			return true;
		}
		//check if it only overflows on one side
		if (tester.start>vi.start && tester.end<vi.end) {
			return false;
		}

		double deltaL = tester.calcDeltaL(vi);
		double deltaR = tester.calcDeltaR(vi);
		double omegaL = tester.calcOmegaL(curMin.getTime());
		double omegaR = tester.calcOmegaR(curMax.getTime());
		bool isL = deltaL > 0;

		double omega = isL ? omegaR : omegaL;
		double delta = isL ? deltaL : deltaR;

		return omega > delta;




	}
	/// <summary>
	/// Min val since throughput 
	/// </summary>
	/// <returns></returns>
	double getThrougput() {
        if(intervals.empty()) return 0;
		double ret = intervals.front().getThrougput();

		for (auto& edge : intervals) {
			ret = std::min(ret, edge.getThrougput());
		}

		return ret;
	}

};

