//
// Created by egordm on 14-3-2018.
//

#ifndef B3ITASSIGNMENT2_FILE_PROCESSING_H
#define B3ITASSIGNMENT2_FILE_PROCESSING_H

#include <string>
#include <opencv2/opencv.hpp>

namespace cvision { namespace processing { namespace file {
    struct ImageSample {
        cv::Mat input;
        cv::Mat label;
    };

    /**
     * Check wether a file exists
     * @param name
     * @return
     */
    bool file_exists(const std::string &name);

    /**
     * Load a sample image from a dataset
     * @param dataset_name
     * @param img_name
     * @return
     */
    ImageSample load_sample(const std::string &dataset_name, const std::string &img_name);
}}};


#endif //B3ITASSIGNMENT2_FILE_PROCESSING_H
