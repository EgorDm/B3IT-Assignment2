//
// Created by egordm on 19-3-2018.
//

#include "window_creators.h"
#include "camera_window.h"
#include "gesture_detection_helper.h"

Window *create_complex_seg_window(const file::Dataset &dataset, const std::string &sample_name, bool webcam,
                                  const std::string &save_name) {
    std::stringstream ss;
    ss << SAVES_PATH << save_name << ".save";

    float marginal_positive_prob;
    image::Histogram3D* positive_hst;
    image::Histogram3D* env_hst;

    if(cvision::processing::file::file_exists(ss.str())) {
        std::cout << "Reading " << ss.str() << std::endl;
        FileStorage fs;
        fs.open(ss.str(), FileStorage::READ);
        M_Assert(fs.isOpened(), "Could not open the save file");

        marginal_positive_prob = (float) fs["marginal_positive_prob"];

        env_hst = new image::Histogram3D();
        env_hst->read(fs["env_hst"]);

        positive_hst = new image::Histogram3D();
        positive_hst->read(fs["positive_hst"]);
    } else {
        auto dataset_images = file::load_dataset(dataset);

        Mat dataset_inputs[dataset_images.size()];
        Mat dataset_masks[dataset_images.size()];
        Mat dataset_masks_inv[dataset_images.size()];

        for (int i = 0; i < dataset_images.size(); ++i) {
            dataset_inputs[i] = dataset_images[i].input;
            dataset_masks[i] = dataset_images[i].label;
            bitwise_not(dataset_masks[i], dataset_masks_inv[i]);
        }

        const float ranges[3] = {256, 256, 256};
        positive_hst = image::extract_histogram_3d(dataset_inputs, (uint) dataset_images.size(), ranges, dataset_masks, 128);
        env_hst = image::extract_histogram_3d(dataset_inputs, (uint) dataset_images.size(), ranges, dataset_masks_inv,128);
        marginal_positive_prob = image::probability_masked_pixels(dataset_masks, (uint) dataset_images.size());

        {
            std::cout << "Writing " << ss.str() << std::endl;
            FileStorage fs(ss.str(), FileStorage::WRITE);
            fs << "marginal_positive_prob" << marginal_positive_prob;
            fs << "env_hst";
            env_hst->write(fs);
            fs << "positive_hst";
            positive_hst->write(fs);

            fs.release();
        }
    }

    auto patcher =  new SegmentationPatcher();

    std::vector<WindowHelper *> helpers = {
            new ComplexSegmentationHelper(positive_hst, env_hst, marginal_positive_prob),
            patcher,
            new HandDetectorHelper(patcher->mask)
    };

    if (webcam) {
        return new CameraWindow("Complex Segmentation (Bayes)", helpers);
    } else {
        auto sample = file::load_sample(dataset, sample_name);
        return new SampleEvaluationWindow("Complex Segmentation (Bayes)", helpers, sample);
    }
}


Window *create_simple_seg_window(const file::Dataset &dataset, const std::string &sample_name, bool webcam,
                                 const std::string &save_name) {
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

    auto positive_hst = image::extract_histogram(dataset_inputs, (uint) dataset_images.size(), ranges, dataset_masks, 128);
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
