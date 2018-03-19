//
// Created by egordm on 19-3-2018.
//

#include "window_helpers.h"

std::vector<Statistic> CFMatrixHelper::get_statistics() {
    return {
            std::bind(&CFMatrixHelper::get_recall, this),
            std::bind(&CFMatrixHelper::get_precision, this),
            std::bind(&CFMatrixHelper::get_score, this),
            std::bind(&CFMatrixHelper::get_error, this)
    };
}

std::string CFMatrixHelper::get_recall() {
    std::stringstream ss;
    ss << "Recall: " << cf_results.recall;
    return ss.str();
}

std::string CFMatrixHelper::get_precision() {
    std::stringstream ss;
    ss << "Precision: " << cf_results.precision;
    return ss.str();
}

std::string CFMatrixHelper::get_score() {
    std::stringstream ss;
    ss << "F1 score: " << cf_results.f1;
    return ss.str();
}

std::string CFMatrixHelper::get_error() {
    std::stringstream ss;
    ss << "Relative error: " << cf_results.relative_error;
    return ss.str();
}

