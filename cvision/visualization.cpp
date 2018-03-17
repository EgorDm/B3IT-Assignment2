//
// Created by egordm on 14-3-2018.
//

#include "visualization.h"
#include "image_processing.h"

using namespace cvision;

void visualization::draw_hsv_histogram(const cvision::processing::image::Histogram &histogram, const int binSize,
                                       const int width, const int height) {
    int bin_w = cvRound((double) width / 128);

    auto dominantColor = processing::image::extract_dominant_color(histogram);

    Mat histImage(width, height, CV_8UC3, Scalar(0, 0, 0));

    Mat hst_normalized[3];
    normalize(histogram[0], hst_normalized[0], 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(histogram[1], hst_normalized[1], 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(histogram[2], hst_normalized[2], 0, histImage.rows, NORM_MINMAX, -1, Mat());

    for (int i = 1; i < binSize; i++) {
        int previ = i - 1;
        line(histImage, Point(bin_w * (previ), height - cvRound(hst_normalized[0].at<float>(previ))),
             Point(bin_w * (i), height - cvRound(hst_normalized[0].at<float>(i))),
             Scalar((180 / binSize) * i, 255, 255), 2, 8, 0);
        line(histImage, Point(bin_w * (previ), height - cvRound(hst_normalized[1].at<float>(previ))),
             Point(bin_w * (i), height - cvRound(hst_normalized[1].at<float>(i))),
             Scalar(dominantColor[0], (256 / binSize) * i, dominantColor[2]), 2, 8, 0);
        line(histImage, Point(bin_w * (previ), height - cvRound(hst_normalized[2].at<float>(previ))),
             Point(bin_w * (i), height - cvRound(hst_normalized[2].at<float>(i))),
             Scalar(dominantColor[0], dominantColor[1], (256 / binSize) * i), 2, 8, 0);
    }

    cvtColor(histImage, histImage, COLOR_HSV2BGR);
    show_image(histImage, HISTOGRAM_WINDOW);
}

void visualization::show_image(const Mat &image, const std::string title) {
    namedWindow(title, CV_WINDOW_AUTOSIZE);
    imshow(title, image);
}

void visualization::draw_histogram(const cvision::processing::image::Histogram &histogram, const int binSize,
                                   const std::string title) {
    const int width = 512;
    const int height = 512;
    int bin_w = cvRound((double) width / 128);

    auto dominantColor = processing::image::extract_dominant_color(histogram);

    Mat histImage(width, height, CV_8UC3, Scalar(0, 0, 0));

    Mat hst_normalized[3];
    normalize(histogram[0], hst_normalized[0], 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(histogram[1], hst_normalized[1], 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(histogram[2], hst_normalized[2], 0, histImage.rows, NORM_MINMAX, -1, Mat());

    for (int i = 1; i < binSize; i++) {
        int previ = i - 1;
        line(histImage, Point(bin_w * (previ), height - cvRound(hst_normalized[0].at<float>(previ))),
             Point(bin_w * (i), height - cvRound(hst_normalized[0].at<float>(i))),
             Scalar(255, 0, 0), 2, 8, 0);
        line(histImage, Point(bin_w * (previ), height - cvRound(hst_normalized[1].at<float>(previ))),
             Point(bin_w * (i), height - cvRound(hst_normalized[1].at<float>(i))),
             Scalar(0, 255, 0), 2, 8, 0);
        line(histImage, Point(bin_w * (previ), height - cvRound(hst_normalized[2].at<float>(previ))),
             Point(bin_w * (i), height - cvRound(hst_normalized[2].at<float>(i))),
             Scalar(0, 0, 255), 2, 8, 0);
    }

    show_image(histImage, title);
}
