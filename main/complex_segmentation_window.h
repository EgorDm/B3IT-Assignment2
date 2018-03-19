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
#include "segmentation_helper.h"

using namespace cvision::processing;

#define COMPLEX_SEG_WINDOW_NAME "Complex Segmentation (Bayes)"

class ComplexSegmentationWindow : public Window {
private:
    file::ImageSample sample;
    ComplexSegmentationHelper *helper;

public:
    ComplexSegmentationWindow(file::ImageSample sample, ComplexSegmentationHelper *helper)
            : Window(COMPLEX_SEG_WINDOW_NAME), helper(helper), sample(std::move(sample)) { }

    void init() override;

    cv::Mat draw() override;
};

ComplexSegmentationWindow
create_complex_seg_window(const std::string &dataset_name, const std::string &sample_name, const bool inverted,
                          const std::string &ext_in,
                          const std::string &ext_lbl);


#endif //B3ITASSIGNMENT2_EVAL_WINDOW_H
