//
// Created by egordm on 19-3-2018.
//

#include "window_creators.h"
#include "camera_window.h"

Window *create_complex_seg_window(const file::Dataset &dataset, const std::string &sample_name, bool webcam) {
    auto dataset_images = file::load_dataset(dataset);
    auto sample = file::load_sample(dataset, sample_name);

    Mat dataset_inputs[dataset_images.size()];
    Mat dataset_masks[dataset_images.size()];
    Mat dataset_masks_inv[dataset_images.size()];

    for (int i = 0; i < dataset_images.size(); ++i) {
        dataset_inputs[i] = dataset_images[i].input;
        dataset_masks[i] = dataset_images[i].label;
        bitwise_not(dataset_masks[i], dataset_masks_inv[i]);
    }

    const float ranges[3] = {256, 256, 256};

    auto positive_hst = image::extract_histogram_3d(dataset_inputs, (uint) dataset_images.size(), ranges, dataset_masks, 128);
    auto env_hst = image::extract_histogram_3d(dataset_inputs, (uint) dataset_images.size(), ranges, dataset_masks_inv,128);
    auto marginal_positive_prob = image::probability_masked_pixels(dataset_masks, (uint) dataset_images.size());

    std::vector<WindowHelper *> helpers = {
            new ComplexSegmentationHelper(positive_hst, env_hst, marginal_positive_prob),
            new SegmentationPatcher(),
    };

    if (webcam) {
        return new CameraWindow("Complex Segmentation (Bayes)", helpers);
    } else {
        return new SampleEvaluationWindow("Complex Segmentation (Bayes)", helpers, sample);
    }
}


Window *create_simple_seg_window(const file::Dataset &dataset, const std::string &sample_name, bool webcam) {
    auto dataset_images = file::load_dataset(dataset);
    auto sample = file::load_sample(dataset, sample_name);

    Mat dataset_inputs[dataset_images.size()];
    Mat dataset_masks[dataset_images.size()];
    Mat dataset_masks_inv[dataset_images.size()];

    for (int i = 0; i < dataset_images.size(); ++i) {
        cvtColor(dataset_images[i].input, dataset_inputs[i], COLOR_BGR2HSV);
        dataset_masks[i] = dataset_images[i].label;
        bitwise_not(dataset_masks[i], dataset_masks_inv[i]);
    }

    const float ranges[3] = {180, 256, 256};

    auto positive_hst = image::extract_histogram(dataset_inputs, (uint) dataset_images.size(), ranges, dataset_masks,
                                                 128);
    std::vector<WindowHelper *> helpers = {
            new SimpleSegmentationHelper(positive_hst),
            new SegmentationPatcher(),
    };

    if (webcam) {
        return new CameraWindow("Simple Segmentation", helpers);
    } else {
        return new SampleEvaluationWindow("Simple Segmentation", helpers, sample);
    }
}
