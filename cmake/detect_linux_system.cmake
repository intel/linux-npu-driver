# Copyright (C) 2022-2025 Intel Corporation
#
# SPDX-License-Identifier: MIT

function(read_os_release ENTRY VAR_OUTPUT)
  if (ANDROID)
      if ("${ENTRY}" STREQUAL "ID")
        set(${VAR_OUTPUT} "android" PARENT_SCOPE)
      endif()
      return()
  endif()
  set(OS_RELEASE_PATH /etc/os-release)
  if (NOT EXISTS ${OS_RELEASE_PATH})
    return()
  endif()

  file(STRINGS ${OS_RELEASE_PATH} OUTPUT REGEX ^${ENTRY}=.* )
  if (NOT OUTPUT)
    message(WARNING "Could not find '${ENTRY}=.*' pattern in ${OS_RELEASE_PATH}")
    return()
  endif()

  string(REGEX REPLACE "${ENTRY}=" "" OUTPUT ${OUTPUT})
  string(REGEX REPLACE "(\"|')" "" OUTPUT ${OUTPUT})
  string(TOLOWER ${OUTPUT} OUTPUT)
  message(STATUS "${ENTRY} = ${OUTPUT}")

  set(${VAR_OUTPUT} ${OUTPUT} PARENT_SCOPE)
endfunction()

read_os_release(ID LINUX_SYSTEM_NAME)
read_os_release(VERSION_ID LINUX_SYSTEM_VERSION_ID)
