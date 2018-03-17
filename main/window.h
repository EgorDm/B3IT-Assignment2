//
// Created by egordm on 17-3-2018.
//

#ifndef B3ITASSIGNMENT2_WINDOW_H
#define B3ITASSIGNMENT2_WINDOW_H

#include <string>
#include <opencv2/opencv.hpp>
#include <utility>


typedef std::function<std::string()> Statistic;


class Window {
protected:
    const std::string window_name;
    std::vector<Statistic> statistics;

public:
    explicit Window(const std::string &window_name);

    void show();

    static void on_mouse(int event, int x, int y, int, void *object);

    virtual void on_click(int x, int y) {}

    int stat_panel_height();
protected:
    virtual void init() = 0;

    virtual cv::Mat draw() = 0;
};

#endif //B3ITASSIGNMENT2_WINDOW_H
