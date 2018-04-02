//
// Created by egordm on 22-3-2018.
//

#ifndef B3ITASSIGNMENT2_GESTURE_DETECTION_HELPER_H
#define B3ITASSIGNMENT2_GESTURE_DETECTION_HELPER_H


#include "window.h"
#include "../cvision/defines.h"
#include "../dollar_recognizer/GeometricRecognizer.h"

#define MIN_HAND_AREA 4000

class HandDetectorHelper : public WindowHelper {
private:
    const cv::Mat &mask;
    const std::vector<cv::Rect> *faces;
public:
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

class GestureDetectionHelper : public WindowHelper {
private:
    DollarRecognizer::GeometricRecognizer recognizer;
    DollarRecognizer::Path2D positions;

    bool drawing;
public:
    DollarRecognizer::RecognitionResult *result;

    GestureDetectionHelper(const std::vector<std::string> &active_templates);

    virtual ~GestureDetectionHelper() { delete result; }

    bool on_click(int x, int y, bool rb) override;

    cv::Mat draw(const cv::Mat &src, const cv::Mat &original) override;
};

#endif //B3ITASSIGNMENT2_GESTURE_DETECTION_HELPER_H
