#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(vpu-umd-test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# umd-test will be linked against ze_loader by default but you can
# use USE_ZE_INTEL_VPU to link against the ze_intel_vpu instead
if(DEFINED USE_ZE_INTEL_VPU)
    set(LEVEL_ZERO_LIB_NAME ze_intel_vpu)
else()
    set(LEVEL_ZERO_LIB_NAME ze_loader)
endif()
message(STATUS "LEVEL_ZERO_LIB_NAME = ${LEVEL_ZERO_LIB_NAME}" )

add_executable(${PROJECT_NAME}
    blob_params.cpp
    model_params.cpp
    main.cpp
    umd_test.cpp
    test_commands.cpp
    test_copy_performance.cpp
    test_inference_performance.cpp
    test_graph_commands.cpp
    test_context.cpp
    test_device.cpp
    test_driver.cpp
    test_memory.cpp
    test_graph.cpp
    test_graph_inference.cpp
    test_graph_cid.cpp
    test_fence.cpp
    test_event.cpp
    test_event_sync.cpp
    test_eventpool.cpp
    test_metric.cpp
    test_metric_streamer.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE -DVPU_GTEST_APP -Wall -Wextra -Werror)
target_link_libraries(${PROJECT_NAME} ${LEVEL_ZERO_LIB_NAME} test_app_lib yaml-cpp)
install(TARGETS ${PROJECT_NAME}
        COMPONENT validation-npu)
