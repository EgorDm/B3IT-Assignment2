//
// Created by egordm on 19-3-18.
//

#include "segmentation_helper.h"
#include "../cvision/segmentation.h"

using namespace cvision::algorithms;
using namespace cvision::evaluation;


Mat ComplexSegmentationHelper::draw(const cv::Mat &src) {
    frame = src;
    float threshold_a = ((float) threshold) / 100;

    Mat prefx;
    src.copyTo(prefx);
    if (preblur > 0) {
        auto blur = (preblur % 2 == 0) ? preblur + 1 : preblur;
        GaussianBlur(prefx, prefx, Size(blur, blur), 4);
    }

    auto mweight = (float) (0.5 + ((positive_marginal_prob - 0.5) * (((float) marginal_weight) / 100)));
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
    double color_array[3] = {(double)colour[0], (double)colour[1], (double)colour[2]};

    pskin = positive_hst->probability(color_array);
    penv = negative_hst->probability(color_array);
    pbayes = bayes_probability(pskin, penv, positive_marginal_prob);

    return true;
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

std::vector<Trackbar> SimpleSegmentationHelper::get_trackbars() {
    return {
            Trackbar("Alpha", &alpha, 40),
            Trackbar("Beta", &beta, 60),
            Trackbar("Gamma", &gamma, 60),
            Trackbar("Pre Blur", &preblur, 30),
            Trackbar("Erode Dilate Size", &ed_size, 30),
            Trackbar("Close Size", &close_size, 30),
            Trackbar("Post Blur", &postblur, 30)
    };
}

Mat SimpleSegmentationHelper::draw(const cv::Mat &src) {
    Mat prefx;
    src.copyTo(prefx);
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

    Mat postfx;
    prefx.copyTo(postfx, mask);

    return postfx;
}
