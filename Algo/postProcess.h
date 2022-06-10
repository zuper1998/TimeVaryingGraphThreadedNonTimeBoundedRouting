//
// Created by Narcano on 6/10/2022.
//

#ifndef TVGGRAPHCALC_POSTPROCESS_H
#define TVGGRAPHCALC_POSTPROCESS_H
#define RETURN_VAL std::tuple<std::unordered_map<std::string, double>,std::unordered_map<std::string, std::tuple<IntervalPath, Path>>>

#include <sstream>
#include <unordered_map>
#include <iostream>
#include <filesystem>
#include <vector>
#include <sstream>
#include <chrono>
#include <fstream>
#include "../Graph/IntervalPath.h"
#include "../Graph/Path.h"

class postProcess {
public:

    static void
    writeToFile(const std::filesystem::directory_entry &f, RETURN_VAL data, Node *start,
                const std::vector<Node *> &destinations);

    static std::string get_stem(const std::filesystem::path &p) { return (p.stem().string()); }

    static std::string EdgeToStr(const IntervalPath &interval);

    static void orderData(const Node *start, std::unordered_map<std::string, double> &edges_best,
                          std::unordered_map<std::string, std::tuple<IntervalPath, Path>> &path_best,
                          const std::tuple<IntervalPath, Path> &path);

    static std::string EdgeToStr(const VisibilityInterval &interval);

    static RETURN_VAL
    processData(std::vector<std::vector<std::tuple<IntervalPath, Path>>> const &data, Node *start,
                const std::vector<Node *> &destinations);
};


#endif //TVGGRAPHCALC_POSTPROCESS_H
