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
#define MAX_FINGER_CURVATURE 80
#define MAX_FINGER_CURVATURE_FALLBACK 60
#define EPSILON_DEPTH 20
#define EPSILON_DIST 20
#define K_VAL 20
#define K_VAL_FALLBACK 20
#define ROI_MULT 3.6

#define PALM_WEIGHT_POINT_EPSILON 20
#define PALM_WEIGHT_POINT_COUNT 3
#define PALM_WEIGHT_POINT_WEIGHT 0.05

PalmCenter palmCenter(const cv::Mat &src, const std::vector<cv::Point> &contour) {
    auto brect = boundingRect(contour);
    auto epsilon = 0.01 * arcLength(contour, true);
    std::vector<cv::Point> approx;
    cv::approxPolyDP(contour, approx, epsilon, true);

    std::vector<int> hullIndexes;
    cv::convexHull(contour, hullIndexes);
    std::vector<cv::Vec4i> convexityDefects;
    cv::convexityDefects(contour, hullIndexes, convexityDefects);
    std::sort(convexityDefects.begin(), convexityDefects.end(), [](Vec4i a, Vec4i b) { return a[3] > b[3]; });

    std::vector<Point> weight_points;
    for (int i = 0; i < PALM_WEIGHT_POINT_COUNT && i < convexityDefects.size(); ++i) {
        if (convexityDefects[i][3] / 256 < PALM_WEIGHT_POINT_EPSILON) break;
        weight_points.push_back(contour[convexityDefects[i][2]]);
    }

    int max_dx = brect.width / 2, max_dy = brect.height / 2;
    int x = brect.x + max_dx, y = brect.y + max_dy;
    int step = 4;

    Point min_point;
    double max_dist = 0;
    double score = 0;
    int iterations = 0;
    for (int dx = 0; dx < max_dx; dx += step) {
        for (int dy = 0; dy < max_dy; dy += step) {
            for (int px = -1; px <= 1; px += 2) {
                for (int py = -1; py <= 1; py += 2) {
                    ++iterations;
                    Point pp(x + dx * px, y + dy * py);
                    double min_dist = pointPolygonTest(approx, pp, true);
                    double point_score = min_dist;
                    for (const auto &weight_point : weight_points)
                        point_score -=
                                sqrt(math::distance_sq(weight_point, pp)) *
                                PALM_WEIGHT_POINT_WEIGHT;

                    //if (min_dist < max_dist) continue;
                    if (point_score < score) continue;
                    min_point = pp;
                    max_dist = min_dist;
                    score = point_score;
                    max_dx = static_cast<int>(brect.width / 2 - max_dist);
                    max_dy = static_cast<int>(brect.height / 2 - max_dist);
                }
            }
        }
    }

    return {min_point, max_dist};
}

int curve_around_array(const int size, const int pos) {
    if (pos < 0) return size - 1 + pos;
    if (pos >= size) return pos - size;
    return pos;
}

std::tuple<int, double>
kCurvaturePeak(const std::vector<cv::Point> &contour, const int pos, const int range, const int k) {
    if (contour.size() < k + 1) return {pos, 9999};

    int min_curv_i = pos;
    double min_curv = 9999;
    double curv;
    for (int i = pos - range; i < pos + range && i < contour.size(); ++i) {
        int p1 = curve_around_array((int) contour.size(), i - k);
        int p3 = curve_around_array((int) contour.size(), i + k);

        curv = abs(math::inner_angle(contour[p1], contour[i], contour[p3]));
        if (curv >= min_curv) continue;
        if (contour[p1] == contour[p3] || contour[p3] == contour[i] || contour[p1] == contour[i]) continue;

        min_curv = curv;
        min_curv_i = i;
    }

    return {min_curv_i, min_curv};
}

int largest_contour(const std::vector<std::vector<Point>> &contours) {
    M_Assert(!contours.empty(), "Contours should not be empty");
    int ret = 0;
    for (int i = 1; i < contours.size(); ++i) {
        if (cv::contourArea(contours[i]) > cv::contourArea(contours[ret])) ret = i;
    }
    return ret;
}

