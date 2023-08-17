#
# Copyright (C) 2022 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

include(cmake/vpux_elf.cmake)

add_subdirectory(googletest EXCLUDE_FROM_ALL)

# TODO: Try to use external level-zero library (EISW-63340)
add_subdirectory(level-zero EXCLUDE_FROM_ALL)
add_dependencies(ze_loader ze_validation_layer)
install(TARGETS ze_loader ze_validation_layer)
