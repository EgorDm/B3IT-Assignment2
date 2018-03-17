//
// Created by egordm on 17-3-2018.
//

#ifndef B3ITASSIGNMENT2_SIMPLE_SEGMENTATION_WINDOW_H
#define B3ITASSIGNMENT2_SIMPLE_SEGMENTATION_WINDOW_H

#include "window.h"

#include <utility>
#include "../cvision/image_processing.h"
#include "../cvision/file_processing.h"
#include "../cvision/evaluation.h"

using namespace cvision::processing;

#define COMPLEX_SEG_WINDOW_NAME "Complex Segmentation (Bayes)"

class SimpleSegmentationWindow : public Window {
private:
    const image::Histogram *skin_histogram;
    const float marginal_positive_probability;
    file::ImageSample sample;
    int alpha, beta, gamma, preblur, ed_size, close_size, postblur;
    cvision::evaluation::ConfusionMatrixResults cf_results{};

public:
    SimpleSegmentationWindow(const image::Histogram *skin_histogram, float marginal_positive_probability,
                             file::ImageSample sample)
            : Window(COMPLEX_SEG_WINDOW_NAME), skin_histogram(skin_histogram),
              marginal_positive_probability(marginal_positive_probability), sample(std::move(sample)),
              alpha(4), beta(20), gamma(5), preblur(4), ed_size(4), close_size(4), postblur(4) {
        init();
    }

    cv::Mat draw() override;

    static void on_trackbar(int newValue, void *object);

    std::string get_recall();

    std::string get_precision();

    std::string get_score();

    std::string get_error();

private:
    void init() override;
};

SimpleSegmentationWindow
create_simple_seg_window(const std::string &dataset_name, const std::string &sample_name, const bool inverted,
                         const std::string &ext_in,
                         const std::string &ext_lbl);

#endif //B3ITASSIGNMENT2_SIMPLE_SEGMENTATION_WINDOW_H
