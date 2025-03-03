#
# Copyright (C) 2025 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

include(${CPACK_PROJECT_CONFIG_FILE})

set(CPACK_GENERATOR RPM)

# Exclude firmware directories to generate intel-fw-npu package without conflicts against filesystem package
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION
    /lib /lib/firmware /lib/firmware/updates /lib/firmware/updates/intel /lib/firmware/updates/intel/vpu)

# Create package per component
set(CPACK_RPM_COMPONENT_INSTALL ON)

# Component conflicts
set(CPACK_RPM_LEVEL_ZERO_PACKAGE_CONFLICTS "level-zero, level-zero-devel")

# Get system architecture
execute_process(
    COMMAND uname -m
    OUTPUT_VARIABLE OUT_RPM_ARCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY)

set(PACKAGE_POSTFIX_NAME ${CPACK_PACKAGE_VERSION}_${LINUX_SYSTEM_NAME}${LINUX_SYSTEM_VERSION_ID}_${OUT_RPM_ARCH}.rpm)
foreach(COMPONENT ${CPACK_COMPONENTS_ALL})
    string(TOUPPER ${COMPONENT} COMPONENT_UPPER)
    set(CPACK_RPM_${COMPONENT_UPPER}_FILE_NAME ${CPACK_PACKAGE_NAME}-${COMPONENT}_${PACKAGE_POSTFIX_NAME})

    if (${COMPONENT_UPPER}_POSTINST)
        list(APPEND CPACK_RPM_${COMPONENT_UPPER}_PACKAGE_CONTROL_EXTRA ${${COMPONENT_UPPER}_POSTINST})
    endif()
    if (${COMPONENT_UPPER}_PRERM)
        list(APPEND CPACK_RPM_${COMPONENT_UPPER}_PACKAGE_CONTROL_EXTRA ${${COMPONENT_UPPER}_PRERM})
    endif()
    if (${COMPONENT_UPPER}_POSTRM)
        list(APPEND CPACK_RPM_${COMPONENT_UPPER}_PACKAGE_CONTROL_EXTRA ${${COMPONENT_UPPER}_POSTRM})
    endif()
    if (${COMPONENT_UPPER}_DEPENDS)
        set(CPACK_RPM_${COMPONENT_UPPER}_PACKAGE_REQUIRES ${${COMPONENT_UPPER}_DEPENDS})
    endif()
endforeach()
