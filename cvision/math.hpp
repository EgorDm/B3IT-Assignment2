//
// Created by egordm on 22-3-2018.
//

#ifndef B3ITASSIGNMENT2_MATH_H
#define B3ITASSIGNMENT2_MATH_H

#include <opencv2/core/types.hpp>


// Reused code from my halite bot :)
namespace cvision { namespace math {
    /**
     * Calculates angle between three points.
     * @param a
     * @param b Center point
     * @param c
     * @return Angle in radiants
     */
    inline double inner_angle(const cv::Point &a, const cv::Point &b, const cv::Point &c) {
        auto diff_ab = b - a;
        auto diff_cb = b - c;

        auto dot = diff_ab.ddot(diff_cb);
        auto cross = diff_ab.cross(diff_cb);

        return atan2(cross, dot);
    }

    inline double rad_to_deg(double rad) { return rad * 180.0 / M_PI; }

    inline double distance_sq(const cv::Point &a, const cv::Point &b) {
        auto diff = a - b;
        return diff.ddot(diff);
    }

    inline cv::Point closest_point_line(const cv::Point &p, const cv::Point &a, const cv::Point &b) {
        auto ds = distance_sq(a, b);
        if (ds < 0.1) return a;

        auto delta = b - a;
        auto t = std::fmax(0, std::fmin(1, (p - a).ddot(delta) / ds));

        return delta * t + a;
    }

    inline double dist_line_sq(const cv::Point &p, const cv::Point &a, const cv::Point &b) {
        return distance_sq(p, closest_point_line(p, a, b));
    }

    inline double length(const cv::Point &p) {
        return sqrt(p.ddot(p));
    }

    inline cv::Point normalize(const cv::Point &p) {
        auto l = length(p);
        if (l == 0) return {};

        return p / l;
    }

    inline cv::Point rotate_point(cv::Point p, const cv::Point &center, double angle) {
        auto s = sin(angle);
        auto c = sin(angle);

        // Translate to origin
        p = p - center;

        // Rotate
        auto xnew = p.x * c - p.y * s;
        auto ynew = p.x * s + p.y * c;

        // translate point back:
        return {(int) (xnew + center.x), (int) (ynew + center.y)};
    }
}}


#endif //B3ITASSIGNMENT2_MATH_H