void drawHand(const cv::Mat &src, const cv::Mat &mask, std::vector<cv::Point> contour) {
    // Center
    auto palm = palmCenter(src, contour);
    cv::circle(src, palm.center, (int) palm.radius, cv::Scalar(255, 255, 0), 2);
    auto roi = palm.radius * ROI_MULT;

    //TODO: cull the region of interest to the begin of the palm from the cut out area to remove the arm
    //TODO: prefer a palm circle closer to deepest defects

    std::vector<Point> arm_border;
    auto roi_sq = pow(roi, 2);
    for (int i = 0; i < contour.size(); ++i) {
        if (math::distance_sq(contour[i], palm.center) < roi_sq) continue;

        auto p1 = contour[curve_around_array((int) contour.size(), i - 1)];
        auto p2 = contour[curve_around_array((int) contour.size(), i + 1)];
        if (math::distance_sq(p1, palm.center) > roi_sq && math::distance_sq(p2, palm.center) > roi_sq) continue;
        arm_border.push_back(contour[i]);
    }
    Point arm_border_center;
    for (const auto &p : arm_border) arm_border_center += p;
    arm_border_center /= (double) arm_border.size();


    Point2f enclosing_center;
    float enclosing_radius;
    if(!arm_border.empty()) {
        // Project the line twice as far to pick the pick the points after the line
        auto arm_cut_center = palm.center + math::normalize(arm_border_center) * palm.radius * 2;
        auto arm_cut_line_p1 = math::rotate_point(arm_cut_center, palm.center, M_PI / 2);
        auto arm_cut_line_p2 = arm_cut_center - (arm_cut_line_p1 - arm_cut_center);
        line(src, arm_cut_line_p2, arm_cut_line_p1, cv::Scalar(255, 0, 255), 2);
        circle(src, palm.center, (int) roi, cv::Scalar(255, 200, 200), 2);

        std::vector<Point> roi_contour;
        for (const auto &p : contour) {
            if (math::distance_sq(p, palm.center) < math::dist_line_sq(p, arm_cut_line_p1, arm_cut_line_p2))
                roi_contour.push_back(p);
        }
        std::vector<std::vector<Point>> roi_contourz;
        roi_contourz.push_back(roi_contour);
        drawContours(src, roi_contourz, 0, Scalar(255, 0, 255), 2);
        minEnclosingCircle(roi_contour, enclosing_center, enclosing_radius);
    } else {
        minEnclosingCircle(contour, enclosing_center, enclosing_radius);
    }

    cv::circle(src, enclosing_center, (int) enclosing_radius, cv::Scalar(255, 255, 255), 2);

    auto palm_to_hand_proportion = static_cast<float>(enclosing_radius / palm.radius);

    // Convex shizzel
    std::vector<cv::Point> hull;
    cv::convexHull(contour, hull, false);
    std::vector<std::vector<cv::Point>> hulls = {hull};
    std::vector<std::vector<cv::Point>> contours = {contour};
    cv::drawContours(src, hulls, 0, cv::Scalar(0, 255, 0));
    cv::drawContours(src, contours, 0, cv::Scalar(0, 0, 255));

    if (hull.size() <= 2) return;

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

        auto curv = math::rad_to_deg(std::get<1>(kCurvaturePeak(contour, defect[0], 15, K_VAL)));
        if (curv < MAX_FINGER_CURVATURE) candidates.push_back(defect[0]);

        curv = math::rad_to_deg(std::get<1>(kCurvaturePeak(contour, defect[1], 15, K_VAL)));
        if (curv < MAX_FINGER_CURVATURE) candidates.push_back(defect[1]);
    }

    if (candidates.empty()) {
        for (const auto &defect : convexityDefects) {
            if (defect[3] / 256 < EPSILON_DEPTH) continue;

            auto kpeak = kCurvaturePeak(contour, defect[0], 15, K_VAL_FALLBACK);
            auto angle = math::rad_to_deg(std::get<1>(kpeak));
            if (angle < MAX_FINGER_CURVATURE_FALLBACK) candidates.push_back(std::get<0>(kpeak));

            kpeak = kCurvaturePeak(contour, defect[1], 15, K_VAL_FALLBACK);
            angle = math::rad_to_deg(std::get<1>(kpeak));
            if (angle < MAX_FINGER_CURVATURE_FALLBACK) candidates.push_back(std::get<0>(kpeak));
        }
    }

    for (const auto &point : candidates) {
        if (math::distance_sq(contour[point], palm.center) > pow(roi, 2)) continue;
        auto kpeak = kCurvaturePeak(contour, point, 15, K_VAL);
        auto kpos = std::get<0>(kpeak);
        auto p1 = contour[curve_around_array((int) contour.size(), kpos - K_VAL)];
        auto p3 = contour[curve_around_array((int) contour.size(), kpos + K_VAL)];

        auto cf = (p1 + p3) / 2;
        line(src, cf, contour[kpos], Scalar(255, 255, 255), 2);
        circle(src, contour[kpos], 2, Scalar(255, 255, 255), 2);
        circle(src, p1, 2, Scalar(255, 255, 0), 2);
        circle(src, p3, 2, Scalar(255, 255, 0), 2);
    }

    /* if(palm_to_hand_proportion > ROI_MULT*0.66) {
         std::cout << "Palm is open" << std::endl;
     } else if(palm_to_hand_proportion > ROI_MULT*0.54) {
         std::cout << "Palm is half open" << std::endl;
     } else {
         std::cout << "Palm is closed" << std::endl;
     }
     std::cout << "Palm to Hand proportion: " << palm_to_hand_proportion << std::endl;*/
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

