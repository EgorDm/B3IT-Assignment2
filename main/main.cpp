#include <iostream>
#include "../cvision/file_processing.h"
#include "../cvision/image_processing.h"
#include "window_creators.h"
#include "detection_helper.h"
#include "segmentation_helper.h"
#include "../cvision/visualization.h"
#include "gesture_recognition_helper.h"

using namespace cv;
using namespace cvision::processing;

//auto sample = file::load_sample(pratheepan, "chenhao0017me9");
//    auto sample = file::load_sample(hgr1, "U_P_hgr1_id08_3");
//    auto sample = file::load_sample(hgr1, "W_P_hgr1_id03_10");
//    auto sample = file::load_sample(hgr1, "3_P_hgr1_id01_1");
//    auto sample = file::load_sample(hgr1, "Z_P_hgr1_id01_3");
//    auto sample = file::load_sample(hgr1, "A_P_hgr1_id06_1");
//    auto sample = file::load_sample(hgr1, "T_P_hgr1_id10_3");

#define SOURCE_TYPE_WEBCAM 0
#define SOURCE_TYPE_VIDEO 1
#define SOURCE_TYPE_IMAGE 2

//#define USE_SOURCE std::tuple<file::Dataset, std::string>(own, "egordmImage-0005")
//#define USE_SOURCE std::tuple<file::Dataset, std::string>(videos, "2018-04-09 12-36-30")
//#define USE_SOURCE std::tuple<file::Dataset, std::string>(hgr1, "W_P_hgr1_id03_10")
#define USE_SOURCE std::tuple<file::Dataset, std::string>(videos, "2018-04-09 12-36-30")
#define USE_SOURCE_TYPE SOURCE_TYPE_VIDEO
#define USE_EVALUATE false
#define USE_MASK false
#define USE_OVERRIDE_MASK false
#define USE_FACE_DETECTION false

int main() {
    file::Dataset pratheepan("Pratheepan", ".jpg", ".png", false);
    file::Dataset hgr1("hgr1", ".jpg", ".bmp", true);
    file::Dataset own("own", ".jpg", ".png", false);
    file::Dataset pom("POM", ".jpg", ".png", false);
    file::Dataset videos("videos", ".mp4", ".mp4", false);

    file::Dataset datasets[] = {hgr1, own, pom};
    auto data = load_complex_dataset_data(datasets, 3);

    std::vector<WindowHelper *> helpers;

#if USE_SOURCE_TYPE == SOURCE_TYPE_IMAGE

    InputHelper *input = new InputHelper(std::get<0>(USE_SOURCE).load_input(std::get<1>(USE_SOURCE)), true);
#if USE_MASK
    InputHelper *label = new InputHelper(std::get<0>(USE_SOURCE).load_label(std::get<1>(USE_SOURCE)), USE_OVERRIDE_MASK);
#endif
#elif USE_SOURCE_TYPE == SOURCE_TYPE_VIDEO
    std::stringstream ss;
    ss << DATASETS_PATH << std::get<0>(USE_SOURCE).name << FILE_SEPARATOR << "inputs" << FILE_SEPARATOR << std::get<1>(USE_SOURCE) << ".mp4";
    InputHelper *input = new InputHelper(ss.str(), true);
#if USE_MASK
    ss.str("");
    ss << DATASETS_PATH << std::get<0>(USE_SOURCE).name << FILE_SEPARATOR << "labels" << FILE_SEPARATOR << std::get<1>(USE_SOURCE) << ".mp4";
    InputHelper *label = new InputHelper(ss.str(), USE_OVERRIDE_MASK);
#endif
#else
    InputHelper *input = new InputHelper(0);
#endif

    helpers.push_back(input);
#if USE_MASK
    helpers.push_back(label);
#endif

    auto patcher = new SegmentationPatcher();
#if !USE_OVERRIDE_MASK
    helpers.push_back(new ComplexSegmentationHelper(data.positive_hst, data.env_hst, data.marginal_positive_prob));
#endif
    helpers.push_back(patcher);
#if USE_MASK && USE_EVALUATE
    helpers.push_back(new CFMatrixHelper(patcher->mask, label->frame));
#endif
#if USE_FACE_DETECTION
    auto face_detect = new FaceDetectorHelper();
    helpers.push_back(face_detect);
    auto hand_detector = new HandDetectorHelper(patcher->mask, &face_detect->faces);
#else
    auto hand_detector = new HandDetectorHelper(patcher->mask);
#endif
    helpers.push_back(hand_detector);
    auto hand_tracker = new HandTrackingHelper(hand_detector);
    helpers.push_back(hand_tracker);
    helpers.push_back(new GestureDetectionHelper({ "Triangle", "X", "Rectangle", "Circle" }, hand_tracker));

    auto window = new Window("Complex Segmentation (Bayes)", input, helpers);
    window->init();


#if USE_SOURCE_TYPE == SOURCE_TYPE_VIDEO
    while(true) {
        window->show();
        if(cv::waitKey(1) >= 0 || input->frame.empty()) break;
    }
#else
    window->show();
#endif

    waitKey(0);
    delete window;
    return 0;
}