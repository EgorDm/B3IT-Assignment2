//
// Created by egordm on 14-3-2018.
//

#include "file_processing.h"
#include "defines.h"
#include <dirent.h>

using namespace cvision::processing;

bool file::file_exists(const std::string &name) {
    std::ifstream f(name.c_str());
    return f.good();
}

file::ImageSample
file::load_sample(const std::string &dataset_name, const std::string &img_name, const bool inverted,
                  const std::string &ext_in, const std::string &ext_lbl) {
    std::stringstream input_path, label_path;
    input_path << DATASETS_PATH << dataset_name << FILE_SEPARATOR << DATASET_INPUTS << img_name << ext_in;
    label_path << DATASETS_PATH << dataset_name << FILE_SEPARATOR << DATASET_LABELS << img_name << ext_lbl;
    assert(file_exists(input_path.str()));
    assert(file_exists(label_path.str()));

    ImageSample ret;
    ret.input = cv::imread(input_path.str(), CV_LOAD_IMAGE_COLOR);
    ret.label = cv::imread(label_path.str(), CV_LOAD_IMAGE_GRAYSCALE);

    if (inverted) bitwise_not(ret.label, ret.label);

    return ret;
}

std::vector<file::ImageSample>
file::load_dataset(const std::string &dataset_name, const bool inverted, const std::string &ext_in,
                   const std::string &ext_lbl) {
    std::stringstream dataset_path;
    dataset_path << DATASETS_PATH << dataset_name << FILE_SEPARATOR << DATASET_INPUTS << FILE_SEPARATOR;
    std::vector<file::ImageSample> ret;

    for (const auto &file : list_dir(dataset_path.str())) {
        if (!file_exists(dataset_path.str() + file)) continue;
        auto dot_pos = file.find_last_of('.');
        if (dot_pos == std::string::npos) continue;
        auto filename = file.substr(0, dot_pos);
        if (filename.length() == 0) continue;

        ret.push_back(load_sample(dataset_name, filename, inverted, ext_in, ext_lbl));
    }

    return ret;
}

std::vector<std::string> file::list_dir(const std::string &path) {
    DIR *dir;
    struct dirent *ent;
    std::vector<std::string> ret;

    if ((dir = opendir(path.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            ret.emplace_back(ent->d_name);
        }
        closedir(dir);
    } else {
        return ret;
    }

    return ret;
}

