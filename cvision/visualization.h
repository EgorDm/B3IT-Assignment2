//
// Created by egordm on 14-3-2018.
//

#ifndef B3ITASSIGNMENT2_VISUALIZATION_H
#define B3ITASSIGNMENT2_VISUALIZATION_H

#include <opencv2/opencv.hpp>
#include "defines.h"

#define HISTOGRAM_WINDOW "Histogram Window"
#define IMAGE_WINDOW "Image Window"

using namespace cv;

namespace cvision { namespace visualization {
    void draw_hsv_histogram(const Histogram &histogram, const int binSize = 128, const int width = 512,
                            const int height = 512);

    void show_image(const Mat &image, const std::string &title = IMAGE_WINDOW);
}};


#endif //B3ITASSIGNMENT2_VISUALIZATION_H
