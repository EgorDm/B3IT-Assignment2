//
// Created by egordm on 19-3-2018.
//

#ifndef B3ITASSIGNMENT2_WINDOW_CREATORS_H
#define B3ITASSIGNMENT2_WINDOW_CREATORS_H


#include "custom_windows.h"

Window* create_complex_seg_window(const file::Dataset &dataset, const std::string &sample_name, bool webcam);

Window* create_simple_seg_window(const file::Dataset &dataset, const std::string &sample_name, bool webcam);

#endif //B3ITASSIGNMENT2_WINDOW_CREATORS_H
