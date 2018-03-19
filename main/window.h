//
// Created by egordm on 17-3-2018.
//

#ifndef B3ITASSIGNMENT2_WINDOW_H
#define B3ITASSIGNMENT2_WINDOW_H

#include <string>
#include <opencv2/opencv.hpp>
#include <utility>


typedef std::function<std::string()> Statistic;
typedef std::tuple<const std::string, int *, int> Trackbar;

class WindowHelper {
public:
    virtual std::vector<Statistic> get_statistics() { return {}; }

    virtual std::vector<Trackbar> get_trackbars() { return {}; }

    virtual cv::Mat draw(const cv::Mat &src) { return src; }

    virtual bool on_click(int x, int y) {}
};

class Window {
protected:
    const std::string window_name;
    std::vector<Statistic> statistics;
    std::vector<WindowHelper *> helpers;

public:
    explicit Window(const std::string &window_name);

    Window(const std::string &window_name, const std::vector<WindowHelper *> &helpers);

    virtual ~Window();

    virtual void init();

    void show();

    virtual void on_click(int x, int y);

    static void on_mouse(int event, int x, int y, int, void *object);

    static void on_trackbar(int newValue, void *object);

protected:
    virtual cv::Mat draw() = 0;

    virtual cv::Mat draw_on(const cv::Mat &src);

    int stat_panel_height();

    void add_statistic(const Statistic &stat);
};

#endif //B3ITASSIGNMENT2_WINDOW_H
