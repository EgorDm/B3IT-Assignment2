//
// Created by egordm on 14-3-2018.
//

#include "image_processing.h"

using namespace cvision::processing;

image::HistogramFlat *image::extract_histogram(const Mat *src, const unsigned int src_count, const float ranges[],
                                               const Mat *mask, const int binSize) {
    int channel_count = src[0].channels();
    const float *ranges_hist[channel_count];
    auto *end_ranges = new float[channel_count];

    for (int i = 0; i < channel_count; ++i) {
        const float range_partial[] = {0, ranges[i]};
        ranges_hist[i] = range_partial;
        end_ranges[i] = ranges[i];
    }

    auto *channels = new Mat[3];

    for (int i = 0; i < src_count; ++i) {
        Mat hsv_planes[channel_count];
        split(src[i], hsv_planes);

        for (int j = 0; j < channel_count; ++j) {
            calcHist(&hsv_planes[j], 1, 0, mask[i], channels[j], 1, &binSize, &ranges_hist[j], true, true);
            // TODO: can we overflow? Should we therefore normalize here too?
        }
    }

    auto ret = new image::HistogramFlat(channel_count, end_ranges, channels);
    ret->normalize();

    return ret;
}


image::Histogram3D *
image::extract_histogram_3d(const Mat *src, const unsigned int src_count, const float *ranges, const Mat *mask,
                            const int binSize) {
    int channel_count = src[0].channels();
    const float *ranges_hist[channel_count];
    auto *end_ranges = new float[channel_count];

    for (int i = 0; i < channel_count; ++i) {
        const float range_partial[] = {0, ranges[i]};
        ranges_hist[i] = range_partial;
        end_ranges[i] = ranges[i];
    }

    const int channels[] = {0, 1, 2};
    const int binsizes[] = {binSize, binSize, binSize};
    float ranges0[] = {0, ranges[0]};
    float ranges1[] = {0, ranges[1]};
    float ranges2[] = {0, ranges[2]};
    const float *zranges[] = {ranges0, ranges1, ranges2};

    Mat hist;
    for (int i = 0; i < src_count; ++i) {
        calcHist(&src[i], 1, channels, mask[i], hist, 3, binsizes, zranges, true, true);
    }

    auto ret = new image::Histogram3D(channel_count, end_ranges, hist);
    ret->normalize();
    return ret;
}

float image::probability_masked_pixels(const Mat *mask, const unsigned int src_count) {
    float ret = 0;
    for (unsigned int i = 0; i < src_count; ++i) {
        double masked_px = countNonZero(mask[i]);
        double all_px = (mask[i].rows * mask[i].cols);
        ret += (masked_px / all_px) / src_count;
    }
    return ret;
}

float image::HistogramFlat::probability(HistColor value) const {
    float ret = 1;
    for (int i = 0; i < channel_count; ++i) ret *= intensity(i, value[i]);
    return ret;
}

void image::HistogramFlat::update_probability(image::HistColor value, float increment) {
    for (int i = 0; i < channel_count; ++i) {
        auto bin = bin_pos(value[i], i);
        channels[i].at<float>(bin) = std::fmaxf(channels[i].at<float>(bin) + increment, 0);
    }
}

image::HistColor image::HistogramFlat::dominat_val() const {
    int maxes[channel_count];
    HistColor ret;

    for (int j = 0; j < channel_count; ++j) maxes[j] = 0;

    for (int i = 0; i < bin_count(); i++) {
        for (int j = 0; j < channel_count; ++j) {
            maxes[j] = channels[j].at<float>(i) > channels[j].at<float>(maxes[j]) ? i : maxes[j];
        }
    }

    for (int j = 0; j < channel_count; ++j) ret[j] = (ranges[j] / bin_count()) * maxes[j];

    return ret;
}

void image::HistogramFlat::write(cv::FileStorage &fs) const {
    fs << "{";
    fs << "channel_count" << channel_count;

    fs << "ranges" << "[";
    for (int i = 0; i < channel_count; ++i) fs << ranges[i];
    fs << "]";

    fs << "channels" << "[";
    for (int i = 0; i < channel_count; ++i) fs << channels[i];
    fs << "]";

    fs << "}";
}

void image::HistogramFlat::read(const cv::FileNode &node) {
    channel_count = (int) node["channel_count"];

    FileNode ranges_seq = node["ranges"];
    M_Assert(ranges_seq.type() == FileNode::SEQ, "Ranges should be a sequence");
    ranges = new float[channel_count];
    int is = 0;
    for (auto it = ranges_seq.begin(); it != ranges_seq.end(); ++it, ++is) ranges[is] = (float) *it;

    FileNode channels_seq = node["channels"];
    M_Assert(channels_seq.type() == FileNode::SEQ, "Channels should be a sequence");
    channels = new Mat[channel_count];
    is = 0;
    for (auto it = channels_seq.begin(); it != channels_seq.end(); ++it, ++is) (*it) >> channels[is];
}

float image::Histogram3D::probability(image::HistColor value) const {
    int bins[channel_count];
    for (int i = 0; i < channel_count; ++i) bins[i] = std::max(0, bin_pos(value[i], i) - 1);
    return histogram.at<float>(bins);
}

float image::Histogram3D::intensity(int channel, double value) const {
    return 0; // TODO: please dont use
}

void image::Histogram3D::update_probability(image::HistColor value, float increment) {
    int bins[channel_count];
    for (int i = 0; i < channel_count; ++i) bins[i] = bin_pos(value[i], i);
    histogram.at<double>(bins) = std::fmax(histogram.at<double>(bins) + increment, 0);
}

image::HistColor image::Histogram3D::dominat_val() const {
    return image::HistColor(); // TODO: please dont use
}

void image::Histogram3D::write(cv::FileStorage &fs) const {
    fs << "{";
    fs << "channel_count" << channel_count;

    fs << "ranges" << "[";
    for (int i = 0; i < channel_count; ++i) fs << ranges[i];
    fs << "]";

    fs << "histogram" << histogram;

    fs << "}";
}

void image::Histogram3D::read(const cv::FileNode &node) {
    channel_count = (int) node["channel_count"];

    FileNode ranges_seq = node["ranges"];
    M_Assert(ranges_seq.type() == FileNode::SEQ, "Ranges should be a sequence");
    ranges = new float[channel_count];
    int is = 0;
    for (auto it = ranges_seq.begin(); it != ranges_seq.end(); ++it, ++is) ranges[is] = (float) *it;

    node["histogram"] >> histogram;
}