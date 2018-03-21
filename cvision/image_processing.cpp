//
// Created by egordm on 14-3-2018.
//

#include "image_processing.h"

using namespace cvision::processing;

image::Histogram *image::extract_histogram(const Mat *src, const unsigned int src_count, const float ranges[],
                                          const Mat *mask, const int binSize) {
    int channel_count = src[0].channels();
    const float *ranges_hist[channel_count];
    auto *end_ranges = new float[channel_count];

    for (int i = 0; i < channel_count; ++i) {
        const float range_partial[] = {0, ranges[i]};
        ranges_hist[i] = range_partial;
        end_ranges[i] = ranges[i];
    }

    /* This needs some explaination.
     We are splitting channels then calculating histogram because then we will get a bin_size x channel_size
     array. Meanwhile doing it the traditional way by doing all channels at once gives bin_size ^ channel_size
     array. I dont get the logic but sounds like the worst deal in history!*/
    auto *channels = new Mat[3];

    for (int i = 0; i < src_count; ++i) {
        Mat hsv_planes[channel_count];
        split(src[i], hsv_planes);

        for (int j = 0; j < channel_count; ++j) {
            calcHist(&hsv_planes[j], 1, 0, mask[i], channels[j], 1, &binSize, &ranges_hist[j], true, true);
            // TODO: can we overflow? Should we therefore normalize here too?
        }
    }

    auto ret = new image::Histogram(channels, channel_count, end_ranges);
    ret->normalize();

    return ret;
}

double* image::extract_dominant_color(const image::Histogram &histogram) {
    int maxes[histogram.channel_count];
    auto *ret = new double[histogram.channel_count];

    for(int j = 0; j < histogram.channel_count; ++j) maxes[j] = 0;

    for (int i = 0; i < histogram.bin_count(); i++) {
        for(int j = 0; j < histogram.channel_count; ++j) {
            maxes[j] = histogram[j].at<float>(i) > histogram[j].at<float>(maxes[j]) ? i : maxes[j];
        }
    }

    for(int j = 0; j < histogram.channel_count; ++j) {
        ret[j] = (histogram.ranges[j] / histogram.bin_count()) * maxes[j];
    }

    return ret;
}

float image::probability_masked_pixels(const Mat *mask, const unsigned int src_count) {
    float ret = 0;
    for(unsigned int i = 0; i < src_count; ++i) {
        double masked_px  = countNonZero(mask[i]);
        double all_px = (mask[i].rows * mask[i].cols);
        ret += (masked_px / all_px) / src_count;
    }
    return ret;
}

float image::Histogram::probability(HistColor value) const {
    float ret = 1;
    for (int i = 0; i < channel_count; ++i) ret *= intensity(i, value[i]);
    return ret;
}

float image::Histogram::intensity(int channel, double value) const {
    return channels[channel].at<float>(bin_pos(value, channel));
}

int image::Histogram::bin_pos(const double value, const int channel) const {
    return static_cast<int>(std::round((value / ranges[channel]) * bin_count()));
}

void image::Histogram::update_probability(image::HistColor value, float increment) {
    for(int i = 0; i < channel_count; ++i) {
        auto bin = bin_pos(value[i], i);
        channels[i].at<float>(bin) += increment;
    }
}

void image::Histogram::normalize() {
    for (int i = 0; i < channel_count; ++i) cv::normalize(channels[i], channels[i], 1, 0, NORM_L1);
}
