//
// Created by Narcano on 6/10/2022.
//

#include "postProcess.h"

#include <utility>


static double **genNXNMatrix(size_t n) {
    auto **ret = new double *[n];
    for (int i = 0; i < n; i++) {
        ret[i] = new double[n];
        for (int k = 0; k < n; k++) {
            ret[i][k] = 0;
        }
    }

    return ret;
}

// Data is all from one src
void postProcess::orderData(const Node *start, std::unordered_map<std::string, double> &edges_best,
                            std::unordered_map<std::string, std::tuple<IntervalPath, Path>> &path_best,
                            const std::tuple<IntervalPath, Path> &path) {
    auto[iPath, p] = path;
    if(iPath.intervals.empty()) return;
    std::string edgeHash = start->getName() + EdgeToStr(iPath) + p.getLastNode()->getName();
    edges_best.try_emplace(edgeHash, 0);
    path_best.try_emplace(edgeHash);
    if (edges_best[edgeHash] < iPath.getThrougput()) {
        edges_best[edgeHash] = iPath.getThrougput();
        path_best[edgeHash] = path;

    }
}




void postProcess::writeToFile(const std::filesystem::directory_entry &f, GraphDataStruct data, Node *start,
                              const std::vector<Node *> &destinations) {


    std::stringstream outputGeoData;

    outputGeoData << R"(../Outputs/)" << get_stem(f) << R"(/GeoData/)";
    std::filesystem::create_directories(outputGeoData.str());

    outputGeoData << start->getName() + ".txt";

    std::ofstream outGeoData(outputGeoData.str(), std::ios_base::out);

    for (auto &d_node: destinations) {
        std::stringstream outputFileData;

        outputFileData  << R"(../Outputs/)" << get_stem(f)  <<  R"(/Data/)";
        std::filesystem::create_directories(outputFileData.str());
        outputFileData << start->getName().c_str() << d_node->getName().c_str() << ".txt";
        std::ofstream outData(outputFileData.str(),std::ios_base::out);



        double sum = data.getBestForNode(start->getName(), d_node->getName(), outData) * DefValues::EntangledRate;

        printf("For the path between %s and %s avarage bitrate was %f sent bits: %f\n", start->getName().c_str(),
               d_node->getName().c_str(), sum / (3600 * 4), sum);
        outGeoData <<  d_node->getName().c_str() << "|" <<sum / (3600 * 4)<<std::endl;

    }


}


std::string postProcess::EdgeToStr(const IntervalPath &interval) {
    std::stringstream buff;
    buff << "|" << interval.intervals.begin()->start.getTime() << interval.intervals.begin()->end.getTime() << "|";

    return buff.str();
}

