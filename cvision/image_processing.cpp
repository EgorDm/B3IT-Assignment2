//
// Created by egordm on 14-3-2018.
//

#include "image_processing.h"

using namespace cvision::processing;

image::Histogram image::extract_histogram(const Mat *src, const unsigned int src_count, const float ranges[],
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

    for (int i = 0; i < channel_count; ++i) normalize(channels[i], channels[i], 1, 0, NORM_L1);

    return image::Histogram(channels, channel_count, end_ranges);
}

Scalar image::extract_dominant_color(const Histogram &histogram) {
    int maxH = 0, maxS = 0, maxV = 0;

    for (int i = 0; i < 128; i++) {
        maxH = histogram[0].at<float>(i) > histogram[0].at<float>(maxH) ? i : maxH;
        maxS = histogram[1].at<float>(i) > histogram[1].at<float>(maxS) ? i : maxS;
        maxV = histogram[2].at<float>(i) > histogram[2].at<float>(maxV) ? i : maxV;
    }
    return ret;
}

float image::Histogram::probability(const double *value) const {
    float ret = 1;
    for (int i = 0; i < channel_count; ++i) ret *= intensity(i, value[i]);
    return ret;
}

float image::Histogram::intensity(int channel, double value) const {
    auto bin = static_cast<int>(std::round((value / ranges[channel]) * bin_count()));
    return channels[channel].at<float>(bin);
}
