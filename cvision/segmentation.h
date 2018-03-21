//
// Created by egordm on 15-3-2018.
//

#ifndef B3ITASSIGNMENT2_SEGMENTATION_H
#define B3ITASSIGNMENT2_SEGMENTATION_H


#include <opencv2/core/types.hpp>
#include "image_processing.h"

using namespace cvision::processing::image;

namespace cvision { namespace algorithms { namespace segmentation {
    /**
     * Split segement out everything in given color range
     * @param src image in same color space as the histogram
     * @param low
     * @param high
     * @return
     */
    cv::Mat simple_segmentation(const cv::Mat &src, const cv::Scalar &low, const cv::Scalar &high);

    /**
     * Use bayesian theorem to segement out pixels that have a higher probability to be in probability histogram.
     * @param src image in same color space as the histogram
     * @param target_histogram
     * @param environment_histogram
     * @param threshold
     * @param positive_probability
     * @return
     */
    cv::Mat complex_segmentation(const cv::Mat &src, const Histogram *target_histogram,
                                 const Histogram *environment_histogram, const float threshold,
                                 const float &positive_probability);

    /**
     * Apply some filters to clean outthe raw segemented mask
     * @param src
     * @param ksize
     * @param close_ksize
     * @param blur_ksize
     * @return
     */
    cv::Mat clean_segmentation(const cv::Mat &src, const int &ksize = 4, const int &close_ksize = 4,const int &blur_ksize = 7);
}}};


#endif //B3ITASSIGNMENT2_SEGMENTATION_H
