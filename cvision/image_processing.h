//
// Created by egordm on 14-3-2018.
//

#ifndef B3ITASSIGNMENT2_IMAGE_PROCESSING_H
#define B3ITASSIGNMENT2_IMAGE_PROCESSING_H

#include <opencv2/opencv.hpp>
#include "defines.h"

using namespace cv;

namespace cvision { namespace processing { namespace image {
    Histogram extract_hsv_histogram(const Mat &src, const Mat &mask = Mat(), const int binSize = 128);

    Scalar extract_dominant_color(const std::vector<Mat> &histogram);
}}}

#endif //B3ITASSIGNMENT2_IMAGE_PROCESSING_H
