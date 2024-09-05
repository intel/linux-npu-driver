#
# Copyright (C) 2024 Intel Corporation
#
# SPDX-License-Identifier: MIT
#


function(strip_script SCRIPT_FILENAME OUTPUT_CONTENT)
    # Remove comments, empty lines or lines like 'set -e..'
    execute_process(COMMAND
                    sh -c "sed -e '/^#/d' -e '/^$/d' -e '/set -/d' ${SCRIPT_FILENAME}"
                    OUTPUT_VARIABLE TMP_OUTPUT_CONTENT
                    COMMAND_ERROR_IS_FATAL ANY)
    set(${OUTPUT_CONTENT} ${TMP_OUTPUT_CONTENT} PARENT_SCOPE)
endfunction()


find_program(EBUILD_EXECUTABLE ebuild REQUIRED)

string(REPLACE "-" "." PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}")

set(PACKAGES_DESTINATION "${CPACK_PACKAGE_DIRECTORY}/packages")
set(REPO_NAME "intel-npu")
set(CATEGORY_NAME "intel-npu-drivers")

foreach(COMPONENT IN LISTS CPACK_COMPONENTS_ALL)
    string(TOUPPER ${COMPONENT} COMPONENT_UPPER)
    set(PACKAGE_NAME "${CPACK_PACKAGE_NAME}-${COMPONENT}")
    set(PACKAGE_DATA "${CPACK_TEMPORARY_DIRECTORY}/${COMPONENT}")
    set(PACKAGE_METADATA_DIR "${CPACK_TEMPORARY_DIRECTORY}/${REPO_NAME}/${CATEGORY_NAME}/${PACKAGE_NAME}")
    set(PACKAGE_DESCRIPTION "${CPACK_COMPONENT_${COMPONENT_UPPER}_DESCRIPTION}")

    file(MAKE_DIRECTORY "${PACKAGE_METADATA_DIR}")

    set(EBUILD_FILE "${PACKAGE_METADATA_DIR}/${PACKAGE_NAME}-${PACKAGE_VERSION}.ebuild")

    file(WRITE "${EBUILD_FILE}"
         "EAPI=7\n"
         "DESCRIPTION=\"${PACKAGE_DESCRIPTION}\"\n"
         "SLOT=\"0\"\n"
         "KEYWORDS=\"~amd64\"\n"
         "S=\"${PACKAGE_DATA}\"\n")

    if (${COMPONENT_UPPER}_DEPENDS)
        file(APPEND "${EBUILD_FILE}"
             "RDEPENDS=\"${${COMPONENT_UPPER}_DEPENDS}\"\n")
    endif()

    file(APPEND "${EBUILD_FILE}"
         "\n"
         "src_install() {\n"
         "    mkdir -p \${D}\n"
         "    cp -r \${S}/* \${D}\n"
         "}\n")

    if (${COMPONENT_UPPER}_POSTINST)
        strip_script(${${COMPONENT_UPPER}_POSTINST} POSTINST)
        file(APPEND "${EBUILD_FILE}"
             "\n"
             "pkg_postinst() {\n"
             "${POSTINST}"
             "}\n")
    endif()

    if (${COMPONENT_UPPER}_PRERM)
        strip_script(${${COMPONENT_UPPER}_PRERM} PRERM)
        file(APPEND "${EBUILD_FILE}"
             "\n"
             "pkg_prerm() {\n"
             "${PRERM}"
             "}\n")
    endif()

    if (${COMPONENT_UPPER}_POSTRM)
        strip_script(${${COMPONENT_UPPER}_POSTRM} POSTRM)
        file(APPEND "${EBUILD_FILE}"
             "\n"
             "pkg_postrm() {\n"
             "${POSTRM}"
             "}\n")
    endif()

    # Generate manifest
    execute_process(COMMAND
                       ${EBUILD_EXECUTABLE} ${EBUILD_FILE} manifest
                    COMMAND_ERROR_IS_FATAL ANY)

    # Create temporary dir for portage
    set(PORTAGE_TMPDIR "${CPACK_TEMPORARY_DIRECTORY}/portage_tmpdir")
    file(MAKE_DIRECTORY "${PORTAGE_TMPDIR}")

    if (${COMPONENT} IN_LIST EXTERNAL_COMPONENTS)
        set(COMPONENT_TYPE "external")
    else()
        set(COMPONENT_TYPE "internal")
    endif()

    set(PKGDIR "${PACKAGES_DESTINATION}/${COMPONENT_TYPE}")
    file(MAKE_DIRECTORY "${PKGDIR}")

    # Create package
    set(ENV_VARS "BINPKG_COMPRESS=zstd PORTAGE_USERNAME=$(id -un) PORTAGE_GRPNAME=$(id -gn) PORTAGE_TMPDIR=${PORTAGE_TMPDIR} PKGDIR=${PKGDIR}")
    execute_process(COMMAND
                    sh -c "${ENV_VARS} ${EBUILD_EXECUTABLE} ${EBUILD_FILE} package"
                    COMMAND_ERROR_IS_FATAL ANY)
endforeach()
