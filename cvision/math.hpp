//
// Created by egordm on 22-3-2018.
//

#ifndef B3ITASSIGNMENT2_MATH_H
#define B3ITASSIGNMENT2_MATH_H


#include <opencv2/core/types.hpp>

namespace cvision { namespace math {
    /**
     * Calculates angle between three points.
     * @param a
     * @param b Center point
     * @param c
     * @return Angle in radiants
     */
    double inner_angle(const cv::Point &a, const cv::Point &b, const cv::Point &c) {
        auto diff_ab = b - a;
        auto diff_cb = b - c;

        auto dot = diff_ab.ddot(diff_cb);
        auto cross = diff_ab.cross(diff_cb);

        return atan2(cross, dot);
    }

    double rad_to_deg(double rad) { return rad * 180.0 / M_PI;}

    double distance_sq(const cv::Point &a, const cv::Point &b) {
        auto diff = a - b;
        return diff.ddot(diff);
    }
}}


#endif //B3ITASSIGNMENT2_MATH_H
