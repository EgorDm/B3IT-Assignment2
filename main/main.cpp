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

    file::Dataset datasets[] = {pratheepan, hgr1};
    auto data = load_complex_dataset_data(datasets, 2);

    //auto window = create_complex_seg_window(data, pratheepan, "chenhao0017me9", false);
    //auto window = create_complex_seg_window(data, hgr1, "U_P_hgr1_id08_3", false);
    //auto window = create_complex_seg_window(data, hgr1, "W_P_hgr1_id03_10", false);
    //auto window = create_complex_seg_window(data, hgr1, "3_P_hgr1_id01_1", false);
    //auto window = create_complex_seg_window(data, hgr1, "T_P_hgr1_id10_3", false);
    auto window = create_complex_seg_window(data, pratheepan, "large_Chapman-family", false);
    window->init();
    window->show();

    waitKey(0);
    delete window;
    return 0;
}