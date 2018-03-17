//
// Created by egordm on 17-3-2018.
//

#include "complex_segmentation_window.h"
#include "../cvision/segmentation.h"
#include "../cvision/evaluation.h"

using namespace cvision::algorithms;
using namespace cvision::evaluation;

cv::Mat ComplexSegmentationWindow::draw() {
    float threshold_a = ((float) threshold) / 100;

    Mat prefx;
    sample.input.copyTo(prefx);
    if (preblur > 0) {
        auto blur = (preblur % 2 == 0) ? preblur + 1 : preblur;
        GaussianBlur(prefx, prefx, Size(blur, blur), 4);
    }

    auto mweight = (float)(0.5 + ((marginal_positive_probability - 0.5) * (((float) marginal_weight) / 100)));
    auto mask = segmentation::complex_segmentation(prefx, skin_histogram, env_histogram, threshold_a, mweight);
    mask = segmentation::clean_segmentation(mask, ed_size, close_size, (postblur % 2 == 0) ? postblur + 1 : postblur);

    Mat result, postfx;
    sample.input.copyTo(result, mask);
    prefx.copyTo(postfx, mask);

    auto matrix = make_confsion_matrix(sample.input, sample.label, result);
    cf_results = matrix.evaluate();
    std::cout << matrix << cf_results << std::endl;

    return postfx;
}

void ComplexSegmentationWindow::init() {
    statistics.emplace_back(std::bind(&ComplexSegmentationWindow::get_recall, this));
    statistics.emplace_back(std::bind(&ComplexSegmentationWindow::get_precision, this));
    statistics.emplace_back(std::bind(&ComplexSegmentationWindow::get_score, this));
    statistics.emplace_back(std::bind(&ComplexSegmentationWindow::get_error, this));
    statistics.emplace_back(std::bind(&ComplexSegmentationWindow::get_probability, this));
    statistics.emplace_back(std::bind(&ComplexSegmentationWindow::get_probability2, this));
    createTrackbar("Threshold", window_name, &threshold, 100, &ComplexSegmentationWindow::on_trackbar, this);
    createTrackbar("Pre Blur", window_name, &preblur, 30, &ComplexSegmentationWindow::on_trackbar, this);
    createTrackbar("Erode Dilate Size", window_name, &ed_size, 30, &ComplexSegmentationWindow::on_trackbar, this);
    createTrackbar("Close Size", window_name, &close_size, 30, &ComplexSegmentationWindow::on_trackbar, this);
    createTrackbar("Post Blur", window_name, &postblur, 30, &ComplexSegmentationWindow::on_trackbar, this);
    createTrackbar("Marginal weight", window_name, &marginal_weight, 100, &ComplexSegmentationWindow::on_trackbar, this);
}

void ComplexSegmentationWindow::on_trackbar(int newValue, void *object) {
    ((ComplexSegmentationWindow *) object)->show();
}


void ComplexSegmentationWindow::on_click(int x, int y) {
    colour = sample.input.at<Vec3b>(Point(x, y));
    double color_array[3] = {colour[0], colour[1], colour[2]};

    pskin = skin_histogram.probability(color_array);
    penv = env_histogram.probability(color_array);
    pbayes = bayes_probability(pskin, penv, marginal_positive_probability);
    /*std::cout << "Pixel " << color_array[2] << "," << color_array[1] << "," << color_array[0] << std::endl
              << "\tProbability skin: " << pskin << std::endl
              << "\tProbability no skin: " << penv << std::endl
              << "\tProbability marginal: " << marginal_positive_probability << std::endl
              << "\tProbability bayes: " << pbayes << std::endl;*/
    this->show();
}

std::string ComplexSegmentationWindow::get_recall() {
    std::stringstream ss;
    ss << "Recall: " << cf_results.recall;
    return ss.str();
}

std::string ComplexSegmentationWindow::get_precision() {
    std::stringstream ss;
    ss << "Precision: " << cf_results.precision;
    return ss.str();
}

std::string ComplexSegmentationWindow::get_score() {
    std::stringstream ss;
    ss << "F1 score: " << cf_results.f1;
    return ss.str();
}

std::string ComplexSegmentationWindow::get_error() {
    std::stringstream ss;
    ss << "Relative error: " << cf_results.relative_error;
    return ss.str();
}

std::string ComplexSegmentationWindow::get_probability() {
    std::stringstream ss;
    ss << "C " << (int) colour[2] << "," << (int) colour[1] << "," << (int) colour[0]
       << "| Pt: " << pskin << "| Pn: " << penv;
    return ss.str();
}

std::string ComplexSegmentationWindow::get_probability2() {
    std::stringstream ss;
    ss  << "Pm: " << marginal_positive_probability << "| Pb: " << pbayes;
    return ss.str();
}
