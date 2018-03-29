//
// Created by egordm on 29-3-2018.
//

#ifndef B3ITASSIGNMENT2_HAND_RECOGNITION_H
#define B3ITASSIGNMENT2_HAND_RECOGNITION_H

#include <opencv2/opencv.hpp>

#define PALM_WEIGHT_POINT_EPSILON 20
#define PALM_WEIGHT_POINT_COUNT 3
#define PALM_WEIGHT_POINT_WEIGHT 0.05
#define HAND_ROI_MULT 3.6
#define MAX_FINGER_DEPTH_ANGLE 90
#define MAX_FINGER_CURVATURE 80
#define MAX_FINGER_CURVATURE_FALLBACK 60
#define FINGER_K_VAL 20
#define FINGER_K_VAL_FALLBACK 20
#define EPSILON_DEPTH 20

namespace cvision { namespace processing { namespace limb_recognition { namespace hand {
    struct Finger {
        cv::Point tip;
        cv::Point direction;
        bool thumb;

        Finger(const cv::Point &tip, const cv::Point &direction, bool thumb) : tip(tip), direction(direction), thumb(thumb) {}
    };

    struct Hand {
        cv::Point palm_center;
        double palm_radius;

        cv::Point enclosing_center;
        double enclosing_radius;

        double palm_to_hand_ratio;
        std::vector<Finger> fingers;
    };

    using Circle = std::tuple<cv::Point, double>;

    using CandidateFinger = std::tuple<int, cv::Vec4i>;

    /**
     * Recognize a hand in a given shape. Contour must have at least 3 vertices
     * @param contour
     * @return
     */
    Hand recognize_hand(const std::vector<cv::Point> &contour);

    /**
     * Find the hand palm which is a largest circle that can fit into the hand
     * @param contour
     * @param defects
     * @return
     */
    Circle find_palm(const std::vector<cv::Point> &contour, const std::vector<cv::Vec4i> &defects);

    /**
     * Find circle around the hand. If we hand an arm attached we try to cut it out first
     * @param contour
     * @param hand
     * @param roi
     * @return
     */
    Circle find_enclosing_circle(const std::vector<cv::Point> &contour, const Hand &hand, const double &roi);

    /**
     * Find the fingers the conventional way. By checking the gap and their sizes.
     * This works only if there are more than one finger present.
     * @param contour
     * @param defects
     * @param hand
     * @return
     */
    std::vector<CandidateFinger> find_fingers(const std::vector<cv::Point> &contour,
                                              const std::vector<cv::Vec4i> &defects, const Hand &hand);

    /**
     * A desperate way to find fingers if none found the conventional way. We check all the peaks  if they are pointy
     * enough to be considered fingers
     * @param contour
     * @param defects
     * @return
     */
    std::vector<CandidateFinger> find_fingers_fallback(const std::vector<cv::Point> &contour,
                                                       const std::vector<cv::Vec4i> &defects);
}}}}


#endif //B3ITASSIGNMENT2_HAND_RECOGNITION_H
