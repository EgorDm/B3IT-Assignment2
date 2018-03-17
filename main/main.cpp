#include <iostream>
#include "../cvision/file_processing.h"
#include "../cvision/image_processing.h"
#include "complex_segmentation_window.h"
#include "simple_segmentation_window.h"

using namespace cv;
using namespace cvision::processing;

int main() {
    auto window = create_simple_seg_window("hgr1", "4_P_hgr1_id01_3", true, ".jpg", ".bmp");
    //auto window = create_complex_seg_window("hgr1", "4_P_hgr1_id01_3", true, ".jpg", ".bmp");
    window.show();

    waitKey(0);
    return 0;
}