//
// Created by egordm on 22-3-2018.
//

#include "detection_helper.h"
#include "../cvision/math.hpp"
#include "../cvision/hand_recognition.h"
#include "../cvision/visualization.h"

using namespace cvision;
using namespace cv;




cv::Mat HandDetectorHelper::draw(const cv::Mat &src, const cv::Mat &original) {
    Mat mask_final;
    mask.copyTo(mask_final);

    // Dilemma subtract face from mask which enables having a portion of hand over face while still finding the other fingers
    // or subtract face form contours and dont have any flase fositives round the face area but also no hand if it
    // overlaps even slightly
    // Here the first approach is taken
    if (faces != nullptr && !faces->empty()) {
        for (const auto &face : *faces) {
            Point center(face.x + face.width / 2, face.y + face.height / 2);
            ellipse(mask_final, center, Size(face.width / 2, face.height / 2), 0, 0, 360, Scalar(0, 0, 0), -1);
        }
    }

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask_final, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    contours.erase(std::remove_if(contours.begin(), contours.end(), [](const std::vector<cv::Point> &contour) {
        return cv::contourArea(contour) < MIN_HAND_AREA;
    }), contours.end());

    hands.clear();
    std::vector<std::vector<cv::Point>> roi_contours;
    for (const auto &contour : contours) {
        if(contour.size() < 3) continue;
        bool is_face = false;
        for(const auto &face : *faces) {
            Point center(face.x + face.width / 2, face.y + face.height / 2);
            if(cv::pointPolygonTest(contour, center, false) > 0) {
                is_face = true;
                break;
            }
        }
        if(is_face) continue;
        std::vector<cv::Point> roi_contour;
        auto hand = processing::limb_recognition::hand::recognize_hand(contour, roi_contour);
        if(hand.palm_radius < MIN_HAND_RADIUS && hand.enclosing_radius > MIN_HAND_RADIUS) continue; // Small hands are removed
        hands.push_back(hand);
        roi_contours.push_back(roi_contour);
    }
    drawContours(src, roi_contours, -1, Scalar(0, 0, 255), 2);

    return src;
}

FaceDetectorHelper::FaceDetectorHelper() {
    std::stringstream cascade_name;
    cascade_name << OPENCV_DATASET_PATH << FACE_CASCADE_NAME;
    if (!face_cascade.load(cascade_name.str())) {
        inited = false;
        std::cout << "Failed loading face dataset!" << std::endl;
    }
}

Mat FaceDetectorHelper::draw(const cv::Mat &src, const cv::Mat &original) {
    if (!inited) return src;
    Mat frame_gray;
    faces.clear();

    cvtColor(original, frame_gray, COLOR_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);

    face_cascade.detectMultiScale(frame_gray, faces, 1.3, 2, 0 | CASCADE_SCALE_IMAGE, Size(35, 35));

    for (const auto &face : faces) {
        Point center(face.x + face.width / 2, face.y + face.height / 2);
        ellipse(src, center, Size(face.width / 2, face.height / 2), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
    }

    return src;
}

#define DISTANCE_THRESH (min_dim * 6.0)


cv::Mat HandTrackingHelper::draw(const cv::Mat &src, const cv::Mat &original) {
    const auto &hands = hands_detector->hands;
    const int min_dim = min(original.rows, original.cols);

    hand_tracker.distance_threshold = DISTANCE_THRESH;
    finger_tracker.distance_threshold = DISTANCE_THRESH/2;
    hand_tracker.update(hands, hand_histories);

    for (const auto &history : hand_histories) {
        finger_tracker.update(history->state_history.back().fingers, history->finger_histories);

        std::stringstream ss;
        ss << history->identifier;

        auto hand = average_hand(history);
        circle(src, hand.palm_center, (int) hand.palm_radius, cv::Scalar(255, 255, 255), 2);
        circle(src, hand.enclosing_center, (int) hand.enclosing_radius, cv::Scalar(255, 255, 200), 2);
        cv::putText(src, ss.str(), hand.palm_center, cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(255, 255, 0));

        for(const auto &finger_history : history->finger_histories) {
            auto finger = average_finger(finger_history);
            std::stringstream sz;
            sz << finger_history->identifier;

            circle(src, finger.tip + hand.palm_center, 2, cv::Scalar(0, 0, 255), 2);
            line(src, finger.tip - finger.direction/2 + hand.palm_center, finger.tip + finger.direction/2 + hand.palm_center, cv::Scalar(255, 0, 0), 2);
            line(src, finger.tip + hand.palm_center, hand.palm_center, cv::Scalar(0, 255, 255), 2);
            cv::putText(src, sz.str(), finger.tip + hand.palm_center, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 255));
        }
    }

    return WindowHelper::draw(src, original);
}

double HandTrackingHelper::distance_hand(const Hand &state, const HandHistory *history) {
    return math::distance_sq(state.palm_center, history->state_history.back().palm_center);
}

double HandTrackingHelper::distance_finger(const Finger &state, const TrackHistory<Finger> *history) {
    return math::distance_sq(state.tip, history->state_history.back().tip);
}

Hand HandTrackingHelper::average_hand(const HandHistory * history, const int &lookup_length) {
    Hand ret;
    int lookup_count = min((int)history->state_history.size(), lookup_length);
    int lookup_till = (int)history->state_history.size() - lookup_count;
    for(int i = (int)history->state_history.size() - 1; i >= lookup_till; --i) {
        auto state = history->state_history[i];
        ret.enclosing_center += state.enclosing_center;
        ret.palm_center += state.palm_center;
        ret.enclosing_radius += state.enclosing_radius;
        ret.palm_radius += state.palm_radius;
        ret.palm_to_hand_ratio += state.palm_to_hand_ratio;
    }
    ret.enclosing_center /= lookup_count;
    ret.palm_center /= lookup_count;
    ret.enclosing_radius /= lookup_count;
    ret.palm_radius /= lookup_count;
    ret.palm_to_hand_ratio /= lookup_count;

    return ret;
}

Finger HandTrackingHelper::average_finger(const TrackHistory<Finger> * history, const int &lookup_length) {
    cv::Point tip;
    cv::Point direction;
    bool thumb;

    int lookup_count = min((int)history->state_history.size(), lookup_length);
    int lookup_till = (int)history->state_history.size() - lookup_count;
    for(int i = (int)history->state_history.size() - 1; i >= lookup_till; --i) {
        auto state = history->state_history[i];
        tip += state.tip;
        direction += state.direction;
        thumb = state.thumb;
    }
    tip /= lookup_count;
    direction /= lookup_count;

    return {tip, direction, thumb};
}
