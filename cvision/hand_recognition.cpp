//
// Created by egordm on 29-3-2018.
//

#include "hand_recognition.h"
#include "math.hpp"
#include "utils.hpp"
#include "geometry.hpp"

using namespace cvision::processing::limb_recognition;
using namespace cvision::processing::limb_recognition::hand;
using namespace cvision;

Hand hand::recognize_hand(const std::vector<cv::Point> &contour) {
    // Draw make convex polygon
    std::vector<cv::Point> hull;
    std::vector<int> hullIndexes;
    cv::convexHull(contour, hullIndexes);
    cv::convexHull(contour, hull);
    // Find convex polygon defects
    std::vector<cv::Vec4i> defects;
    cv::convexityDefects(contour, hullIndexes, defects);
    std::sort(defects.begin(), defects.end(), [](cv::Vec4i a, cv::Vec4i b) { return a[3] > b[3]; });

    Hand hand;
    // Find palm circle
    auto palm = find_palm(contour, defects);
    hand.palm_center = std::get<0>(palm);
    hand.palm_radius = std::get<1>(palm);
    // Find circle enclosing hand* aka. Frathest fingertip position
    auto roi = hand.palm_radius * HAND_ROI_MULT;
    auto enc_circle = find_enclosing_circle(contour, hand, roi);
    hand.enclosing_center = std::get<0>(enc_circle);
    hand.enclosing_radius = std::get<1>(enc_circle);
    hand.palm_to_hand_ratio = hand.enclosing_radius / hand.palm_radius;

    // Find the fingers
    auto candidates = find_fingers(contour, defects, hand);
    if (candidates.empty()) candidates = find_fingers_fallback(contour, defects);
    candidates.erase(std::remove_if(candidates.begin(), candidates.end(), [&](const CandidateFinger &candidate) {
        return math::distance_sq(contour[std::get<0>(candidate)], hand.palm_center) > pow(roi, 2);
    }), candidates.end());
    candidates.erase(std::unique(candidates.begin(), candidates.end(),
                                 [&](const CandidateFinger &c1, const CandidateFinger &c2) {
                                     return std::get<0>(c1) == std::get<0>(c2);
                                 }), candidates.end());

    for (const auto &candidate : candidates) {
        const auto &finger_tip_idx = std::get<0>(candidate);
        auto p1 = contour[utils::wrap_around_array((int) contour.size(), finger_tip_idx - FINGER_K_VAL)];
        auto p2 = contour[utils::wrap_around_array((int) contour.size(), finger_tip_idx + FINGER_K_VAL)];
        auto tip = contour[finger_tip_idx];
        auto mid = (p1 + p2) / 2;

        hand.fingers.emplace_back(tip, tip - mid, false);
    }

    return hand;
}

Circle hand::find_palm(const std::vector<cv::Point> &contour, const std::vector<cv::Vec4i> &defects) {
    std::vector<cv::Point> weight_points;
    for (int i = 0; i < PALM_WEIGHT_POINT_COUNT && i < defects.size(); ++i) {
        if (defects[i][3] / 256 < PALM_WEIGHT_POINT_EPSILON) break;
        weight_points.push_back(contour[defects[i][2]]);
    }

    return utils::geometry::incircle(contour, weight_points, PALM_WEIGHT_POINT_WEIGHT);
}

