//

#include "object_tracking.h"

using namespace cvision::processing;

//
// Created by egordm on 8-4-2018.
std::vector<std::tuple<int, int>>
tracking::match_points(std::vector<std::tuple<int, int, double>> &candidates, const double &thresh) {
    std::sort(candidates.begin(), candidates.end(),
              [](const std::tuple<int, int, double> &c1, const std::tuple<int, int, double> &c2) {
                  return std::get<2>(c1) < std::get<2>(c2);
              });

    std::vector<std::tuple<int, int>> ret;
    while (!candidates.empty()) {
        const auto candidate = candidates.front();
        if (std::get<2>(candidate) > pow(thresh, 2)) break;

        ret.emplace_back(std::get<0>(candidate), std::get<1>(candidate));
        candidates.erase(std::remove_if(candidates.begin(), candidates.end(),
                                        [&candidate](const std::tuple<int, int, double> &c1) {
                                            return std::get<0>(candidate) == std::get<0>(c1) ||
                                                   std::get<1>(candidate) == std::get<1>(c1);
                                        }), candidates.end());
    }
    return ret;
}