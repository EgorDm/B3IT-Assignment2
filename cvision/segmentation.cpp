//
// Created by egordm on 15-3-2018.
//

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
    M_Assert(dynamic_cast<const Histogram3D*>( target_histogram ) != NULL, "Histogram must be a 3d histogram");

    Mat binned_src = src / (257.0 / target_histogram->bin_count()); // 257 so it doesnt round up
    auto target_histogram_3d = (Histogram3D*) target_histogram;
    auto env_histogram_3d = (Histogram3D*) environment_histogram;

    Mat ppos = Mat(src.size(), CV_32FC1);
    Mat pneg = Mat(src.size(), CV_32FC1);

    const cv::Vec3b *read_row;
    float *write_ppos;
    float *write_pneg;

    for (int j = 0; j < src.rows; ++j) {
        read_row = binned_src.ptr<const cv::Vec3b>(j);
        write_ppos = ppos.ptr<float>(j);
        write_pneg = pneg.ptr<float>(j);

        for (int i = 0; i < src.cols; ++i) {
            const uchar *vals = read_row[i].val;
            int bins[target_histogram_3d->channel_count];
            for(int k = 0; k < target_histogram_3d->channel_count; ++k) bins[k] = vals[k];

            write_ppos[i] = target_histogram_3d->histogram.at<float>(bins);
            write_pneg[i] = env_histogram_3d->histogram.at<float>(bins);
        }
    }

    ppos = ppos * positive_probability;
    Mat pbayes = (ppos / (ppos + (pneg * (1.0 - positive_probability))));
    Mat thresh_res, ret;
    cv::threshold(pbayes, thresh_res, threshold, 255, THRESH_BINARY);
    thresh_res.convertTo(ret, CV_8U);
    return ret;
}

cv::Mat
segmentation::clean_segmentation(const cv::Mat &src, const int &ksize, const int &close_ksize, const int &blur_ksize) {
    Mat ret;
    src.copyTo(ret);

    // Morphological Processing
    if (ksize > 3) {
        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(ksize, ksize));
        morphologyEx(src, ret, MORPH_ERODE, kernel, Point(-1, -1), 2);
        morphologyEx(ret, ret, MORPH_DILATE, kernel, Point(-1, -1), 2);
    }

    if (close_ksize > 3) {
        Mat close_element = getStructuringElement(MORPH_ELLIPSE, Size(close_ksize, close_ksize), cv::Point(2, 2));
        morphologyEx(ret, ret, MORPH_CLOSE, close_element, Point(-1, -1), 2);
    }

    if(blur_ksize > 0 && blur_ksize % 2 != 0) GaussianBlur(ret, ret, Size(blur_ksize, blur_ksize), 4);

    return ret;
}
