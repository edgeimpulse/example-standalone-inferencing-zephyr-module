cmake_minimum_required(VERSION 3.13.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/../model-zephyr)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(example-standalone-inferencing-zephyr)

include(utils/cmake/utils.cmake)

# Needed for colorful output
zephyr_compile_options(-fdiagnostics-color=always)

# Use hardware acceleration for DSP and Neural Network code
# You'll need to disable these on non-Arm cores
add_definitions(-DEI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN=1
                -DARM_MATH_LOOPUNROLL
                )

# If using Nordic boards, add the following definition
# -DEI_NORDIC

# Edge impulse SDK include directories
set(INCLUDES
    .
    src
    )
include_directories(${INCLUDES})

# add all sources to the project
target_sources(app PRIVATE ${SOURCE_FILES})
target_sources(app PRIVATE src/main.cpp)
