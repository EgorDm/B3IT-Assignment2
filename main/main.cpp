#include <iostream>
#include "../cvision/file_processing.h"
#include "../cvision/image_processing.h"
#include "custom_windows.h"
#include "camera_window.h"
#include "window_creators.h"

using namespace cv;
using namespace cvision::processing;

int main() {
    file::Dataset pratheepan("Pratheepan", ".jpg", ".png", false);//"chenhao0017me9",
    file::Dataset hgr1("hgr1", ".jpg", ".bmp", true);//"3_P_hgr1_id01_1",

    auto window = create_complex_seg_window(pratheepan, "chenhao0017me9", false);
    window->init();
    window->show();

    waitKey(0);
    delete window;
    return 0;
}