//
// Created by egordm on 19-3-2018.
//

#include "window_creators.h"

SampleEvaluationWindow
create_complex_seg_window(const std::string &dataset_name, const std::string &sample_name, const bool inverted,
                          const std::string &ext_in, const std::string &ext_lbl) {
    auto dataset = file::load_dataset(dataset_name, inverted,ext_in, ext_lbl);
    auto sample = file::load_sample(dataset_name, sample_name, inverted, ext_in, ext_lbl);

    Mat dataset_inputs[dataset.size()];
    Mat dataset_masks[dataset.size()];
    Mat dataset_masks_inv[dataset.size()];

    for (int i = 0; i < dataset.size(); ++i) {
        dataset_inputs[i] = dataset[i].input;
        dataset_masks[i] = dataset[i].label;
        bitwise_not(dataset_masks[i], dataset_masks_inv[i]);
    }

    const float ranges[3] = {256, 256, 256};

    auto positive_hst = image::extract_histogram(dataset_inputs, (uint) dataset.size(), ranges, dataset_masks, 256);
    auto env_hst = image::extract_histogram(dataset_inputs, (uint) dataset.size(), ranges, dataset_masks_inv, 256);
    auto marginal_positive_prob = image::probability_masked_pixels(dataset_masks, (uint) dataset.size());

    return SampleEvaluationWindow("Complex Segmentation (Bayes)", {new ComplexSegmentationHelper(positive_hst, env_hst, marginal_positive_prob)}, sample);
}


SampleEvaluationWindow
create_simple_seg_window(const std::string &dataset_name, const std::string &sample_name, const bool inverted,
                         const std::string &ext_in, const std::string &ext_lbl) {
    auto dataset = file::load_dataset(dataset_name, inverted,ext_in, ext_lbl);
    auto sample = file::load_sample(dataset_name, sample_name, inverted, ext_in, ext_lbl);

    Mat dataset_inputs[dataset.size()];
    Mat dataset_masks[dataset.size()];
    Mat dataset_masks_inv[dataset.size()];

    for (int i = 0; i < dataset.size(); ++i) {
        cvtColor(dataset[i].input, dataset_inputs[i], COLOR_BGR2HSV);
        dataset_masks[i] = dataset[i].label;
        bitwise_not(dataset_masks[i], dataset_masks_inv[i]);
    }

    const float ranges[3] = {180, 256, 256};

    auto positive_hst = image::extract_histogram(dataset_inputs, (uint) dataset.size(), ranges, dataset_masks, 128);
    auto marginal_positive_prob = image::probability_masked_pixels(dataset_masks, (uint) dataset.size());

    return SampleEvaluationWindow("Simple Segmentation", {new SimpleSegmentationHelper(positive_hst)}, sample);
}
