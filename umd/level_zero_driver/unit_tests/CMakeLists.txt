#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

# use SKIP_UNIT_TESTS=ON to skip building unit tests
if (SKIP_UNIT_TESTS)
    message(STATUS "Skip building unit tests")
    return()
endif()

set(TARGET_NAME ${TARGET_NAME_L0}_tests)

add_executable(${TARGET_NAME}
            ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

add_subdirectories()

target_link_libraries(${TARGET_NAME} pthread
                      ${TARGET_NAME_L0}
)

append_sources_from_properties(VPU_CORE_tests_mocks VPU_CORE_tests_mocks)
target_sources(${TARGET_NAME} PRIVATE
               $<TARGET_OBJECTS:${TARGET_NAME_L0}_mocks>
               ${VPU_CORE_tests_mocks}
)

add_subdirectory_unique(source/core)
add_subdirectory_unique(source/ext)
add_subdirectory_unique(source/tools)
