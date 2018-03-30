//
// Created by egordm on 22-3-2018.
//

#ifndef B3ITASSIGNMENT2_GESTURE_DETECTION_HELPER_H
#define B3ITASSIGNMENT2_GESTURE_DETECTION_HELPER_H


#include "window.h"
#include "../cvision/defines.h"

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

#endif //B3ITASSIGNMENT2_GESTURE_DETECTION_HELPER_H
