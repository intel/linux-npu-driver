#
# Copyright (C) 2022-2024 Intel Corporation
#
# SPDX-License-Identifier: MIT

find_program(DPKG_EXECUTABLE dpkg)
if(NOT DPKG_EXECUTABLE)
  message(STATUS "CPack won't be configured for your system. Only Debian is supported")
  return()
endif()

if(${CMAKE_VERSION} VERSION_LESS "3.20")
  # Using older version stucks on detecting the package dependencies
  message(STATUS "CPack Debian in driver is not supported by cmake version less than 3.20.1")
  return()
endif()

set(CPACK_GENERATOR DEB)

# Base information
set(CPACK_PACKAGE_NAME "intel")
set(CPACK_PACKAGE_VENDOR "Intel")
set(CPACK_PACKAGE_CONTACT "Intel Corporation")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION}.${BUILD_NUMBER})

# Create package per component
set(CPACK_DEB_COMPONENT_INSTALL ON)

# Enable detection of component dependencies
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
list(APPEND SHLIBDEPS_PRIVATE_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
list(APPEND SHLIBDEPS_PRIVATE_DIRS ${CMAKE_BINARY_DIR}/_deps/openvino-src/runtime/lib/intel64/)
list(APPEND SHLIBDEPS_PRIVATE_DIRS ${CMAKE_BINARY_DIR}/_deps/openvino-src/opencv/lib/)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS_PRIVATE_DIRS ${SHLIBDEPS_PRIVATE_DIRS})

# Component dependencies
set(CPACK_DEBIAN_VALIDATION-NPU_PACKAGE_DEPENDS "level-zero (>=1.10.0) | intel-level-zero")

# Component conflicts
set(CPACK_DEBIAN_LEVEL-ZERO_PACKAGE_CONFLICTS "level-zero, level-zero-devel")

set(CPACK_DEBIAN_FW-NPU_PACKAGE_CONTROL_EXTRA ${CMAKE_CURRENT_LIST_DIR}/packaging/postinst)

# Get Debian architecture
execute_process(
  COMMAND ${DPKG_EXECUTABLE} --print-architecture
  OUTPUT_VARIABLE OUT_DPKG_ARCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
  COMMAND_ERROR_IS_FATAL ANY)

# Component list
get_cmake_property(CPACK_COMPONENTS_ALL COMPONENTS)
list(REMOVE_ITEM CPACK_COMPONENTS_ALL level-zero-devel)
list(REMOVE_ITEM CPACK_COMPONENTS_ALL openvino_standalone_package)

# File names
set(PACKAGE_POSTFIX_NAME ${CPACK_PACKAGE_VERSION}_${LINUX_SYSTEM_NAME}${LINUX_SYSTEM_VERSION_ID}_${OUT_DPKG_ARCH}.deb)
foreach(COMPONENT ${CPACK_COMPONENTS_ALL})
  string(TOUPPER ${COMPONENT} COMPONENT_UPPER)
  set(CPACK_DEBIAN_${COMPONENT_UPPER}_FILE_NAME intel-${COMPONENT}_${PACKAGE_POSTFIX_NAME})
endforeach()

# Component descriptions
set(CPACK_COMPONENT_DRIVER-COMPILER-NPU_DESCRIPTION "Intel(R) Driver Compiler for NPU hardware\n\
The driver compiler enables compilation of OpenVINO IR models using Level Zero Graph Extension API.")
set(CPACK_COMPONENT_FW-NPU_DESCRIPTION "Intel(R) Firmware package for NPU hardware\n\
Firmware files required by IVPU kernel module.")
set(CPACK_COMPONENT_LEVEL-ZERO-NPU_DESCRIPTION "Intel(R) Level Zero Driver for NPU hardware\n\
Library implements Level Zero API to interract with NPU hardware.")
set(CPACK_COMPONENT_VALIDATION-NPU_DESCRIPTION "Intel(R) Validation applications for NPU\n\
Set of application required for testing of Intel(R) Level Zero Driver for NPU hardware.")

set(CPACK_DEBIAN_DEBUGINFO_PACKAGE OFF)
if(${BUILD_TYPE_LOWER} STREQUAL "release")
  set(CPACK_DEBIAN_DEBUGINFO_PACKAGE ON)

  if (NOT CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    FILE(WRITE "${CMAKE_BINARY_DIR}/post_package.cmake" "execute_process(COMMAND sh -c \"cp -r ${CMAKE_BINARY_DIR}/_CPack_Packages/Linux/DEB/intel-${CPACK_PACKAGE_VERSION}-Linux/level-zero-npu-dbgsym/usr/lib ${CMAKE_INSTALL_PREFIX}/\") \n")
    FILE(APPEND "${CMAKE_BINARY_DIR}/post_package.cmake" "execute_process(COMMAND sh -c \"cp -r ${CMAKE_BINARY_DIR}/_CPack_Packages/Linux/DEB/intel-${CPACK_PACKAGE_VERSION}-Linux/level-zero-npu/usr/lib ${CMAKE_INSTALL_PREFIX}/\") \n")
    LIST(APPEND CPACK_POST_BUILD_SCRIPTS "${CMAKE_BINARY_DIR}/post_package.cmake")
  endif()
endif()

include(CPack)
