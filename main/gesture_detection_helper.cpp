//
// Created by egordm on 22-3-2018.
//

#include "gesture_detection_helper.h"
#include "../cvision/math.hpp"
#include "../cvision/hand_recognition.h"
#include "../cvision/visualization.h"

using namespace cvision;
using namespace cv;


cv::Mat HandDetectorHelper::draw(const cv::Mat &src, const cv::Mat &original) {
    Mat mask_final;
    mask.copyTo(mask_final);

    if(faces != nullptr && !faces->empty()) {
        for(const auto &face : *faces) {
            Point center(face.x + face.width / 2, face.y + face.height / 2);
            ellipse(mask_final, center, Size(face.width / 2, face.height / 2), 0, 0, 360, Scalar(0, 0, 0), -1);
        }
    }

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask_final, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    // TODO: find some other way like proportion to image size to support multiple hands
    int largestContour = 0;
    for (int i = 1; i < contours.size(); i++) {
        if (cv::contourArea(contours[i]) > cv::contourArea(contours[largestContour])) largestContour = i;
    }

    if (!contours.empty()) {
        auto hand = processing::limb_recognition::hand::recognize_hand(contours[largestContour]);
        circle(src, hand.palm_center, (int) hand.palm_radius, Scalar(255, 255, 255), 2);
        circle(src, hand.enclosing_center, (int) hand.enclosing_radius, Scalar(255, 255, 200), 2);

        for (const auto &finger : hand.fingers) {
            circle(src, finger.tip, 2, Scalar(0, 0, 255), 2);
            line(src, finger.tip - finger.direction, finger.tip + finger.direction, Scalar(255, 0, 0), 2);
        }
    }

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
    if(!inited) return src;
    Mat frame_gray;
    faces.clear();

    cvtColor(original, frame_gray, COLOR_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);

    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

    for (const auto &face : faces) {
        Point center(face.x + face.width / 2, face.y + face.height / 2);
        ellipse(src, center, Size(face.width / 2, face.height / 2), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
    }

    return src;
}
