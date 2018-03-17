//
// Created by egordm on 17-3-2018.
//

#include <opencv/cv.hpp>
#include "window.h"

#define STAT_HEIGHT 18
#define STAT_MARGIN 8

Window::Window(const std::string &window_name) : window_name(window_name) {
    cv::namedWindow(window_name, CV_WINDOW_NORMAL);//CV_WINDOW_KEEPRATIO
    cv::setMouseCallback(window_name, &on_mouse, this);
}

void Window::show() {
    cv::Mat res = draw();

    if (statistics.empty()) {
        cv::imshow(window_name, res);
        return;
    }
    auto stats_bar_height = stat_panel_height();
    cv::Scalar value(255, 255, 255);
    cv::copyMakeBorder(res, res, stats_bar_height, 0, 0, 0, CV_HAL_BORDER_CONSTANT, value);
    for (int i = 0; i < statistics.size(); ++i) {
        cv::putText(res, statistics[i](), cv::Point(10, STAT_HEIGHT * (i + 1)), CV_FONT_NORMAL, 0.46,
                    cv::Scalar::all(0));
    }

    cv::imshow(window_name, res);
}

void Window::on_mouse(int event, int x, int y, int, void *object) {
    if (event == CV_EVENT_LBUTTONDOWN) {
        auto win = ((Window *) object);
        win->on_click(x, y - win->stat_panel_height());
    }
}

int Window::stat_panel_height() {
    return static_cast<int>(statistics.size() * STAT_HEIGHT + STAT_MARGIN);
}
