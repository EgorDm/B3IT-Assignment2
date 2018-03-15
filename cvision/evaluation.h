//
// Created by egordm on 15-3-2018.
//

#ifndef B3ITASSIGNMENT2_EVALUATION_H
#define B3ITASSIGNMENT2_EVALUATION_H

#include <opencv2/opencv.hpp>

namespace cvision { namespace evaluation {
    struct ConfusionMatrixResults {
        double relative_error, total_performace, bep, precision, recall, f1, specificity, generality;
    };

    struct ConfusionMatrix {
        int tp, tn, fp, fn;

        ConfusionMatrixResults evaluate();
    };

    std::ostream &operator<<(std::ostream &stream, ConfusionMatrixResults const &o);
    std::ostream &operator<<(std::ostream &stream, ConfusionMatrix const &o);

    ConfusionMatrix make_confsion_matrix(const cv::Mat &input, const cv::Mat &ground_truth, const cv::Mat &prediction);
}};


#endif //B3ITASSIGNMENT2_EVALUATION_H
