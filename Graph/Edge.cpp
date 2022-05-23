#include "Edge.h"




void Edge::RemoveVis(VisibilityInterval const& vi,double tr) {
	std::unique_lock lock(mutex_);
	//We assume that no two visibility intervals start and end at the same time
	auto result = std::find_if(vis_intervals.begin(), vis_intervals.end(), [&](VisibilityInterval const& vInner) {return vi==vInner; });
	if (result != vis_intervals.end()) {
		if (tr >= result->getThrougput()) {
			vis_intervals.erase(result);
		}
		else {
			result->removeTrans(tr);
		}
	}
	else {
		throw "Path doesn't contain what it should";
	}

}




Edge::Edge(Node* n1, Node* n2)
{
	start = n1;
	end = n2;

}
/// <summary>
/// check for duplication 
/// </summary>
/// <param name="vi"></param>
void Edge::addVisInterval(VisibilityInterval vi)
{
	if (std::none_of(vis_intervals.begin(), vis_intervals.end(), [&] (VisibilityInterval outer) {return outer == vi;}))
	vis_intervals.push_back(vi);
}

bool Edge::operator==(Edge const& e) const
{	//Direction matters bc upwards transmittance differs from downwards transmittance
	bool b1 = (start == e.start && end == e.end);
	bool b2 = (start == e.end && end == e.start);
	return b1;// || b2;
}
std::ostream& operator<< (std::ostream& os, Edge const& e)
{
	return os;
}
