//
// Created by egordm on 19-3-2018.
//

#ifndef B3ITASSIGNMENT2_WINDOW_CREATORS_H
#define B3ITASSIGNMENT2_WINDOW_CREATORS_H


#include "custom_windows.h"

SampleEvaluationWindow
create_complex_seg_window(const std::string &dataset_name, const std::string &sample_name, const bool inverted,
                          const std::string &ext_in,
                          const std::string &ext_lbl);

SampleEvaluationWindow
create_simple_seg_window(const std::string &dataset_name, const std::string &sample_name, const bool inverted,
                         const std::string &ext_in,
                         const std::string &ext_lbl);

#endif //B3ITASSIGNMENT2_WINDOW_CREATORS_H
