//
// Created by egordm on 15-3-2018.
//

#include <c++/7.3.0/chrono>
#include "segmentation.h"
#include "visualization.h"
#include "evaluation.h"

using namespace cvision::algorithms;
using namespace cvision::evaluation;

cv::Mat
segmentation::simple_segmentation(const cv::Mat &src, const cv::Scalar &low, const cv::Scalar &high) {
    Mat prediction_mask;
    inRange(src, low, high, prediction_mask);
    return prediction_mask;

}

cv::Mat segmentation::complex_segmentation(const cv::Mat &src, const Histogram *target_histogram,
                                           const Histogram *environment_histogram, const float threshold,
                                           const float &positive_probability) {
    M_Assert(target_histogram->ranges[0] == 256, "Histogram must be RGB color space histogram");
    M_Assert(dynamic_cast<const Histogram3D *>( target_histogram ) != NULL, "Histogram must be a 3d histogram");

    Mat binned_src = src / (257.0 / target_histogram->bin_count()); // 257 so it doesnt round up
    auto target_histogram_3d = (Histogram3D *) target_histogram;
    auto env_histogram_3d = (Histogram3D *) environment_histogram;

    Mat ret = Mat::zeros(src.size(), CV_8U);

    const cv::Vec3b *read_row;
    char *write_ret;

    read_row = binned_src.ptr<const cv::Vec3b>(0, 0);
    write_ret = ret.ptr<char>(0, 0);
    parallel_for_(Range(0, binned_src.rows * binned_src.cols), [&](const Range &range) {
        for (int r = range.start; r < range.end; ++r) {
            const uchar *vals = read_row[r].val;
            int bins[target_histogram_3d->channel_count];
            for (int k = 0; k < target_histogram_3d->channel_count; ++k) bins[k] = vals[k];

            double ppos = target_histogram_3d->histogram.at<float>(bins) * positive_probability;
            double pneg = env_histogram_3d->histogram.at<float>(bins) * (1.0 - positive_probability);
            double pbayes = ppos / (ppos + pneg);

            if(pbayes > threshold) write_ret[r] = (char)(0xFF);
        }
    });

    return ret;
}

cv::Mat
segmentation::clean_segmentation(const cv::Mat &src, const int &ksize, const int &close_ksize, const int &blur_ksize) {
    Mat ret;
    src.copyTo(ret);

    // Morphological Processing
    if (ksize > 0) {
        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size( 2*ksize + 1, 2*ksize+1 ), Point(ksize, ksize));
        morphologyEx(src, ret, MORPH_ERODE, kernel, Point(-1, -1), 2);
        morphologyEx(ret, ret, MORPH_DILATE, kernel, Point(-1, -1), 2);
    }

    if (close_ksize > 0) {
        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size( 2*close_ksize + 1, 2*close_ksize+1 ), Point(close_ksize, close_ksize));
        morphologyEx(ret, ret, MORPH_CLOSE, kernel, Point(-1, -1), 2);
    }

    if (blur_ksize > 0) GaussianBlur(ret, ret, Size(blur_ksize * 2, blur_ksize * 2), 4);

    return ret;
}
