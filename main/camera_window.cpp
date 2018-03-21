//
// Created by egordm on 19-3-18.
//

#include "camera_window.h"
#include "../cvision/macros.h"

void CameraWindow::init() {
    M_Assert(capture.isOpened(), "Video capture should be opened!");
    Window::init();
}

cv::Mat CameraWindow::draw() {
    return draw_on(frame);
}

void CameraWindow::start_loop() {
    looping = true;
    while(true) {
        capture >> frame;
        show();
        if(cv::waitKey(30) >= 0) break;
    }
    looping = false;
}

CameraWindow::~CameraWindow() {
    capture.release();
}

void CameraWindow::show() {
    if(looping) Window::show();
    else start_loop();
}
