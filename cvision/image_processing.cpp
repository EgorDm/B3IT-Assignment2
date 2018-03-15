//
// Created by egordm on 14-3-2018.
//

#include "image_processing.h"
#include "defines.h"

using namespace cvision::processing;

Histogram image::extract_hsv_histogram(const Mat &src, const Mat &mask, const int binSize) {
    float hrange[] = {0, 180};
    float svrange[] = {0, 256};
    const float *hranges[] = {hrange};
    const float *svranges[] = {svrange};

    Mat hsv_planes[3];
    split(src, hsv_planes);

    Histogram histogram = {Mat(), Mat(), Mat()};
    calcHist(&hsv_planes[0], 1, 0, mask, histogram[0], 1, &binSize, hranges, true, false);
    calcHist(&hsv_planes[1], 1, 0, mask, histogram[1], 1, &binSize, svranges, true, false);
    calcHist(&hsv_planes[2], 1, 0, mask, histogram[2], 1, &binSize, svranges, true, false);

    return histogram;
}

Scalar image::extract_dominant_color(const Histogram &histogram) {
    int maxH = 0, maxS = 0, maxV = 0;

    for (int i = 0; i < 128; i++) {
        maxH = histogram[0].at<float>(i) > histogram[0].at<float>(maxH) ? i : maxH;
        maxS = histogram[1].at<float>(i) > histogram[1].at<float>(maxS) ? i : maxS;
        maxV = histogram[2].at<float>(i) > histogram[2].at<float>(maxV) ? i : maxV;
    }

    return {(180.0 / 128) * maxH, (256.0 / 128) * maxS, (256.0 / 128) * maxV};
}