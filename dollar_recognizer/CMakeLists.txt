file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "*.hpp"
        )

add_library(dollar_recognizer ${SOURCE_FILES})
