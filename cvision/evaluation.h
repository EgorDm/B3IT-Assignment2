//
// Created by egordm on 15-3-2018.
//

#ifndef B3ITASSIGNMENT2_EVALUATION_H
#define B3ITASSIGNMENT2_EVALUATION_H

#include <opencv2/opencv.hpp>

namespace cvision { namespace evaluation {
    /**
     * Calculated data from the confusion matrix
     */
    struct ConfusionMatrixResults {
        double relative_error, total_performace, bep, precision, recall, f1, specificity, generality;
    };

    /**
     * ...
     */
    struct ConfusionMatrix {
        int tp, tn, fp, fn;

        ConfusionMatrixResults evaluate();
    };

    // Print both structs
    std::ostream &operator<<(std::ostream &stream, ConfusionMatrixResults const &o);
    std::ostream &operator<<(std::ostream &stream, ConfusionMatrix const &o);

    /**
     * Create confusion matrix from the input data. Ground truth mask and the input masked by predicted mask
     * @param input
     * @param ground_truth
     * @param prediction
     * @return
     */
    ConfusionMatrix make_confsion_matrix(const cv::Mat &input, const cv::Mat &ground_truth, const cv::Mat &prediction);

    /**
     * Bayes theorem
     * @param pa
     * @param pb
     * @param marginal_pa
     * @return
     */
    float bayes_probability(float pa, float pb, float marginal_pa);
}};


#endif //B3ITASSIGNMENT2_EVALUATION_H
