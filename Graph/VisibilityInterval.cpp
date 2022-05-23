#include "VisibilityInterval.h"


/// <summary>
/// Copy ctor
/// </summary>
/// <param name=""></param>
VisibilityInterval::VisibilityInterval(const VisibilityInterval& outer)
{
	transmittance.clear();
	std::copy(outer.transmittance.begin(), outer.transmittance.end(), std::back_inserter(transmittance));
	start = outer.start;
	end = outer.end;
	througput = outer.througput;


}

bool VisibilityInterval::operator<(VisibilityInterval&  e) const
{
	return end < e.start;
}

bool VisibilityInterval::operator>(VisibilityInterval& e) const
{
	return start > e.end;
}

bool VisibilityInterval::operator==(VisibilityInterval const& e) const
{
	return start == e.start && end == e.end;
}

bool VisibilityInterval::mRelation(VisibilityInterval& e) const
{
	return end == e.start;
}

bool VisibilityInterval::miRelation(VisibilityInterval& e) const
{
	return start == e.end;
}


bool VisibilityInterval::oRelation(VisibilityInterval&  e) 
{
	return start < e.start&& e.start < end&& end < e.end;
}

bool VisibilityInterval::oiRelation(VisibilityInterval&   e) 
{
	return e.start < start&& start < e.end&& e.end < end;
}


bool VisibilityInterval::sRelation(VisibilityInterval& e) 
{
	return start == e.start && end < e.end;
}

bool VisibilityInterval::siRelation(VisibilityInterval& e) 
{
	return start == e.start && e.end < end;
}

bool VisibilityInterval::dRelation(VisibilityInterval& e) 
{
	return e.start < start&& end < e.end;
}

bool VisibilityInterval::diRelation(VisibilityInterval& e) 
{
	return start < e.start&& e.end < end;
}

bool VisibilityInterval::fRelation(VisibilityInterval& e) 
{
	return end == e.end && e.start < start;
}

bool VisibilityInterval::fiRelation(VisibilityInterval& e) 
{
	return end == e.end && start < e.start;
}
