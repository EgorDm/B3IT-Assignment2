file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "*.hpp"
        )

add_executable(cvision ${SOURCE_FILES})

target_link_libraries(cvision ${OpenCV_LIBS})