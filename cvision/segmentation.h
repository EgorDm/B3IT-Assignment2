//
// Created by egordm on 15-3-2018.
//

#ifndef B3ITASSIGNMENT2_SEGMENTATION_H
#define B3ITASSIGNMENT2_SEGMENTATION_H


#include <opencv2/core/types.hpp>

namespace cvision { namespace algorithms { namespace segmentation {
    cv::Mat simple_segmentation(const cv::Mat &src, const cv::Scalar &low, const cv::Scalar &high, const int &ksize = 4,
                                const int &close_ksize = 4, const int &blur_ksize = 7);
}}};


#endif //B3ITASSIGNMENT2_SEGMENTATION_H
