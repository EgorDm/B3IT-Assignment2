//
// Created by egordm on 15-3-2018.
//

#include "evaluation.h"
#include "visualization.h"

using namespace cv;
using namespace cvision;

evaluation::ConfusionMatrix evaluation::make_confsion_matrix(const Mat &input, const Mat &ground_truth,
                                                             const Mat &prediction) {
    Mat tp, tn, fp, fn, ground_lie;
    bitwise_not(ground_truth, ground_lie);

    // Mask
    bitwise_and(input, prediction, tp, ground_truth);
    bitwise_and(input, prediction, fp, ground_lie);
    bitwise_xor(input, prediction, tn, ground_lie);
    bitwise_xor(input, prediction, fn, ground_truth);

    // Convert to b&w
    threshold(tp, tp, 1, 255, CV_THRESH_BINARY);
    cvtColor(tp, tp, COLOR_BGR2GRAY);
    threshold(tn, tn, 1, 255, CV_THRESH_BINARY);
    cvtColor(tn, tn, COLOR_BGR2GRAY);
    threshold(fp, fp, 1, 255, CV_THRESH_BINARY);
    cvtColor(fp, fp, COLOR_BGR2GRAY);
    threshold(fn, fn, 1, 255, CV_THRESH_BINARY);
    cvtColor(fn, fn, COLOR_BGR2GRAY);

    // Normalize
    int pixelCount = input.cols * input.rows;
    evaluation::ConfusionMatrix ret{};
    ret.tp = static_cast<int>(((double) countNonZero(tp) / pixelCount) * 1000);
    ret.tn = static_cast<int>(((double) countNonZero(tn) / pixelCount) * 1000);
    ret.fp = static_cast<int>(((double) countNonZero(fp) / pixelCount) * 1000);
    ret.fn = static_cast<int>(((double) countNonZero(fn) / pixelCount) * 1000);

    return ret;
}

evaluation::ConfusionMatrixResults evaluation::ConfusionMatrix::evaluate() {
    int total = (fp + fn + tp + tn);
    double v = tp;
    double c = tp + fn;
    double s = tp + fp;

    evaluation::ConfusionMatrixResults ret{};
    ret.relative_error = (double) (fp + fn) / (tp + tn);
    ret.total_performace = (double) (fp + fn) / total;
    ret.bep = (v / c) * 100;
    ret.precision = v / s;
    ret.recall = v / c;
    ret.f1 = 2 * ((ret.precision * ret.recall) / (ret.precision + ret.recall));
    ret.specificity = (double) tn / (fp + tn);
    ret.generality = c / total;

    return ret;
}

std::ostream &evaluation::operator<<(std::ostream &stream, const evaluation::ConfusionMatrix &o) {
    return stream << "- Confusion Matrix: " << std::endl
                  << "\tTrue Positives: " << o.tp << std::endl << "\tFalse Positives: " << o.fp << std::endl
                  << "\tTrue Negatives: " << o.tn << std::endl << "\tFalse Negatives: " << o.fn << std::endl;
}

std::ostream &evaluation::operator<<(std::ostream &stream, const evaluation::ConfusionMatrixResults &o) {
    return stream << "- Confusion Matrix Evalutaion:" << std::endl
                  << "\tRelative Error: " << o.relative_error << std::endl
                  << "\tTotal Preformance: " << o.total_performace << std::endl
                  << "\tBulls eye %: " << o.bep << std::endl
                  << "\tPrecision: " << o.precision << std::endl
                  << "\tRecall: " << o.recall << std::endl
                  << "\tF1 Score: " << o.f1 << std::endl
                  << "\tSpecificity: " << o.specificity << std::endl
                  << "\tGenerality: " << o.generality << std::endl;
}
