//
// Created by egordm on 19-3-2018.
//

#ifndef B3ITASSIGNMENT2_WINDOW_CREATORS_H
#define B3ITASSIGNMENT2_WINDOW_CREATORS_H

#include "window.h"
#include "../cvision/image_processing.h"
#include "../cvision/file_processing.h"

using namespace cvision::processing;

struct ComplexDatasetData {
    float marginal_positive_prob;
    image::Histogram3D* positive_hst;
    image::Histogram3D* env_hst;
};

ComplexDatasetData load_complex_dataset_data(const file::Dataset *datasets, int dataset_count, const std::string &save_name = "complex_seg");

#endif //B3ITASSIGNMENT2_WINDOW_CREATORS_H
