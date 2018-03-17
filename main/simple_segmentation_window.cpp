//
// Created by egordm on 17-3-2018.
//

#include "../cvision/segmentation.h"
#include "../cvision/evaluation.h"
#include "simple_segmentation_window.h"
#include "../cvision/visualization.h"
#include "../cvision/rainbows.h"

using namespace cvision::algorithms;
using namespace cvision::evaluation;

cv::Mat SimpleSegmentationWindow::draw() {
    Mat prefx;
    sample.input.copyTo(prefx);
    if (preblur > 0) {
        auto blur = (preblur % 2 == 0) ? preblur + 1 : preblur;
        GaussianBlur(prefx, prefx, Size(blur, blur), 4);
    }
    cvtColor(prefx, prefx, COLOR_BGR2HSV);

    auto color = cvision::processing::image::extract_dominant_color(*skin_histogram);
    Scalar low(std::fmin(0, color[0] - ((double)alpha / 2)), std::fmax(0, ((double)beta / 2)), std::fmax(0,  ((double)gamma / 2)));
    Scalar high(std::fmin(180, color[0] +  ((double)alpha / 2)), std::fmin(256, 256 -  ((double)beta / 2)), std::fmin(256, 256 -  ((double)gamma / 2)));

    auto mask = segmentation::simple_segmentation(prefx, low, high);
    cvtColor(prefx, prefx, COLOR_HSV2BGR);

    mask = segmentation::clean_segmentation(mask, ed_size, close_size, (postblur % 2 == 0) ? postblur + 1 : postblur);

    Mat result, postfx;
    sample.input.copyTo(result, mask);
    prefx.copyTo(postfx, mask);

    auto matrix = make_confsion_matrix(sample.input, sample.label, result);
    cf_results = matrix.evaluate();
    std::cout << matrix << cf_results << std::endl;

    return postfx;
}

void SimpleSegmentationWindow::init() {
    statistics.emplace_back(std::bind(&SimpleSegmentationWindow::get_recall, this));
    statistics.emplace_back(std::bind(&SimpleSegmentationWindow::get_precision, this));
    statistics.emplace_back(std::bind(&SimpleSegmentationWindow::get_score, this));
    statistics.emplace_back(std::bind(&SimpleSegmentationWindow::get_error, this));
    createTrackbar("Alpha", window_name, &alpha, 40, &SimpleSegmentationWindow::on_trackbar, this);
    createTrackbar("Beta", window_name, &alpha, 60, &SimpleSegmentationWindow::on_trackbar, this);
    createTrackbar("Gamma", window_name, &alpha, 60, &SimpleSegmentationWindow::on_trackbar, this);
    createTrackbar("Pre Blur", window_name, &preblur, 30, &SimpleSegmentationWindow::on_trackbar, this);
    createTrackbar("Erode Dilate Size", window_name, &ed_size, 30, &SimpleSegmentationWindow::on_trackbar, this);
    createTrackbar("Close Size", window_name, &close_size, 30, &SimpleSegmentationWindow::on_trackbar, this);
    createTrackbar("Post Blur", window_name, &postblur, 30, &SimpleSegmentationWindow::on_trackbar, this);
}

void SimpleSegmentationWindow::on_trackbar(int newValue, void *object) {
    ((SimpleSegmentationWindow *) object)->show();
}


std::string SimpleSegmentationWindow::get_recall() {
    std::stringstream ss;
    ss << "Recall: " << cf_results.recall;
    return ss.str();
}

std::string SimpleSegmentationWindow::get_precision() {
    std::stringstream ss;
    ss << "Precision: " << cf_results.precision;
    return ss.str();
}

std::string SimpleSegmentationWindow::get_score() {
    std::stringstream ss;
    ss << "F1 score: " << cf_results.f1;
    return ss.str();
}

std::string SimpleSegmentationWindow::get_error() {
    std::stringstream ss;
    ss << "Relative error: " << cf_results.relative_error;
    return ss.str();
}

SimpleSegmentationWindow
create_simple_seg_window(const std::string &dataset_name, const std::string &sample_name, const bool inverted,
                          const std::string &ext_in, const std::string &ext_lbl) {
    auto dataset = file::load_dataset(dataset_name, inverted,ext_in, ext_lbl);
    auto sample = file::load_sample(dataset_name, sample_name, inverted, ext_in, ext_lbl);

    Mat dataset_inputs[dataset.size()];
    Mat dataset_masks[dataset.size()];
    Mat dataset_masks_inv[dataset.size()];

    for (int i = 0; i < dataset.size(); ++i) {
        cvtColor(dataset[i].input, dataset_inputs[i], COLOR_BGR2HSV);
        dataset_masks[i] = dataset[i].label;
        bitwise_not(dataset_masks[i], dataset_masks_inv[i]);
    }

    const float ranges[3] = {180, 256, 256};

    auto positive_hst = image::extract_histogram(dataset_inputs, (uint) dataset.size(), ranges, dataset_masks, 128);
    auto marginal_positive_prob = image::probability_masked_pixels(dataset_masks, (uint) dataset.size());

    return SimpleSegmentationWindow(positive_hst, marginal_positive_prob, sample);
}
