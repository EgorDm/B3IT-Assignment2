//
// Created by egordm on 19-3-18.
//

#include "segmentation_helper.h"
#include "../cvision/segmentation.h"

using namespace cvision::algorithms;
using namespace cvision::evaluation;


Mat ComplexSegmentationHelper::draw(const cv::Mat &src, const cv::Mat &original) {
    frame = src;
    float threshold_a = ((float) threshold) / 100;

    auto mweight = (float) (0.5 + ((positive_marginal_prob - 0.5) * (((float) marginal_weight) / 100)));
    return segmentation::complex_segmentation(src, *positive_hst, *negative_hst, threshold_a, mweight);
}

std::vector<Statistic> ComplexSegmentationHelper::get_statistics() {
    return {
            std::bind(&ComplexSegmentationHelper::get_probability, this),
            std::bind(&ComplexSegmentationHelper::get_probability2, this)
    };
}

std::vector<Trackbar> ComplexSegmentationHelper::get_trackbars() {
    return { Trackbar("Threshold", &threshold, 100)};
}

bool ComplexSegmentationHelper::on_click(int x, int y, bool rb) {
    if(rb) return false;
    colour = frame.at<Vec3b>(Point(x, y));
    HistColor color_array = {(double) colour[0], (double) colour[1], (double) colour[2]};

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
    };
}

Mat SimpleSegmentationHelper::draw(const cv::Mat &src, const cv::Mat &original) {
    Mat prefx;
    cvtColor(src, prefx, COLOR_BGR2HSV);

    auto color = cvision::processing::image::extract_dominant_color(*skin_histogram);
    Scalar low(std::fmin(0, color[0] - ((double) alpha / 2)), std::fmax(0, ((double) beta / 2)),
               std::fmax(0, ((double) gamma / 2)));
    Scalar high(std::fmin(180, color[0] + ((double) alpha / 2)), std::fmin(256, 256 - ((double) beta / 2)),
                std::fmin(256, 256 - ((double) gamma / 2)));

    return segmentation::simple_segmentation(prefx, low, high);
}

std::vector<Trackbar> SegmentationPatcher::get_trackbars() {
    return {
            Trackbar("Erode Dilate Size", &ed_size, 30),
            Trackbar("Close Size", &close_size, 30),
            Trackbar("Post Blur", &postblur, 30)
    };
}

Mat SegmentationPatcher::draw(const cv::Mat &src, const cv::Mat &original) {
    Mat mask = segmentation::clean_segmentation(src, ed_size, close_size, (postblur % 2 == 0) ? postblur + 1 : postblur);
    Mat ret;
    original.copyTo(ret, mask);

    return ret;
}

bool HistogramCorrector::on_click(int x, int y, bool rb) {
    std::vector<Vec3d> colors;
    float dw = (float)weight / 1000;
    auto boundl = static_cast<int>(-std::floor(radius / 2));
    auto boundr = static_cast<int>(std::ceil((float)radius / 2));

    Vec3b colour = frame.at<Vec3b>(Point(x, y));
    for(int ix = boundl; ix <= boundr; ++ix) {
        for(int iy = boundl; iy <=boundr; ++iy) {
            auto color = frame.at<Vec3b>(Point(x + ix, y + iy));
            if(use_hsv) {
                Mat space(1, 1, CV_8UC3, color);
                cvtColor(space, space, COLOR_BGR2HSV);
                color = space.at<Vec3b>(Point(0, 0));
            }
            colors.emplace_back((double) color[0], (double) color[1], (double) color[2]);
        }
    }

    if(positive_histogram != nullptr) {
        for(const auto &color : colors) {
            auto prob_before = positive_histogram->probability(color);
            positive_histogram->update_probability(color, rb ? dw : -dw);
        }
        positive_histogram->normalize();
    }

    if(negative_histogram != nullptr) {
        for(const auto &color : colors)
            negative_histogram->update_probability(color, rb ? -dw : dw);
        negative_histogram->normalize();
    }

    return true;
}

Mat HistogramCorrector::draw(const cv::Mat &src, const cv::Mat &original) {
    frame = original;
    return WindowHelper::draw(src, original);
}

std::vector<Trackbar> HistogramCorrector::get_trackbars() {
    return {
            Trackbar("Weight Radius", &radius, 20),
            Trackbar("Weight increase", &weight, 500),
    };
}
