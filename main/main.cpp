#include <iostream>
#include "../cvision/file_processing.h"
#include "../cvision/image_processing.h"
#include "complex_segmentation_window.h"

using namespace cv;
using namespace cvision::processing;

int main() {
    auto dataset = file::load_dataset("hgr1", true, ".jpg", ".bmp");
    auto sample = file::load_sample("hgr1", "4_P_hgr1_id01_3", true, ".jpg", ".bmp");

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

    ComplexSegmentationWindow window(positive_hst, env_hst, marginal_positive_prob, sample);
    window.show();

    waitKey(0);
    return 0;
}