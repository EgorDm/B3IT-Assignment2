//
// Created by egordm on 19-3-18.
//

#ifndef B3IT_ASSIGNMENT2_SEGMENTATION_HELPER_H
#define B3IT_ASSIGNMENT2_SEGMENTATION_HELPER_H

#include "../cvision/image_processing.h"
#include "../cvision/file_processing.h"
#include "../cvision/evaluation.h"
#include "window.h"

using namespace cvision::processing;

class ComplexSegmentationHelper : public WindowHelper {
private:
    Mat frame;
    const image::Histogram *positive_hst;
    const image::Histogram *negative_hst;
public:
    const float positive_marginal_prob;
    int threshold, preblur, ed_size, close_size, postblur, marginal_weight;
    float pskin{}, penv{}, pbayes{};
    Vec3b colour;

    ComplexSegmentationHelper(const image::Histogram *positive_hst, const image::Histogram *negative_hst,
                              const float positive_marginal_prob)
            : positive_hst(positive_hst), negative_hst(negative_hst), positive_marginal_prob(positive_marginal_prob),
              threshold(50), preblur(4), ed_size(4), close_size(4), postblur(4), marginal_weight(100) {}

    std::vector<Statistic> get_statistics() override;

    Mat draw(const cv::Mat &src, const cv::Mat &original) override;

    std::vector<Trackbar> get_trackbars() override;

    bool on_click(int x, int y, bool rb) override;

    std::string get_probability();

    std::string get_probability2();
};

class SimpleSegmentationHelper : public WindowHelper {
private:
    const image::Histogram *skin_histogram;
public:
    int alpha, beta, gamma;

    explicit SimpleSegmentationHelper(const image::Histogram *skin_histogram)
            : skin_histogram(skin_histogram), alpha(4), beta(20), gamma(5) {}

    std::vector<Trackbar> get_trackbars() override;

    Mat draw(const cv::Mat &src, const cv::Mat &original) override;
};

class SegmentationPatcher : public WindowHelper {
public:
    Mat mask;
    int ed_size, close_size, postblur;

    SegmentationPatcher() : ed_size(2), close_size(0), postblur(0) {}

    std::vector<Trackbar> get_trackbars() override;

    Mat draw(const cv::Mat &src, const cv::Mat &original) override;
};


class CFMatrixHelper : public WindowHelper {
private:
    const cv::Mat &input_mask;
    const cv::Mat &label_mask;
public:
    cvision::evaluation::ConfusionMatrixResults cf_results{};

    CFMatrixHelper(const cv::Mat &input_mask, const cv::Mat &label_mask)
            : input_mask(input_mask), label_mask(label_mask) {}

    cv::Mat draw(const cv::Mat &src, const cv::Mat &original) override;

    std::string get_recall();

    std::string get_precision();

    std::string get_score();

    std::string get_error();

    std::vector<Statistic> get_statistics() override;
};


#endif //B3IT_ASSIGNMENT2_SEGMENTATION_HELPER_H
