cmake_minimum_required(VERSION 3.20.10)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/model)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(example-standalone-inferencing-zephyr
    DESCRIPTION "Edge Impulse Standalone Inferencing Zephyr Example"
    VERSION 1.0.0
    LANGUAGES C CXX ASM
    )

include(utils/cmake/utils.cmake)

# Needed for colorful output
zephyr_compile_options(-fdiagnostics-color=always)

if("${BOARD}" MATCHES "esp32")
    message(STATUS "detected: esp32")
elseif("${ARCH}" STREQUAL "arm")
    message(STATUS "detected: ARM architecture")
    add_definitions(-DEI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN=1
                    -DARM_MATH_LOOPUNROLL
                    )
else()  
    message(STATUS "Not including DSP or NN extensions")
endif()
# If using Nordic boards, add the following definition
# -DEI_NORDIC

# Edge impulse SDK include directories
set(INCLUDES
    .
    src
    )
include_directories(${INCLUDES})

# add all sources to the project
target_sources(app PRIVATE src/main.cpp)
