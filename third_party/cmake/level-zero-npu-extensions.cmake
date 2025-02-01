# Copyright 2022-2023 Intel Corporation.
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

# TODO: Detect empty directory and upgrade to warning then regardless
if (USE_SYSTEM_LIBRARIES)
  message(STATUS "USE_SYSTEM_LIBRARIES does not apply to level-zero-npu-extensions; it must always be vendored.")
endif()

set(LEVEL_ZERO_EXT_HEADERS_DIR "${CMAKE_BINARY_DIR}/include/level_zero")
file(MAKE_DIRECTORY ${LEVEL_ZERO_EXT_HEADERS_DIR})
file(GLOB_RECURSE LEVEL_ZERO_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/level-zero-npu-extensions/*.h)
file(COPY ${LEVEL_ZERO_HEADERS} DESTINATION ${LEVEL_ZERO_EXT_HEADERS_DIR})
