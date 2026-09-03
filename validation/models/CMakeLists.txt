#
# Copyright (C) 2024-2026 Intel Corporation
#
# SPDX-License-Identifier: MIT
#
project(embed_models)

add_library(${PROJECT_NAME} STATIC model_object.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

macro(add_binary_to_embed_models BINARY)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${BINARY}.o
        COMMAND ${CMAKE_LINKER} -m elf_x86_64 -r -b binary -o ${CMAKE_CURRENT_BINARY_DIR}/${BINARY}.o ${BINARY}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${BINARY}
        VERBATIM)
    target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${BINARY}.o)
endmacro()

foreach(MODEL_DIR add_abc mul_add)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${MODEL_DIR})
    file(GLOB BINARIES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${MODEL_DIR}/*)
    foreach(BINARY ${BINARIES})
        add_binary_to_embed_models(${BINARY})
    endforeach()
endforeach()


if(NOT VALIDATION_MODELS)
  return()
endif()

function(download_gta_asset ASSET_URL DEST_DIR)
    string(REGEX REPLACE "/$" "" ASSET_URL ${ASSET_URL})
    string(REGEX MATCH "^https://[^/]+" ROOT_URL ${ASSET_URL})
    string(REPLACE "${ROOT_URL}/" "" ASSET_URL ${ASSET_URL})
    string(REPLACE "/" ";" ASSET_URL ${ASSET_URL})
    list(POP_BACK ASSET_URL ASSET_VERSION)
    list(POP_BACK ASSET_URL ASSET_NAME)
    list(JOIN ASSET_URL "/" ASSET_PATH)
    execute_process(
        COMMAND gta-asset pull --root-url ${ROOT_URL} ${ASSET_PATH} ${ASSET_NAME} ${ASSET_VERSION} --user=$ENV{SYS_VPU_DRV_USER} --password=$ENV{SYS_VPU_DRV_PASS} --dest-dir=${DEST_DIR}
        COMMAND_ERROR_IS_FATAL ANY)
endfunction()
