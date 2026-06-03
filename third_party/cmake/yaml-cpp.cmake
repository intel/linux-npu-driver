#
# Copyright (C) 2022-2026 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

option(ENABLE_YAML_CPP_FROM_SUBMODULE "Force building yaml-cpp from submodule" OFF)

# Try to find system-installed yaml-cpp package
# Package names vary by distribution:
# - Fedora/RHEL/openSUSE Tumbleweed: yaml-cpp-devel
# - Ubuntu/Debian: libyaml-cpp-dev
if(NOT ENABLE_YAML_CPP_FROM_SUBMODULE)
  find_package(yaml-cpp QUIET)
endif()

if(NOT yaml-cpp_FOUND)
  message(STATUS "System yaml-cpp not found, building from submodule")
  set(YAML_CPP_INSTALL OFF)
  set(BUILD_SHARED_LIBS OFF)
  # Force min CMake ver 3.5 to avoid CMake error on policy CMP0048
  set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
  add_subdirectory(yaml-cpp EXCLUDE_FROM_ALL)
endif()

