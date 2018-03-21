file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "*.hpp"
        )

add_library(scheduler ${SOURCE_FILES})