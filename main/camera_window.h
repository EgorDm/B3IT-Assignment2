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
    bool looping = false;

public:
    explicit CameraWindow(const std::string &window_name = "Camera Window")
            : Window(window_name), capture(cv::VideoCapture(0)) {};

    CameraWindow(const std::string &window_name, const std::vector<WindowHelper *> &helpers)
            : Window(window_name, helpers), capture(cv::VideoCapture(0)) {};

    ~CameraWindow() override;

    void start_loop();

    void show() override;

protected:
    void init() override;

    cv::Mat draw() override;
};


#endif //B3IT_ASSIGNMENT2_CAMERA_WINDOW_H