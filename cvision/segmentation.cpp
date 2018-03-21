//
// Created by egordm on 15-3-2018.
//

#include "segmentation.h"
#include <opencv2/opencv.hpp>
#include "visualization.h"
#include "evaluation.h"

using namespace cvision::algorithms;
using namespace cvision::evaluation;

cv::Mat
segmentation::simple_segmentation(const cv::Mat &src, const cv::Scalar &low, const cv::Scalar &high) {
    Mat prediction_mask;
    inRange(src, low, high, prediction_mask);
    return prediction_mask;

}

cv::Mat segmentation::complex_segmentation(const cv::Mat &src, const Histogram &target_histogram,
                                           const Histogram &environment_histogram, const float threshold,
                                           const float &positive_probability) {
    Mat tmp_src(src);
    Mat ret = Mat::zeros(tmp_src.size(), tmp_src.type());

    cv::Vec3b *read_row;
    cv::Vec3b *write_row;

    for (int j = 0; j < tmp_src.rows; ++j) {
        read_row = tmp_src.ptr<cv::Vec3b>(j);
        write_row = ret.ptr<cv::Vec3b>(j);

        for (int i = 0; i < tmp_src.cols; ++i) {
            HistColor color = {(double) read_row[i][0], (double) read_row[i][1], (double) read_row[i][2]};
            const float probability = bayes_probability(target_histogram.probability(color),
                                                        environment_histogram.probability(color), positive_probability);
            if (probability > threshold) {
                write_row[i][0] = 0xFF;
                write_row[i][1] = 0xFF;
                write_row[i][2] = 0xFF;
            }
        }
    }

    return ret;
}


cv::Mat
segmentation::clean_segmentation(const cv::Mat &src, const int &ksize, const int &close_ksize, const int &blur_ksize) {
    Mat ret;
    src.copyTo(ret);

    // Morphological Processing
    if (ksize > 4) {
        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(ksize, ksize));
        morphologyEx(src, ret, MORPH_ERODE, kernel, Point(-1, -1), 2);
        morphologyEx(ret, ret, MORPH_DILATE, kernel, Point(-1, -1), 2);
    }

    if (close_ksize > 3) {
        Mat close_element = getStructuringElement(MORPH_ELLIPSE, Size(close_ksize, close_ksize), cv::Point(2, 2));
        morphologyEx(ret, ret, MORPH_CLOSE, close_element, Point(-1, -1), 2);
    }

    if(blur_ksize > 0 && blur_ksize % 2 != 0) GaussianBlur(ret, ret, Size(blur_ksize, blur_ksize), 4);

    return ret;
}
