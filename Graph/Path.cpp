#include "Path.h"

std::ostream& operator<<( std::ostream& os, Path const& p)
{
	for (Edge *e : p.edges) {
		os << *e->start << " --> " << *e->end << " -->" ;
	}
	os << std::endl;
	return os;
}

void Path::RemoveVisIntervals(IntervalPath const& p,double tr)
{
	//We assume that the edges are in good order 


	for (int i = 0; i < p.intervals.size();i++) {
		Edge* curr = edges.at(i);
		curr->RemoveVis(p.intervals.at(i),tr);

	}

}
