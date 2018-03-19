//
// Created by egordm on 19-3-18.
//

#include "segmentation_helper.h"
#include "../cvision/segmentation.h"

using namespace cvision::algorithms;
using namespace cvision::evaluation;

std::vector<Statistic> CFMatrixHelper::get_statistics() {
    return {
            std::bind(&CFMatrixHelper::get_recall, this),
            std::bind(&CFMatrixHelper::get_precision, this),
            std::bind(&CFMatrixHelper::get_score, this),
            std::bind(&CFMatrixHelper::get_error, this)
    };
}

Mat ComplexSegmentationHelper::draw(const cv::Mat &src) {
    frame = src;
    float threshold_a = ((float) threshold) / 100;

    Mat prefx;
    src.copyTo(prefx);
    if (preblur > 0) {
        auto blur = (preblur % 2 == 0) ? preblur + 1 : preblur;
        GaussianBlur(prefx, prefx, Size(blur, blur), 4);
    }

    auto mweight = (float)(0.5 + ((positive_marginal_prob - 0.5) * (((float) marginal_weight) / 100)));
    auto mask = segmentation::complex_segmentation(prefx, *positive_hst, *negative_hst, threshold_a, mweight);
    mask = segmentation::clean_segmentation(mask, ed_size, close_size, (postblur % 2 == 0) ? postblur + 1 : postblur);

    Mat result, postfx;
    src.copyTo(result, mask);
    prefx.copyTo(postfx, mask);

    return postfx;
}


std::vector<Statistic> ComplexSegmentationHelper::get_statistics() {
    return {
            std::bind(&ComplexSegmentationHelper::get_probability, this),
            std::bind(&ComplexSegmentationHelper::get_probability2, this)
    };
}

std::vector<Trackbar> ComplexSegmentationHelper::get_trackbars() {
    return {
            Trackbar("Threshold", &threshold, 100),
            Trackbar("Pre Blur", &preblur, 30),
            Trackbar("Erode Dilate Size", &ed_size, 30),
            Trackbar("Close Size", &close_size, 30),
            Trackbar("Post Blur", &postblur, 30),
            Trackbar("Marginal weight", &marginal_weight, 100)
    };
}

bool ComplexSegmentationHelper::on_click(int x, int y) {
    colour = frame.at<Vec3b>(Point(x, y));
    double color_array[3] = {colour[0], colour[1], colour[2]};

    pskin = positive_hst->probability(color_array);
    penv = negative_hst->probability(color_array);
    pbayes = bayes_probability(pskin, penv, positive_marginal_prob);

    return true;
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

std::string ComplexSegmentationHelper::get_probability() {
    std::stringstream ss;
    ss << "C " << (int) colour[2] << "," << (int) colour[1] << "," << (int) colour[0]
       << "| Pt: " << pskin << "| Pn: " << penv;
    return ss.str();
}

std::string ComplexSegmentationHelper::get_probability2() {
    std::stringstream ss;
    ss << "Pm: " << positive_marginal_prob << "| Pb: " << pbayes;
    return ss.str();
}
