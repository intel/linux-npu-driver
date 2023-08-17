#
# Copyright (C) 2022-2023 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_NAME "intel")
set(CPACK_PACKAGE_VENDOR "Intel")
set(CPACK_PACKAGE_CONTACT "Intel Corporation")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS_PRIVATE_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS ON)

set(CPACK_COMPONENT_DRIVER-COMPILER-NPU_DESCRIPTION "Intel(R) Driver Compiler for NPU hardware\n\
The driver compiler enables compilation of OpenVINO IR models using Level Zero Graph Extension API.")
set(CPACK_COMPONENT_FW-NPU_DESCRIPTION "Intel(R) Firmware package for NPU hardware\n\
Firmware files required by IVPU kernel module.")
set(CPACK_COMPONENT_LEVEL-ZERO-NPU_DESCRIPTION "Intel(R) Level Zero Driver for NPU hardware\n\
Library implements Level Zero API to interract with NPU hardware.")
set(CPACK_COMPONENT_VALIDATION-NPU_DESCRIPTION "Intel(R) Validation applications for NPU\n\
Set of application required for testing of Intel(R) Level Zero Driver for NPU hardware.")

set(CPACK_DEBIAN_LEVEL-ZERO_PACKAGE_CONFLICTS "level-zero")
set(CPACK_DEBIAN_VALIDATION-NPU_PACKAGE_DEPENDS "level-zero (>=1.10.0)")

set(CPACK_VERBATIM_VARIABLES YES)

include(CPack)
