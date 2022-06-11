//
// Created by Narcano on 6/11/2022.
//

#ifndef TVGGRAPHCALC_UTILITY_H
#define TVGGRAPHCALC_UTILITY_H



#include <sstream>

#include "Graph/IntervalPath.h"


class Utility{
public:
    static std::string EdgeToStr(const VisibilityInterval &interval) {
        std::stringstream buff;
        buff << "|" << interval.start.getTime() << interval.end.getTime() << "|";

        return buff.str();
    }

};

#endif //TVGGRAPHCALC_UTILITY_H
