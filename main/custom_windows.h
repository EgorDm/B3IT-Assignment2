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
#include "window_helpers.h"

using namespace cvision::processing;

class SampleEvaluationWindow : public Window {
private:
    file::ImageSample sample;
    CFMatrixHelper *cf_helper;

public:
    SampleEvaluationWindow(const std::string &window_name, const std::vector<WindowHelper *> &helpers,
                           file::ImageSample sample)
            : Window(window_name, helpers), sample(std::move(sample)), cf_helper(new CFMatrixHelper()) {}

    void init() override;

protected:
    Mat draw() override;
};


#endif //B3ITASSIGNMENT2_EVAL_WINDOW_H
