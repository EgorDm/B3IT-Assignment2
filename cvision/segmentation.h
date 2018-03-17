//
// Created by egordm on 15-3-2018.
//

#ifndef B3ITASSIGNMENT2_SEGMENTATION_H
#define B3ITASSIGNMENT2_SEGMENTATION_H


#include <opencv2/core/types.hpp>
#include "image_processing.h"

using namespace cvision::processing::image;

namespace cvision { namespace algorithms { namespace segmentation {
    cv::Mat simple_segmentation(const cv::Mat &src, const cv::Scalar &low, const cv::Scalar &high);

    cv::Mat complex_segmentation(const cv::Mat &src, const Histogram &target_histogram,
                                 const Histogram &environment_histogram, const float threshold,
                                 const float &positive_probability);

    cv::Mat clean_segmentation(const cv::Mat &src, const int &ksize = 4, const int &close_ksize = 4,const int &blur_ksize = 7);
}}};


#endif //B3ITASSIGNMENT2_SEGMENTATION_H
