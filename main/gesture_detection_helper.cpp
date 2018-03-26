//
// Created by egordm on 22-3-2018.
//

#include <c++/7.3.0/chrono>
#include "gesture_detection_helper.h"
#include "../cvision/math.hpp"
#include "../cvision/visualization.h"

using namespace cvision;
using namespace cv;


#define MAX_FINGER_DEPTH_ANGLE 90
#define MIN_FINGER_CURVATURE 80
#define EPSILON_DEPTH 20
#define K_VAL 20
#define ROI_MULT 3.6

PalmCenter palmCenter(const cv::Mat &src, const std::vector<cv::Point> &contour) {
    auto brect = boundingRect(contour);
    auto epsilon = 0.01 * arcLength(contour, true);
    std::vector<cv::Point> approx;
    cv::approxPolyDP(contour, approx, epsilon, true);

    int max_dx = brect.width / 2, max_dy = brect.height / 2;
    int x = brect.x + max_dx, y = brect.y + max_dy;
    int step = 4;
    Point min_point;
    double max_dist = 0;
    int iterations = 0;
    for (int dx = 0; dx < max_dx; dx += step) {
        for (int dy = 0; dy < max_dy; dy += step) {
            for (int px = -1; px <= 1; px += 2) {
                for (int py = -1; py <= 1; py += 2) {
                    ++iterations;
                    Point pp(x + dx * px, y + dy * py);
                    double min_dist = pointPolygonTest(approx, pp, true);
                    if (min_dist < max_dist) continue;
                    min_point = pp;
                    max_dist = min_dist;
                    max_dx = static_cast<int>(brect.width / 2 - max_dist);
                    max_dy = static_cast<int>(brect.height / 2 - max_dist);
                }
            }
        }
    }

    return {min_point, max_dist};
}

int curve_around_array(const int size, const int pos) {
    if(pos < 0) return size - 1 + pos;
    if(pos >= size) return pos - size;
    return pos;
}

std::tuple<int, double>
kCurvaturePeak(const std::vector<cv::Point> &contour, const int pos, const int range, const int k) {
    if (contour.size() < k + 1) return {pos, 9999};

    int min_curv_i = pos;
    double min_curv = 9999;
    double curv;
    for (int i = pos - range; i < pos + range && i < contour.size(); ++i) {
        int p1 = curve_around_array((int)contour.size(), i - k);
        int p3 = curve_around_array((int)contour.size(), i + k);

        curv = abs(math::inner_angle(contour[p1], contour[pos], contour[p3]));
        if (curv >= min_curv) continue;

        min_curv = curv;
        min_curv_i = pos;
    }

    return {min_curv_i, min_curv};
}

void drawHand(const cv::Mat &src, const cv::Mat &mask, std::vector<cv::Point> contour) {
    // Center
    auto palm = palmCenter(src, contour);
    cv::circle(src, palm.center, (int) palm.radius, cv::Scalar(255, 255, 0), 2);
    auto roi = palm.radius * ROI_MULT;

    // Convex shizzel
    std::vector<cv::Point> hull;
    cv::convexHull(contour, hull, false);
    std::vector<std::vector<cv::Point>> hulls = {hull};
    std::vector<std::vector<cv::Point>> contours = {contour};
    cv::drawContours(src, hulls, 0, cv::Scalar(0, 255, 0));
    cv::drawContours(src, contours, 0, cv::Scalar(0, 0, 255));

    if (hull.size() <= 2) return;

    Point2f enclosing_center;
    float enclosing_radius;
    minEnclosingCircle(hull, enclosing_center, enclosing_radius);

    // Defects
    std::vector<int> hullIndexes;
    cv::convexHull(contour, hullIndexes);

    std::vector<cv::Vec4i> convexityDefects;
    cv::convexityDefects(contour, hullIndexes, convexityDefects);

    std::vector<int> candidates;
    for (const auto &defect : convexityDefects) {
        auto depth = defect[3] / 256;
        if (depth < palm.radius || depth > enclosing_radius) continue;
        auto angle = math::rad_to_deg(
                abs(math::inner_angle(contour[defect[0]], contour[defect[2]], contour[defect[1]])));
        if (angle > MAX_FINGER_DEPTH_ANGLE) continue;
        candidates.push_back(defect[0]);
        candidates.push_back(defect[1]);
    }

    if (candidates.empty()) {
        for (const auto &defect : convexityDefects) {
            if (defect[3] / 256 < EPSILON_DEPTH) continue;

            auto kpeak = kCurvaturePeak(contour, defect[0], 10, K_VAL);
            auto angle = math::rad_to_deg(std::get<1>(kpeak));
            if (angle < MIN_FINGER_CURVATURE) {
                candidates.push_back(std::get<0>(kpeak));
            }

            kpeak = kCurvaturePeak(contour, defect[1], 10, K_VAL);
            angle = math::rad_to_deg(std::get<1>(kpeak));
            if (angle < MIN_FINGER_CURVATURE) {
                candidates.push_back(std::get<0>(kpeak));
            }
        }
    }

    for (const auto &point : candidates) {
        if(math::distance_sq(contour[point], palm.center) > pow(roi, 2)) continue;
        auto kpeak = kCurvaturePeak(contour, point, 10, K_VAL);
        auto kpos = std::get<0>(kpeak);
        auto p1 = contour[curve_around_array((int)contour.size(), kpos - K_VAL)];
        auto p3 = contour[curve_around_array((int)contour.size(), kpos + K_VAL)];

        auto cf = (p1 + p3) / 2;
        line(src, cf, contour[kpos], Scalar(255, 255, 255), 2);
        circle(src, contour[kpos], 2, Scalar(255, 255, 255), 2);
        circle(src, p1, 2, Scalar(255, 255, 0), 2);
        circle(src,p3, 2, Scalar(255, 255, 0), 2);
    }
}


cv::Mat HandDetectorHelper::draw(const cv::Mat &src, const cv::Mat &original) {
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    // TODO: find some other way like proportion to image size to support multiple hands
    int largestContour = 0;
    for (int i = 1; i < contours.size(); i++) {
        if (cv::contourArea(contours[i]) > cv::contourArea(contours[largestContour])) largestContour = i;
    }

    if (!contours.empty()) {
        drawHand(src, mask, contours[largestContour]);
    }

    return src;
}

