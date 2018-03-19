//
// Created by egordm on 19-3-2018.
//

#ifndef B3ITASSIGNMENT2_WINDOW_HELPERS_H
#define B3ITASSIGNMENT2_WINDOW_HELPERS_H

#include "window.h"
#include "../cvision/evaluation.h"

class CFMatrixHelper : public WindowHelper {
public:
    cvision::evaluation::ConfusionMatrixResults cf_results{};

    std::string get_recall();

    std::string get_precision();

    std::string get_score();

    std::string get_error();

    std::vector<Statistic> get_statistics() override;
};


#endif //B3ITASSIGNMENT2_WINDOW_HELPERS_H
