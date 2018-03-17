//
// Created by egordm on 14-3-2018.
//

#ifndef B3ITASSIGNMENT2_IMAGE_PROCESSING_H
#define B3ITASSIGNMENT2_IMAGE_PROCESSING_H

#include <opencv2/opencv.hpp>
#include "defines.h"

using namespace cv;

namespace cvision { namespace processing { namespace image {
    struct Histogram {
        const int channel_count;
        const float *ranges;
        const cv::Mat *channels;

        Histogram() : channel_count(0), ranges(nullptr), channels(nullptr) {};

        explicit Histogram(cv::Mat *channels, const int channel_count, const float *channel_ranges)
                : channel_count(channel_count), channels(channels), ranges(channel_ranges) {
            for (int i = 0; i < channel_count; ++i) {
                assert(sum(channels[i])[0] - 1 < EPSILON);
            }
        }

        virtual ~Histogram() {
            delete[] channels;
            delete[] ranges;
        }

        float probability(const double value[]) const;

        float intensity(int channel, double value) const;

        int bin_count() const { return channels[0].rows; }

        const cv::Mat &operator[](const unsigned int i) const { return channels[i]; }
    };

    Histogram *extract_histogram(const Mat *src, const unsigned int src_count, const float ranges[],
                                const Mat *mask, const int binSize = 128);

    float probability_masked_pixels(const Mat *mask, const unsigned int src_count);

    double* extract_dominant_color(const Histogram &histogram);
}}}

#endif //B3ITASSIGNMENT2_IMAGE_PROCESSING_H
