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

    virtual cv::Mat draw(const cv::Mat &src, const cv::Mat &original) { return src; }

    virtual bool on_click(int x, int y, bool rb) {}
};

class InputHelper : public WindowHelper {
private:
    cv::VideoCapture capture;
    bool is_video{};
    bool output;
    bool flip = false;
public:
    cv::Mat frame;

    explicit InputHelper(cv::Mat input, const bool &output = false) : frame(std::move(input)), output(output) {}

    explicit InputHelper(const std::string &source, const bool &output = false)
            : capture(cv::VideoCapture(source)), is_video(true), output(output) {
        draw(cv::Mat(), cv::Mat());
    }

    explicit InputHelper(const int &source_camera, const bool &output = false)
            : capture(cv::VideoCapture(source_camera)), is_video(true), output(output), flip(true) {
        draw(cv::Mat(), cv::Mat());
    }

    cv::Mat draw(const cv::Mat &src, const cv::Mat &original) override {
        if (is_video) {
            capture >> frame;
            if(flip) cv::flip(frame, frame, 1);
        }
        if(output) return frame;
        return WindowHelper::draw(src, original);
    }
};

class Window {
protected:
    const std::string window_name;
    std::vector<Statistic> statistics;
    std::vector<WindowHelper *> helpers;
    InputHelper *source;
public:
    bool debug;

    explicit Window(const std::string &window_name, InputHelper *source);

    Window(const std::string &window_name, InputHelper *source, const std::vector<WindowHelper *> &helpers);

    virtual ~Window();

    virtual void init();

    virtual void show();

    virtual void on_click(int x, int y, bool rb);

    static void on_mouse(int event, int x, int y, int, void *object);

    static void on_trackbar(int newValue, void *object);

protected:
    virtual cv::Mat draw();

    virtual cv::Mat draw_on(const cv::Mat &src);

    int stat_panel_height();

    void add_statistic(const Statistic &stat);
};


#endif //B3ITASSIGNMENT2_WINDOW_H