Circle hand::find_enclosing_circle(const std::vector<cv::Point> &contour, const Hand &hand, const double &roi) {
    // Cut off arm. Aka if something is goes outside ROI then its an arm or something else. This will be cut off
    // to the wrist aka. begin palm
    std::vector<cv::Point> arm_border;
    auto roi_sq = pow(roi, 2);
    for (int i = 0; i < contour.size(); ++i) {
        if (math::distance_sq(contour[i], hand.palm_center) < roi_sq) continue;
        auto p1 = contour[utils::wrap_around_array((int) contour.size(), i - 1)];
        auto p2 = contour[utils::wrap_around_array((int) contour.size(), i + 1)];

        if (math::distance_sq(p1, hand.palm_center) > roi_sq && math::distance_sq(p2, hand.palm_center) > roi_sq)
            continue;
        arm_border.push_back(contour[i]);
    }

    cv::Point2f enclosing_center;
    float enclosing_radius;
    if (!arm_border.empty()) {
        cv::Point arm_border_center; // TODO: this can be done better pairing 2 points to cut outside parts 1 by one
        for (const auto &p : arm_border) arm_border_center += p;
        arm_border_center /= (double) arm_border.size();

        // We draw a line/tangent to the palm circle. This line is 90 degrees to the line palm_center-arm_cut_center
        // This line is drawn at twice the palm radius since we ant to cut off everything before the line which is
        // at palm radius. Therefore we place it twice as so the former line becomes the border which is crossed
        // then that means the point is farther from palm center than the line
        auto arm_cut_center = hand.palm_center + math::normalize(arm_border_center) * hand.palm_radius * 2;
        auto arm_cut_line_p1 = math::rotate_point(arm_cut_center, hand.palm_center, M_PI / 2);
        auto arm_cut_line_p2 = arm_cut_center - (arm_cut_line_p1 - arm_cut_center);

        // Remove all the points after the line. If they are sorted correctly which i trust then it will be a clean cut.
        std::vector<cv::Point> roi_contour;
        for (const auto &p : contour) {
            if (math::distance_sq(p, hand.palm_center) < math::dist_line_sq(p, arm_cut_line_p1, arm_cut_line_p2))
                roi_contour.push_back(p);
        }

        minEnclosingCircle(roi_contour, enclosing_center, enclosing_radius);
    } else {
        minEnclosingCircle(contour, enclosing_center, enclosing_radius);
    }

    return {enclosing_center, enclosing_radius};
}

std::vector<CandidateFinger>
hand::find_fingers(const std::vector<cv::Point> &contour, const std::vector<cv::Vec4i> &defects,
                   const Hand &hand) {
    // Referencing paper "Hand tracking and gesture recognition system for human-computer interaction using
    // low-cost hardware" by "Hui-Shyong Yeo, Byung-Gook Lee, Hyotaek Lim"
    // Following methods are implementation of finger regonition descibed in the paper section: 2.3.5
    std::vector<CandidateFinger> candidates;
    for (const auto &defect : defects) {
        auto depth = defect[3] / 256;
        if (depth < hand.palm_radius || depth > hand.enclosing_radius) continue;

        auto angle = math::rad_to_deg(
                abs(math::inner_angle(contour[defect[0]], contour[defect[2]], contour[defect[1]])));
        if (angle > MAX_FINGER_DEPTH_ANGLE) continue;

        auto curvature = utils::geometry::k_curvature_peak(contour, defect[0], 15, FINGER_K_VAL);
        auto c_angle = math::rad_to_deg(std::get<1>(curvature));
        if (c_angle < MAX_FINGER_CURVATURE &&
            math::distance_sq(contour[std::get<0>(curvature)], hand.enclosing_center) <= pow(hand.enclosing_radius, 2)) {
            candidates.emplace_back(std::get<0>(curvature), defect);
        }


        curvature = utils::geometry::k_curvature_peak(contour, defect[1], 15, FINGER_K_VAL);
        c_angle = math::rad_to_deg(std::get<1>(curvature));
        if (c_angle < MAX_FINGER_CURVATURE &&
                math::distance_sq(contour[std::get<0>(curvature)], hand.enclosing_center) <= pow(hand.enclosing_radius, 2)) {
            candidates.push_back({std::get<0>(curvature), {defect[1], defect[0], defect[2], defect[3]}});
        }
    }

    return candidates;
}

std::vector<CandidateFinger>
hand::find_fingers_fallback(const std::vector<cv::Point> &contour, const std::vector<cv::Vec4i> &defects) {
    // Again using fallback methodology from the earlier referenced paper. If we find no fingers the traditional way
    // then we check for everything thats pointy
    std::vector<CandidateFinger> candidates;
    for (const auto &defect : defects) {
        if (defect[3] / 256 < EPSILON_DEPTH) continue;
        // TODO: check if finger length is not too short

        auto kpeak = utils::geometry::k_curvature_peak(contour, defect[0], 15, FINGER_K_VAL_FALLBACK);
        auto angle = math::rad_to_deg(std::get<1>(kpeak));
        if (angle < MAX_FINGER_CURVATURE_FALLBACK) candidates.emplace_back(std::get<0>(kpeak), defect);

        kpeak = utils::geometry::k_curvature_peak(contour, defect[1], 15, FINGER_K_VAL_FALLBACK);
        angle = math::rad_to_deg(std::get<1>(kpeak));
        if (angle < MAX_FINGER_CURVATURE_FALLBACK)
            candidates.push_back({std::get<0>(kpeak), {defect[1], defect[0], defect[2], defect[3]}});
    }

    return candidates;
}
