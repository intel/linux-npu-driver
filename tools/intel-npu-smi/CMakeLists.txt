#
# Copyright (C) 2026 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

project(intel-npu-smi
    VERSION 1.0.0
    DESCRIPTION "Intel NPU System Monitoring Tool"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    src/npu_smi_common.cpp
    src/file_utils.cpp
    src/pmt_telemetry.cpp
    src/process_tracker.cpp
    src/terminal_display.cpp
    src/csv_logger.cpp
    src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/tools/intel-npu-smi/include")

message(STATUS "Intel NPU SMI Tool Version ${PROJECT_VERSION}")

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    COMPONENT validation-npu)
