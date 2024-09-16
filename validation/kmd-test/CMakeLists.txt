#
# Copyright (C) 2022-2024 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

project(npu-kmd-test VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    kmd_test.cpp
    main.cpp
    test_copy.cpp
    test_device.cpp
    test_exec.cpp
    test_uapi.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/linux/include/uapi")
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror)
target_link_libraries(${PROJECT_NAME} udev test_app_lib fw_vpu_api_headers)
install(TARGETS ${PROJECT_NAME} COMPONENT validation-npu)
