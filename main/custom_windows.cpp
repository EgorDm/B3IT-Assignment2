//
// Created by egordm on 17-3-2018.
//

#include "custom_windows.h"
#include "../cvision/segmentation.h"
#include "../cvision/evaluation.h"
#include "window_helpers.h"

using namespace cvision::algorithms;
using namespace cvision::evaluation;

void SampleEvaluationWindow::init() {
    helpers.push_back(cf_helper);
    Window::init();
}

Mat SampleEvaluationWindow::draw() {
    Mat ret = draw_on(sample.input);
    cf_helper->evaluate(sample, ret);
    return ret;
}