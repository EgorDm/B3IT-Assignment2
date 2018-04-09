//
// Created by egordm on 7-4-2018.
//

#ifndef B3ITASSIGNMENT2_GESTURE_RECOGNITION_HELPER_H
#define B3ITASSIGNMENT2_GESTURE_RECOGNITION_HELPER_H


#include "window.h"
#include "../dollar_recognizer/GeometricRecognizer.h"
#include "detection_helper.h"

struct GestureDetectSession {
    int hand_id;
    int finger_id;
    DollarRecognizer::Path2D positions;

    GestureDetectSession(int hand_id, int finger_id, const DollarRecognizer::Path2D &positions)
            : hand_id(hand_id),  finger_id(finger_id), positions(positions) {}
};

class GestureDetectionHelper : public WindowHelper {
private:
    DollarRecognizer::GeometricRecognizer recognizer;
    DollarRecognizer::Path2D positions;
    bool drawing;

    const HandTrackingHelper *hand_tracker;
public:
    DollarRecognizer::RecognitionResult *result;
    std::vector<GestureDetectSession> sessions;

    explicit GestureDetectionHelper(const std::vector<std::string> &active_templates, const HandTrackingHelper *hand_tracker);

    virtual ~GestureDetectionHelper() { delete result; }

    bool on_click(int x, int y, bool rb) override;

    cv::Mat draw(const cv::Mat &src, const cv::Mat &original) override;
};




#endif //B3ITASSIGNMENT2_GESTURE_RECOGNITION_HELPER_H
