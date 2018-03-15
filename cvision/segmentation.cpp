//
// Created by egordm on 15-3-2018.
//

#include "segmentation.h"
#include <opencv2/opencv.hpp>
#include "visualization.h"

using namespace cvision::algorithms;

cv::Mat
segmentation::simple_segmentation(const cv::Mat &src, const cv::Scalar &low, const cv::Scalar &high, const int &ksize,
                                  const int &close_ksize, const int &blur_ksize) {
    Mat prediction_mask;
    inRange(src, low, high, prediction_mask);

    // Morphological Processing
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(ksize, ksize));
    morphologyEx(prediction_mask, prediction_mask, MORPH_ERODE, kernel, Point(-1, -1), 2);
    morphologyEx(prediction_mask, prediction_mask, MORPH_DILATE, kernel, Point(-1, -1), 2);

    Mat close_element = getStructuringElement(MORPH_RECT, Size(close_ksize, close_ksize), cv::Point(2, 2));
    morphologyEx(prediction_mask, prediction_mask, MORPH_CLOSE, close_element, Point(-1, -1), 2);

    GaussianBlur(prediction_mask, prediction_mask, Size(blur_ksize, blur_ksize), 4);

    return prediction_mask;

}
