//
// Created by egordm on 22-3-2018.
//

#include "gesture_detection_helper.h"
#include "../cvision/math.hpp"

using namespace cvision;
using namespace cv;


#define MIN_FINGER_DEPTH 20
#define MAX_FINGER_CENTER_ANGLE 90
#define MAX_FINGER_SPLIT_ANGLE 110

void drawHand(const cv::Mat &src, const std::vector<cv::Point> &contour) {
    // Convex shizzel
    std::vector<cv::Point> hull;
    cv::convexHull(contour, hull, false);
    std::vector<std::vector<cv::Point>> hulls = {hull};
    //cv::drawContours(src, hulls, 0, cv::Scalar(0, 255, 0));

    // Center
    auto moment = cv::moments(hull);
    cv::Point center(cvRound(moment.m10 / moment.m00), cvRound(moment.m01 / moment.m00));
    cv::circle(src, center, 4, cv::Scalar(255, 255, 0), -1);

    if (hull.size() <= 2) return;

    // Defects
    std::vector<int> hullIndexes;
    cv::convexHull(contour, hullIndexes);

    std::vector<cv::Vec4i> convexityDefects;
    cv::convexityDefects(contour, hullIndexes, convexityDefects);
    convexityDefects.erase(std::remove_if(convexityDefects.begin(), convexityDefects.end(),
                   [](const cv::Vec4i &v) { return v[3] / 256 < MIN_FINGER_DEPTH; }), convexityDefects.end());

    double center_radius = 99999;
    for(const auto &defect : convexityDefects) {
        const auto dist = sqrt(math::distance_sq(center, contour[defect[2]]));
        if(center_radius > dist) center_radius = dist;
    }
    cv::circle(src, center, static_cast<int>(center_radius), cv::Scalar(255, 255, 0), 1);

    auto defCount = static_cast<int>(convexityDefects.size());
    std::vector<cv::Point> candidates_p1;
    std::vector<cv::Point> candidates_p2;
    for (int i = 0; i < defCount; i++) {
        auto p1 = contour[convexityDefects[i][0]];
        auto p2 = contour[convexityDefects[i][1]];
        auto defect = contour[convexityDefects[i][2]];

        double angle_c = abs(math::rad_to_deg(math::inner_angle(p1, center, p2)));
        double angle_f = abs(math::rad_to_deg(math::inner_angle(p1, defect, p2)));
        if (angle_c >= MAX_FINGER_CENTER_ANGLE || angle_f > MAX_FINGER_SPLIT_ANGLE) continue;

        candidates_p1.push_back(p1);
        candidates_p2.push_back(p2);
    }

    std::vector<cv::Point> fingers;
    const int thresh = static_cast<const int>(pow(20, 2));
    for (const auto &p1 : candidates_p1) {
        bool handled = false;
        for (int i = (int) candidates_p2.size() - 1; i >= 0; --i) {
            if (math::distance_sq(p1, candidates_p2[i]) > thresh) continue;
            fingers.push_back((p1 + candidates_p2[i]) / 2);
            candidates_p2.erase(candidates_p2.begin() + i);
            handled = true;
            break;
        }
        if (!handled) fingers.push_back(p1);
    }
    fingers.insert(fingers.end(), candidates_p2.begin(), candidates_p2.end());

    for (const auto &finger : fingers) {
        cv::circle(src, finger, 5, cv::Scalar(0, 255, 0), 2);
        cv::line(src, center, finger, cv::Scalar(255, 0, 0), 1);
    }
}


cv::Mat HandDetectorHelper::draw(const cv::Mat &src, const cv::Mat &original) {
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::findContours(mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    // TODO: find some other way like proportion to image size to support multiple hands
    int largestContour = 0;
    for (int i = 1; i < contours.size(); i++) {
        if (cv::contourArea(contours[i]) > cv::contourArea(contours[largestContour])) largestContour = i;
    }

    if (!contours.empty()) {
        drawHand(src, contours[largestContour]);
    }

    return src;
}

