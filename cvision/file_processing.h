//
// Created by egordm on 14-3-2018.
//

#ifndef B3ITASSIGNMENT2_FILE_PROCESSING_H
#define B3ITASSIGNMENT2_FILE_PROCESSING_H

#include <string>
#include <opencv2/opencv.hpp>

namespace cvision { namespace processing { namespace file {
    struct Dataset {
        const std::string name;
        const std::string ext_in;
        const std::string ext_lbl;
        const bool inverted;

        Dataset(const std::string &name, const std::string &ext_in, const std::string &ext_lbl, const bool inverted)
                : name(name), ext_in(ext_in), ext_lbl(ext_lbl), inverted(inverted) {}
    };

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
    ImageSample load_sample(const Dataset &dataset, const std::string &img_name);


    /**
     * Load a whole dataset. This is really bad. Everything in memory. Mayby better do one by one
     * @param dataset_name
     * @param inverted
     * @param ext_in
     * @param ext_lbl
     * @return
     */
    std::vector<file::ImageSample> load_dataset(const Dataset &dataset);

    /**
     * Get all files in directory
     * @param path
     * @return
     */
    std::vector<std::string> list_dir(const std::string &path);
}}};


#endif //B3ITASSIGNMENT2_FILE_PROCESSING_H
