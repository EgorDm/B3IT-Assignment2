//
// Created by egordm on 15-3-2018.
//

#ifndef B3ITASSIGNMENT2_RAINBOWS_H
#define B3ITASSIGNMENT2_RAINBOWS_H

#include <opencv2/opencv.hpp>

namespace cvision { namespace rainbows {
    void translate_coolors(const cv::Scalar &color) {
        std::cout << "H: " << (color[0] * 2)
                  << " S: " << ((color[1] / 255) * 100)
                  << " V: " << ((color[2] / 255) * 100) << std::endl;
    }
}};


#endif //B3ITASSIGNMENT2_RAINBOWS_H
