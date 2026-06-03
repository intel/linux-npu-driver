#
# Copyright (C) 2022-2026 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

option(ENABLE_GOOGLETEST_FROM_SUBMODULE "Force building GoogleTest from submodule" OFF)

# Try to find system-installed GoogleTest package
# Package names vary by distribution:
# - Fedora/RHEL/openSUSE Tumbleweed: gtest-devel (provides gmock-devel as well)
# - Ubuntu/Debian: libgtest-dev
if(NOT ENABLE_GOOGLETEST_FROM_SUBMODULE)
  find_package(GTest QUIET)
endif()

if(NOT GTest_FOUND)
  message(STATUS "System GoogleTest not found, building from submodule")
  set(INSTALL_GTEST OFF)
  add_subdirectory(googletest EXCLUDE_FROM_ALL)
else()
  message(STATUS "  Found gtest, version ${GTest_VERSION}")
  # Display include directories and library paths for each target
  if(TARGET GTest::gtest)
    get_target_property(GTEST_INCLUDE_DIRS GTest::gtest INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(GTEST_LOCATION GTest::gtest LOCATION)
    message(STATUS "GTest_INCLUDE_DIRS	: ${GTEST_INCLUDE_DIRS}")
    message(STATUS "GTest_LIBRARIES	: ${GTEST_LOCATION}")
    message(STATUS "GTest_VERSION	: ${GTest_VERSION}")
  endif()

  if(TARGET GTest::gmock)
    get_target_property(GMOCK_INCLUDE_DIRS GTest::gmock INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(GMOCK_LOCATION GTest::gmock LOCATION)
    message(STATUS "GMock_INCLUDE_DIRS	: ${GMOCK_INCLUDE_DIRS}")
    message(STATUS "GMock_LIBRARIES	: ${GMOCK_LOCATION}")
    message(STATUS "GMock_VERSION	: ${GTest_VERSION}")
  endif()

  # Create aliases for consistency with submodule build
  # The submodule provides 'gtest' and 'gmock' targets
  # System package provides 'GTest::gtest', 'GTest::gmock', etc.
  if(NOT TARGET gtest)
    add_library(gtest ALIAS GTest::gtest)
  endif()
  if(NOT TARGET gmock)
    add_library(gmock ALIAS GTest::gmock)
  endif()
  if(NOT TARGET gtest_main AND TARGET GTest::gtest_main)
    add_library(gtest_main ALIAS GTest::gtest_main)
  endif()
  if(NOT TARGET gmock_main AND TARGET GTest::gmock_main)
    add_library(gmock_main ALIAS GTest::gmock_main)
  endif()
endif()

