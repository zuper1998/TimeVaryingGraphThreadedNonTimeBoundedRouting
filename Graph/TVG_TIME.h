#pragma once
#include <iostream>

class TVG_TIME
{
	/// <summary>
	/// Seconds from the start of the simulation. NOT an absolute value!
	/// </summary>
	double time=0;
public:
	TVG_TIME(double t) {
		time = t;
	}
	TVG_TIME() {
		time = -1;
	}
	bool operator < (TVG_TIME const& outer)const{
		return time < outer.time;
	}
	bool operator > (TVG_TIME  const& outer)const {
		return time > outer.time;
	}
	bool operator == (TVG_TIME const& outer)const {
		return time == outer.time;
	}


	[[nodiscard]] double getTime() const { return time; }

	friend TVG_TIME getAvarage(TVG_TIME t1, TVG_TIME t2) {
		double half = (t1.getTime() + t2.getTime()) / 2.0;
		return {half};
	}

};

