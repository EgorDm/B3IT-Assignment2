#include <iostream>
#include <opencv2/opencv.hpp>
#include "defines.h"
#include "file_processing.h"
#include "image_processing.h"
#include "algorithms.h"
#include "visualization.h"
#include "rainbows.h"
#include "evaluation.h"

using namespace cv;
using namespace cvision;

int main(int argc, char *argv[]) {
    auto sample = processing::file::load_sample("Pratheepan", "pic447a1uu2");
    Mat hsv_input;
    cvtColor(sample.input, hsv_input, COLOR_BGR2HSV);

    auto histogram = processing::image::extract_hsv_histogram(hsv_input, sample.label);

    auto dominantColor = processing::image::extract_dominant_color(histogram);
    Scalar low(std::fmax(dominantColor[0] - 6, 0), 0, 60);
    Scalar high(std::fmin(dominantColor[0] + 6, 255), 255, 255);

    rainbows::translate_coolors(low);
    rainbows::translate_coolors(dominantColor);
    rainbows::translate_coolors(high);

    auto mask = algorithms::simple_segmentation(hsv_input, low, high);
    Mat masked;
    sample.input.copyTo(masked, mask);

    visualization::draw_hsv_histogram(histogram);
    visualization::show_image(masked, "Predicted Mask");
    visualization::show_image(sample.input, "Input");

    auto matrix = evaluation::make_confsion_matrix(sample.input, sample.label, masked);
    std::cout << matrix << matrix.evaluate() << std::endl;

    waitKey(0);
    return 0;
}