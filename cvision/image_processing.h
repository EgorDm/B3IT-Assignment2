//
// Created by egordm on 14-3-2018.
//

#ifndef B3ITASSIGNMENT2_IMAGE_PROCESSING_H
#define B3ITASSIGNMENT2_IMAGE_PROCESSING_H

#include <opencv2/opencv.hpp>
#include <utility>
#include "defines.h"
#include "macros.h"

using namespace cv;

namespace cvision { namespace processing { namespace image {
    using HistColor = cv::Vec3d;

    struct Histogram {
        int channel_count;
        float *ranges;

        Histogram(const int channel_count, float *ranges) : channel_count(channel_count), ranges(ranges) {}

        /**
        * Get raw probability of a color occuring
        * @param value
        * @return
        */
        virtual float probability(HistColor value) const = 0;

        /**
         * Get histogram intensity
         * @param channel
         * @param value
         * @return
         */
        virtual float intensity(int channel, double value) const = 0;

        /**
         * Get count of bins the channels have
         * @return
         */
        virtual int bin_count() const = 0;

        /**
        * Translate color value to a bin position
        * @param value
        * @param channel
        * @return
        */
        virtual int bin_pos(double value, int channel) const {
            return static_cast<int>(std::round((value / ranges[channel]) * bin_count()));
        };

        /**
         * Change probability of a color occuring
         * @param value
         * @param multiplier
         */
        virtual void update_probability(HistColor value, double probability) = 0;

        /*
         * Normalize the channels to sum up to one
         */
        virtual void normalize() = 0;

        virtual HistColor dominat_val() const = 0;

        virtual void write(cv::FileStorage &fs) const = 0;

        virtual void read(const cv::FileNode &node) = 0;
    };

    /**
     * Struct that contains a histogram and operantions that come along with it
     */
    struct HistogramFlat : public Histogram {
        cv::Mat *channels;

        HistogramFlat() : Histogram(0, nullptr), channels(nullptr) {};

        /**
        * Create histogram with separate channel histograms. Channels should be normalized!
        * @param channels
        * @param channel_count
        * @param channel_ranges the value range the bins split. 256 for rgb
        */
        HistogramFlat(const int channel_count, float *ranges, Mat *channels)
                : Histogram(channel_count, ranges), channels(channels) {}

        ~HistogramFlat() {
            delete[] channels;
            delete[] ranges;
        }

        float probability(HistColor value) const override;

        float intensity(int channel, double value) const override {
            return channels[channel].at<float>(bin_pos(value, channel));
        }

        int bin_count() const override { return channels[0].rows; }

        void update_probability(HistColor value, double probability) override;

        void normalize() override {
            for (int i = 0; i < channel_count; ++i) cv::normalize(channels[i], channels[i], 1, 0, NORM_L1);
        }

        HistColor dominat_val() const override;

        void write(cv::FileStorage &fs) const override;

        void read(const cv::FileNode &node) override;
    };

    struct Histogram3D : public Histogram {
        cv::Mat histogram;

        Histogram3D() : Histogram(0, nullptr) {}

        Histogram3D(const int channel_count, float *ranges, const Mat &histogram)
                : Histogram(channel_count, ranges), histogram(std::move(histogram)) {}

        ~Histogram3D() {
            delete[] ranges;
        }

        float probability(HistColor value) const override;

        float intensity(int channel, double value) const override;

        int bin_count() const override { return histogram.size[0]; }

        void update_probability(HistColor value, double probability) override;

        void normalize() override {
            cv::normalize(histogram, histogram, 1, 0, NORM_L1);
        }

        HistColor dominat_val() const override;

        void write(cv::FileStorage &fs) const override;

        void read(const cv::FileNode &node) override;
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
    HistogramFlat *extract_histogram(const Mat *src, const unsigned int src_count, const float ranges[],
                                     const Mat *mask, const int binSize = 128);

    Histogram3D *extract_histogram_3d(const Mat *src, const unsigned int src_count, const float ranges[],
                                      const Mat *mask, const int binSize = 128);

    /**
     * Get marginal probability of a white pixel occuring in the image(s)
     * Basically occurence/total_pixels
     * @param mask
     * @param src_count
     * @return
     */
    float probability_masked_pixels(const Mat *mask, const unsigned int src_count);
}}}

#endif //B3ITASSIGNMENT2_IMAGE_PROCESSING_H
