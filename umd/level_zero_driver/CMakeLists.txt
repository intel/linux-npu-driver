#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(TARGET_NAME_L0 ze_intel_vpu)

# Level Zero third party detection
if(DEFINED LEVEL_ZERO_ROOT)
    get_filename_component(LEVEL_ZERO_ROOT "${LEVEL_ZERO_ROOT}" ABSOLUTE)
else()
    get_filename_component(LEVEL_ZERO_ROOT_tmp "${VPU_SOURCE_DIR}/third_party/level-zero" ABSOLUTE)
    # Level Zero Headers if read from the git repo are in include/core & include/tools.
    # To support the installation path of level_zero_loader headers which is include/*
    # the header files are combined into the path include/* in the commands below.
    if(IS_DIRECTORY "${LEVEL_ZERO_ROOT_tmp}")
        set(CUSTOM_L0_INCLUDE_PATH "${VPU_BUILD_DIR}/level_zero/")
        file(MAKE_DIRECTORY ${CUSTOM_L0_INCLUDE_PATH})
        file(GLOB LEVEL_ZERO_API_HEADERS
            "${VPU_SOURCE_DIR}/third_party/level-zero/include/*.h"
            "${VPU_SOURCE_DIR}/third_party/level-zero/include/layers"
            "${VPU_SOURCE_DIR}/third_party/level-zero/include/loader"
        )
        file(COPY ${LEVEL_ZERO_API_HEADERS} DESTINATION ${CUSTOM_L0_INCLUDE_PATH})
        set(LEVEL_ZERO_ROOT "${LEVEL_ZERO_ROOT_tmp}")
    endif()
endif()

project(level-zero-vpu VERSION 1.1.0)
message(STATUS "Level Zero driver version: ${PROJECT_VERSION}")

# VPU UMD Version
# Note: Max value: 2 bytes / 65535
if(NOT DEFINED VPU_VERSION_BUILD)
    set(VPU_VERSION_BUILD 0)
    string(TIMESTAMP VPU_VERSION_BUILD "%y%U%w")
    message(STATUS "Version build: ${VPU_VERSION_BUILD}")
endif()
add_definitions(-DVPU_VERSION_BUILD=${VPU_VERSION_BUILD})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(LevelZero)
if(NOT LevelZero_FOUND)
    message(STATUS "Level zero headers not found")
    message(STATUS "Skipping level zero")
    set(BUILD_WITH_L0 FALSE PARENT_SCOPE)
    return()
endif()

add_definitions( -DL0_PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR} )
add_definitions( -DL0_PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR} )

file(WRITE "${CMAKE_BINARY_DIR}/VERSION" "${PROJECT_VERSION}")

add_compile_options(-fpermissive -fPIC)

# Copy third_party_binaries to output BIN folder
add_custom_target(copy_third_party_files)

if (CUSTOM_L0_INCLUDE_PATH)
    include_directories(${CUSTOM_L0_INCLUDE_PATH}/../)
else()
    include_directories(${LevelZero_INCLUDE_DIRS})
endif()
include_directories(ddi)

add_library(${TARGET_NAME_L0} SHARED)

add_subdirectory_unique(api)

append_sources_from_properties(L0_SHARED_LIB_SRCS VPU_CORE_SRCS_LINK)
target_sources(${TARGET_NAME_L0} PRIVATE ${L0_SHARED_LIB_SRCS})

target_link_libraries(${TARGET_NAME_L0}
    vpu_driver
    dl
)

set_target_properties(${TARGET_NAME_L0} PROPERTIES
    FOLDER ${TARGET_NAME_L0}
    VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
)

install(TARGETS ${TARGET_NAME_L0}
    LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT ${PROJECT_NAME}
        NAMELINK_SKIP
)

add_subdirectory_unique(unit_tests)

add_subdirectories()
add_subdirectory(core/source)
add_subdirectory(tools/source)

append_sources_from_properties(L0_SOURCES L0_API_SOURCE L0_CORE_SOURCE L0_EXT_SOURCE L0_TOOLS_SOURCE)
target_sources(${TARGET_NAME_L0} PRIVATE ${L0_SOURCES})
