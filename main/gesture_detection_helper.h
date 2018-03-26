//
// Created by egordm on 22-3-2018.
//

#ifndef B3ITASSIGNMENT2_GESTURE_DETECTION_HELPER_H
#define B3ITASSIGNMENT2_GESTURE_DETECTION_HELPER_H


#include "window.h"

struct PalmCenter {
    cv::Point center;
    double radius;
};

class HandDetectorHelper : public WindowHelper {
private:
    const cv::Mat &mask;
public:
    explicit HandDetectorHelper(const cv::Mat &mask) : mask(mask) {}

    cv::Mat draw(const cv::Mat &src, const cv::Mat &original) override;
};


#endif //B3ITASSIGNMENT2_GESTURE_DETECTION_HELPER_H
