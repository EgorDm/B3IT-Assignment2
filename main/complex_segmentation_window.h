//
// Created by egordm on 17-3-2018.
//

#ifndef B3ITASSIGNMENT2_EVAL_WINDOW_H
#define B3ITASSIGNMENT2_EVAL_WINDOW_H


#include "window.h"

#include <utility>
#include "../cvision/image_processing.h"
#include "../cvision/file_processing.h"
#include "../cvision/evaluation.h"

using namespace cvision::processing;

#define COMPLEX_SEG_WINDOW_NAME "Complex Segmentation (Bayes)"

class ComplexSegmentationWindow : public Window {
private:
    image::Histogram skin_histogram;
    image::Histogram env_histogram;
    float marginal_positive_probability;
    file::ImageSample sample;
    int threshold, preblur, ed_size, close_size, postblur, marginal_weight;
    float pskin{}, penv{}, pbayes{};
    Vec3b colour;
    cvision::evaluation::ConfusionMatrixResults cf_results{};

public:
    ComplexSegmentationWindow(const image::Histogram &skin_histogram, const image::Histogram &env_histogram,
               float marginal_positive_probability, file::ImageSample sample)
            : Window(COMPLEX_SEG_WINDOW_NAME), skin_histogram(skin_histogram), env_histogram(env_histogram),
              marginal_positive_probability(marginal_positive_probability), sample(std::move(sample)),
              threshold(50), preblur(4), ed_size(4), close_size(4), postblur(4), marginal_weight(100) {
        init();
    }

    cv::Mat draw() override;

    static void on_trackbar(int newValue, void *object);

    void on_click(int x, int y) override;

    std::string get_recall();

    std::string get_precision();

    std::string get_score();

    std::string get_error();

    std::string get_probability();

    std::string get_probability2();

private:
    void init() override;
};


#endif //B3ITASSIGNMENT2_EVAL_WINDOW_H
