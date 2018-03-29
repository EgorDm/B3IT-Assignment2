//
// Created by egordm on 22-3-2018.
//

#include "gesture_detection_helper.h"
#include "../cvision/math.hpp"
#include "../cvision/visualization.h"
#include "../cvision/hand_recognition.h"

using namespace cvision;
using namespace cv;


cv::Mat HandDetectorHelper::draw(const cv::Mat &src, const cv::Mat &original) {
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    // TODO: find some other way like proportion to image size to support multiple hands
    int largestContour = 0;
    for (int i = 1; i < contours.size(); i++) {
        if (cv::contourArea(contours[i]) > cv::contourArea(contours[largestContour])) largestContour = i;
    }

    if (!contours.empty()) {
        auto hand = processing::limb_recognition::hand::recognize_hand(contours[largestContour]);
        circle(src, hand.palm_center, (int)hand.palm_radius, Scalar(255, 255, 255), 2);
        circle(src, hand.enclosing_center, (int)hand.enclosing_radius, Scalar(255, 255, 200), 2);

        for(const auto &finger : hand.fingers) {
            circle(src, finger.tip, 2, Scalar(0, 0, 255), 2);
            line(src, finger.tip - finger.direction, finger.tip + finger.direction, Scalar(255, 0, 0), 2);
        }
    }

    return src;
}
