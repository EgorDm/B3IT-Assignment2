file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "*.hpp"
        )

add_library(cvision ${SOURCE_FILES})

target_link_libraries(cvision ${OpenCV_LIBS})