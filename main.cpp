#include <iostream>

#include "Graph/TVG.h"
#include "Graph/Path.h"
#include "Algo/postProcess.h"
#include <filesystem>

#define PRINT(X) std::cout << X << std::endl;

using std::filesystem::directory_iterator;



void retEdges() {
    std::string path = R"(../InputData)";
    if(DefValues::isTest){
        path = R"(../TestInputData)";
    }
    for (const auto &file: directory_iterator(path)) {
        PRINT(file.path());


        TVG tvg(file.path().string());
        std::vector<std::string> cities = tvg.getCitiesWithout("");

        for (const std::string &start: cities) {

            if(DefValues::isTest && start.starts_with("Test")){
                continue;
            }

            Node *findNode = tvg.findNode(start);
            auto destinations = tvg.getNodesWithout(tvg.findNode(start));
            postProcess::writeToFile(file,
                    tvg.findRoutesBetween(start, tvg.getCitiesWithout(start)),
                    findNode,
                    destinations
            );
            break;
        }
    }


}

int main() {
    retEdges();

}
