#pragma once

#include <vector>
#include <numeric>
#include "../DefValues.h"
#include "TVG_TIME.h"


/// <summary>
/// The comperassion functions are based on Allen's time interval algebra
/// Edge time start should always be < end
/// </summary>
class VisibilityInterval
{
	double througput = -1;
public:
	VisibilityInterval(TVG_TIME _start, TVG_TIME _end, std::vector<double>t): start(_start), end(_end) {
		
		transmittance = t;
		througput = 0;
		for (auto& d : transmittance) {
			througput += d * DefValues::EntangledRate;
		}

	}
	
	VisibilityInterval(const VisibilityInterval&);
	void removeTrans(double tr)
	{
		std::vector<double>::iterator iter;

		for (iter = transmittance.begin(); iter != transmittance.end();iter++) {
			tr -= *iter*DefValues::EntangledRate;
			if (tr < 0) break;
		}

		transmittance.erase(transmittance.begin(), iter);


	}

	

	/// <summary>
	/// the percent of photons that are able to travel trough the channel 
	/// </summary>
	std::vector<double> transmittance;
	/// <summary>
	/// Seconds from the start of the simulation to the start of the edge
	/// </summary>
	TVG_TIME start;
	/// <summary>
	/// Seconds from the start of the simulation to the end of the edge
	/// </summary>
	TVG_TIME end;
	/// <summary>
	/// This precedes E
	/// </summary>
	/// <param name="e">E</param>
	/// <returns></returns>
	bool operator <(VisibilityInterval& e)const;
	/// <summary>
	/// This is preceded by E
	/// </summary>
	/// <param name="e">E</param>
	/// <returns></returns>
	bool operator >(VisibilityInterval& e) const;
	/// <summary>
	/// Both edges are the same
	/// </summary>
	/// <param name="e">E</param>
	/// <returns></returns>
	bool operator ==(VisibilityInterval const& e) const;
	/// <summary>
	/// TThis meets E
	/// </summary>
	/// <param name="e">E</param>
	/// <returns></returns>
	bool mRelation(VisibilityInterval& e) const;
	bool miRelation(VisibilityInterval& e) const;
	/// <summary>
	/// This overlaps E
	/// </summary>
	/// <param name="e">E</param>
	/// <returns></returns>
	bool oRelation(VisibilityInterval& e) ;
	/// <summary>
	/// Is overlapped by E
	/// </summary>
	/// <param name="e">E</param>
	/// <returns></returns>
	bool oiRelation(VisibilityInterval& e) ;
	/// <summary>
	/// Starts E 
	/// </summary>
	/// <param name="e">E</param>
	/// <returns></returns>
	bool sRelation(VisibilityInterval& e) ;
	/// <summary>
	/// Is started by E
	/// </summary>
	/// <param name="e">E</param>
	/// <returns></returns>
	bool siRelation(VisibilityInterval& e) ;
	/// <summary>
	/// Is during E
	/// </summary>
	/// <param name="e">E</param>
	/// <returns></returns>
	bool dRelation(VisibilityInterval& e) ;
	/// <summary>
	/// Contains E
	/// </summary>
	/// <param name="e">E</param>
	/// <returns></returns>
	bool diRelation(VisibilityInterval& e) ;
	/// <summary>
	/// Finishes E
	/// </summary>
	/// <param name="e">E</param>
	/// <returns></returns>
	bool fRelation(VisibilityInterval& e) ;
	/// <summary>
	/// Is finished by E
	/// </summary>
	/// <param name="e">E</param>
	/// <returns></returns>
	bool fiRelation(VisibilityInterval& e) ;

	double calcDeltaL(VisibilityInterval vi) {
		return  start.getTime()- vi.start.getTime();		
	}
	double calcDeltaR(VisibilityInterval vi) {
		return vi.end.getTime() - end.getTime();

	}
	double calcOmegaL(double curMin) {
		return  curMin - this->start.getTime();
	}
	double calcOmegaR(double curMax) {
		return this->end.getTime() - curMax;

	}
	


	double getThrougput() {
		return througput;
	}
};

