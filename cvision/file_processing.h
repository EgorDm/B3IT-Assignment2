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
    ImageSample load_sample(const std::string &dataset_name, const std::string &img_name, const bool inverted = false,
                            const std::string &ext_in = ".jpg", const std::string &ext_lbl = ".png");


    /**
     * Load a whole dataset. This is really bad. Everything in memory. Mayby better do one by one
     * @param dataset_name
     * @param inverted
     * @param ext_in
     * @param ext_lbl
     * @return
     */
    std::vector<file::ImageSample> load_dataset(const std::string &dataset_name, const bool inverted = false,
                                                const std::string &ext_in = ".jpg",  const std::string &ext_lbl = ".png");

    /**
     * Get all files in directory
     * @param path
     * @return
     */
    std::vector<std::string> list_dir(const std::string &path);
}}};


#endif //B3ITASSIGNMENT2_FILE_PROCESSING_H
