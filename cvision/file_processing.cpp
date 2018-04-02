//
// Created by egordm on 14-3-2018.
//

#include "file_processing.h"
#include "defines.h"
#include "macros.h"
#include <dirent.h>

using namespace cvision::processing;

bool file::file_exists(const std::string &name) {
    std::ifstream f(name.c_str());
    return f.good();
}

cv::Mat file::load_image(const std::vector<std::string> &path, const std::string &ext, const int &load_type) {
    M_Assert(!path.empty(), "Path must not be empty!");
    std::stringstream file_path;
    file_path << DATASETS_PATH << path[0];
    for(int i = 1; i < path.size(); ++i) file_path << FILE_SEPARATOR << path[i];
    file_path << ext;
    M_Assert(file_exists(file_path.str()), file_path.str().c_str());

    cv::Mat ret;
    ret = cv::imread(file_path.str(), load_type);
    return ret;
}

file::ImageSample file::load_sample(const Dataset &dataset, const std::string &img_name) {
    ImageSample ret;
    ret.input = dataset.load_input(img_name);
    ret.label = dataset.load_label(img_name);
    return ret;
}

std::vector<file::ImageSample> file::load_dataset(const Dataset &dataset) {
    std::stringstream dataset_path;
    dataset_path << DATASETS_PATH << dataset.name << FILE_SEPARATOR << DATASET_INPUTS << FILE_SEPARATOR;
    std::vector<file::ImageSample> ret;

    for (const auto &file : list_dir(dataset_path.str())) {
        if (!file_exists(dataset_path.str() + file)) continue;
        auto dot_pos = file.find_last_of('.');
        if (dot_pos == std::string::npos || dot_pos == file.size() - 1) continue;
        auto filename = file.substr(0, dot_pos);
        if (filename.length() == 0) continue;

        ret.push_back(load_sample(dataset, filename));
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
