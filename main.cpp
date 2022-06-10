#include <iostream>

#include "Graph/TVG.h"
#include "Graph/Path.h"
#define PRINT(X) std::cout << X << std::endl;
void retEdges() {
    TVG tvg(R"(C:\Users\Narcano\CLionProjects\TVGGraphCalc\DataIn\QSAT_CROS_LOW.txt.satNetwork)");
    for (const Path& p : tvg.findRoutesBetween("Budapest", "Helsinki")) {
        std::cout << p;
    }




}

int main()
{
    retEdges();

}
