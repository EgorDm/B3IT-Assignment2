//
// Created by egordm on 14-3-2018.
//

#include "file_processing.h"
#include "defines.h"

using namespace cvision::processing;

bool file::file_exists(const std::string &name) {
    std::ifstream f(name.c_str());
    return f.good();
}

file::ImageSample file::load_sample(const std::string &dataset_name, const std::string &img_name) {
    std::stringstream input_path, label_path;
    input_path << DATASETS_PATH << dataset_name << FILE_SEPARATOR << DATASET_INPUTS << img_name << ".jpg";
    label_path << DATASETS_PATH << dataset_name << FILE_SEPARATOR << DATASET_LABELS << img_name << ".png";
    assert(file_exists(input_path.str()));
    assert(file_exists(label_path.str()));

    ImageSample ret;
    ret.input = cv::imread(input_path.str(), CV_LOAD_IMAGE_COLOR);
    ret.label = cv::imread(label_path.str(), CV_LOAD_IMAGE_GRAYSCALE);

    return ret;
}