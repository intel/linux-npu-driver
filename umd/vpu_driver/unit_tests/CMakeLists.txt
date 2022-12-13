#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

# use SKIP_UNIT_TESTS=ON to skip building unit tests
if (SKIP_UNIT_TESTS)
    message(STATUS "Skip building VPU unit tests")
    return()
endif()

set(TARGET_NAME vpu_shared_tests)

append_sources_from_properties(CORE_ENABLERS VPU_CORE_SRCS_LINK)

add_executable(${TARGET_NAME}
             ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
             ${CMAKE_CURRENT_SOURCE_DIR}/mocks/mock_os_interface_imp.cpp
             ${CMAKE_CURRENT_SOURCE_DIR}/mocks/mock_os_interface_imp.hpp
             ${CMAKE_CURRENT_SOURCE_DIR}/mocks/mock_vpu_device.cpp
             ${CMAKE_CURRENT_SOURCE_DIR}/mocks/mock_vpu_device.hpp
             ${CORE_ENABLERS}
)

target_link_libraries(${TARGET_NAME}
                    vpu_driver
                    pthread
)

add_subdirectory_unique(os_interface)
add_subdirectory_unique(job_submission)
add_subdirectory_unique(vpu_device)
add_subdirectory_unique(memory)
add_subdirectory_unique(mocks)
