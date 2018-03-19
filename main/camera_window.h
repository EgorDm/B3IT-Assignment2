/*
//
// Created by egordm on 19-3-18.
//

#ifndef B3IT_ASSIGNMENT2_CAMERA_WINDOW_H
#define B3IT_ASSIGNMENT2_CAMERA_WINDOW_H


#include "window.h"

class CameraWindow : public Window {
private:
    cv::VideoCapture capture;
    cv::Mat frame;

public:
    explicit CameraWindow(const std::string &window_name = "Camera Window")
            : Window(window_name), capture(cv::VideoCapture(0)) {};

    virtual ~CameraWindow() {
        capture.release();
    }

    void start_loop();


protected:
    void init() override;

    cv::Mat draw() override;
};


#endif //B3IT_ASSIGNMENT2_CAMERA_WINDOW_H
*/
