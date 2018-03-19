//
// Created by egordm on 14-3-2018.
//

#ifndef B3ITASSIGNMENT2_IMAGE_PROCESSING_H
#define B3ITASSIGNMENT2_IMAGE_PROCESSING_H

#include <opencv2/opencv.hpp>
#include "defines.h"
#include "macros.h"

using namespace cv;

namespace cvision { namespace processing { namespace image {
    /**
     * Struct that contains a histogram and operantions that come along with it
     */
    struct Histogram {
        const int channel_count;
        const float *ranges;
        const cv::Mat *channels;

        Histogram() : channel_count(0), ranges(nullptr), channels(nullptr) {};

        /**
         * Create histogram with separate channel histograms. Channels should be normalized!
         * @param channels
         * @param channel_count
         * @param channel_ranges the value range the bins split. 256 for rgb
         */
        explicit Histogram(cv::Mat *channels, const int channel_count, const float *channel_ranges)
                : channel_count(channel_count), channels(channels), ranges(channel_ranges) {
            for (int i = 0; i < channel_count; ++i) {
                M_Assert(sum(channels[i])[0] - 1 < EPSILON, "Frequency sum of a channel should be equal to 1. Please normalize.");
            }
        }

        virtual ~Histogram() {
            delete[] channels;
            delete[] ranges;
        }

        /**
         * Get raw probability of a color occuring
         * @param value
         * @return
         */
        float probability(const double value[]) const;

        /**
         * Get histogram intensity
         * @param channel
         * @param value
         * @return
         */
        float intensity(int channel, double value) const;

        /**
         * Get count of bins the channels have
         * @return
         */
        int bin_count() const { return channels[0].rows; }

        /**
         * Channel accessor
         * @param i
         * @return
         */
        const cv::Mat &operator[](const unsigned int i) const { return channels[i]; }
    };

    /**
     * Extract a histogram given a collection of images & masks.
     * @param src
     * @param src_count
     * @param ranges Max values each channel goes to. For hsv first channel should be 180
     * @param mask
     * @param binSize
     * @return
     */
    Histogram *extract_histogram(const Mat *src, const unsigned int src_count, const float ranges[],
                                const Mat *mask, const int binSize = 128);

    /**
     * Get marginal probability of a white pixel occuring in the image(s)
     * Basically occurence/total_pixels
     * @param mask
     * @param src_count
     * @return
     */
    float probability_masked_pixels(const Mat *mask, const unsigned int src_count);

    /**
     * Extract the dominant/most occurent pixel in the histogram
     * @param histogram
     * @return
     */
    double* extract_dominant_color(const Histogram &histogram);
}}}

#endif //B3ITASSIGNMENT2_IMAGE_PROCESSING_H
