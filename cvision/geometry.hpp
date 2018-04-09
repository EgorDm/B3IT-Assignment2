//
// Created by egordm on 29-3-2018.
//

#ifndef B3ITASSIGNMENT2_GEOMETRY_H
#define B3ITASSIGNMENT2_GEOMETRY_H

#include <opencv2/opencv.hpp>
#include "math.hpp"
#include "utils.hpp"

namespace cvision { namespace utils { namespace geometry {
    /**
     * Search the k curvature peak in the given range
     * @param contour
     * @param pos Start index
     * @param range Amount of indices to search left and right
     * @param k
     * @return Tuple of the peak index and its curvature
     */
    inline std::tuple<int, double> k_curvature_peak(const std::vector<cv::Point> &contour, const int &pos, const int &range,
                                             const int &k) {
        if (contour.size() < k + 1) return {pos, 9999};

        int min_curv_i = pos;
        double min_curv = 9999;
        double curv;
        for (int i = pos - range; i < pos + range; ++i) {
            int p1 = cvision::utils::wrap_around_array((int) contour.size(), i - k);
            int p2 = cvision::utils::wrap_around_array((int) contour.size(), i);
            int p3 = cvision::utils::wrap_around_array((int) contour.size(), i + k);

            curv = abs(math::inner_angle(contour[p1], contour[p2], contour[p3]));
            if (curv >= min_curv) continue;
            if (contour[p1] == contour[p3] || contour[p3] == contour[p2] || contour[p1] == contour[p2]) continue;

            min_curv = curv;
            min_curv_i = p2;
        }

        return {min_curv_i, min_curv};
    }

    /**
     * Calculate largest incircle nearest to given weight points. If none given then its just neares incircle
     * @param contour
     * @param weight_points
     * @param weight
     * @param step_size Determizes amount of iterations and the precision of the circle
     * @return Tuple of circle center and its radius
     */
    inline std::tuple<cv::Point, double>
    incircle(const std::vector<cv::Point> &contour, const std::vector<cv::Point> &weight_points, double weight,
             const int step_size = 4) {
        // Simplify contour
        auto epsilon = 0.005 * arcLength(contour, true);
        std::vector<cv::Point> approx;
        cv::approxPolyDP(contour, approx, epsilon, true);

        // Initialize search bounds
        auto brect = boundingRect(contour);
        int max_dx = brect.width / 2, max_dy = brect.height / 2;
        int x = brect.x + max_dx, y = brect.y + max_dy;

        // Loop from the middle out to narrow out the search area
        cv::Point candidate;
        double candidate_radius = 0;
        double candidate_score = 0;
        for (int dx = 0; dx < max_dx; dx += step_size) { // Much for such loop
            for (int dy = 0; dy < max_dy; dy += step_size) {
                for (int px = -1; px <= 1; px += 2) {
                    for (int py = -1; py <= 1; py += 2) {
                        cv::Point point(x + dx * px, y + dy * py);
                        double radius = pointPolygonTest(approx, point, true);
                        double point_score = radius;
                        for (const auto &weight_point : weight_points)
                            point_score -= sqrt(math::distance_sq(weight_point, point)) * weight;

                        if (point_score <= candidate_score) continue;
                        candidate = point;
                        candidate_radius = radius;
                        candidate_score = point_score;
                        max_dx = static_cast<int>(brect.width / 2 - candidate_radius);
                        max_dy = static_cast<int>(brect.height / 2 - candidate_radius);
                    }
                }
            }
        }

        return {candidate, candidate_radius};
    };
}}}


#endif //B3ITASSIGNMENT2_GEOMETRY_H
