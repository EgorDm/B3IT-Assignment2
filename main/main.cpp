#include <iostream>
#include "../cvision/file_processing.h"
#include "../cvision/image_processing.h"
#include "custom_windows.h"
#include "camera_window.h"
#include "window_creators.h"

using namespace cv;
using namespace cvision::processing;

int main() {
    auto window = create_simple_seg_window("Pratheepan", "chenhao0017me9", true, ".jpg", ".png");
    //auto window = create_complex_seg_window("Pratheepan", "chenhao0017me9", true, ".jpg", ".png");
    //auto window = CameraWindow();
    window.init();
    window.show();

    waitKey(0);
    return 0;
}