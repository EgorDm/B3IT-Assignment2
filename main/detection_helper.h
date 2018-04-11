//
// Created by egordm on 22-3-2018.
//

#ifndef B3ITASSIGNMENT2_GESTURE_DETECTION_HELPER_H
#define B3ITASSIGNMENT2_GESTURE_DETECTION_HELPER_H


#include "window.h"
#include "../cvision/defines.h"
#include "../cvision/hand_recognition.h"
#include "../cvision/object_tracking.h"

#define MIN_HAND_AREA 4000
// Tracking
#define HAND_HISTORY_SIZE 3
#define FINGER_HISTORY_SIZE 5
#define DISTANCE_THRESH (min_dim * 6.0)
#define HAND_TIMEOUT_TICKS 5
#define FINGER_TIMEOUT_TICKS 5
#define MIN_HAND_RADIUS 20

using namespace cvision::processing::limb_recognition::hand;
using namespace cvision::processing::tracking;

class HandDetectorHelper : public WindowHelper {
private:
    const cv::Mat &mask;
    const std::vector<cv::Rect> *faces;
public:
    std::vector<cvision::processing::limb_recognition::hand::Hand> hands;
    explicit HandDetectorHelper(const cv::Mat &mask, const std::vector<cv::Rect> *faces = nullptr) : mask(mask), faces(faces) {}

    cv::Mat draw(const cv::Mat &src, const cv::Mat &original) override;
};


#define FACE_CASCADE_NAME "haarcascade_frontalface_alt.xml"

class FaceDetectorHelper : public WindowHelper {
private:
    cv::CascadeClassifier face_cascade;
    bool inited = true;

public:
    std::vector<cv::Rect> faces;

    FaceDetectorHelper();

    cv::Mat draw(const cv::Mat &src, const cv::Mat &original) override;
};

struct HandHistory : public TrackHistory<Hand> {
    std::vector<TrackHistory<Finger>*> finger_histories;

    HandHistory(const Hand &o, int identifier) : TrackHistory(o, identifier) {}

    virtual ~HandHistory() {
        for(auto &finger : finger_histories) delete finger;
        finger_histories.clear();
    }
};

class HandTrackingHelper : public WindowHelper {
private:
    const HandDetectorHelper *hands_detector;
    ObjectTracker<Hand, HandHistory> hand_tracker;
    ObjectTracker<Finger, TrackHistory<Finger>> finger_tracker;
public:
    std::vector<HandHistory*> hand_histories;

    explicit HandTrackingHelper(const HandDetectorHelper *hands_detector)
            : hands_detector(hands_detector), hand_tracker(HAND_HISTORY_SIZE, HAND_TIMEOUT_TICKS, 0, &distance_hand),
              finger_tracker(FINGER_HISTORY_SIZE, FINGER_TIMEOUT_TICKS, 0, &distance_finger){}

    virtual ~HandTrackingHelper() {
        for(auto &hand : hand_histories) delete hand;
        hand_histories.clear();
    }

    cv::Mat draw(const cv::Mat &src, const cv::Mat &original) override;

    static double distance_hand(const Hand &state, const HandHistory* history);

    static double distance_finger(const Finger &state, const TrackHistory<Finger>* history);

    static Hand average_hand(const HandHistory* history, const int &lookup_length = HAND_HISTORY_SIZE);

    static Finger average_finger(const TrackHistory<Finger>* history, const int &lookup_length = FINGER_HISTORY_SIZE);
};


#endif //B3ITASSIGNMENT2_GESTURE_DETECTION_HELPER_H
