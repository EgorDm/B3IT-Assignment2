/*
//
// Created by egordm on 19-3-18.
//

#include "camera_window.h"
#include "../cvision/macros.h"

void CameraWindow::init() {
    M_Assert(capture.isOpened(), "Video capture should be opened!");
}

cv::Mat CameraWindow::draw() {
    return frame;
}

void CameraWindow::start_loop() {
    while(true) {
        capture >> frame;
        show();
        if(cv::waitKey(30) >= 0) break;
    }
}
*/
