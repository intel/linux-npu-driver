# Copyright 2022-2024 Intel Corporation.
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this
# software or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express
# or implied warranties, other than those that are expressly stated in
# the License.

function(read_os_release ENTRY VAR_OUTPUT)
  set(OS_RELEASE_PATH /etc/os-release)
  if (NOT EXISTS ${OS_RELEASE_PATH})
    return()
  endif()

  file(STRINGS ${OS_RELEASE_PATH} OUTPUT REGEX ^${ENTRY}=.* )
  if (NOT OUTPUT)
    message(WARNING "Could not find '${ENTRY}=.*' pattern in ${OS_RELEASE_PATH}")
  endif()

  string(REGEX REPLACE "${ENTRY}=" "" OUTPUT ${OUTPUT})
  string(REGEX REPLACE "(\"|')" "" OUTPUT ${OUTPUT})
  string(TOLOWER ${OUTPUT} OUTPUT)
  message(STATUS "${ENTRY} = ${OUTPUT}")

  set(${VAR_OUTPUT} ${OUTPUT} PARENT_SCOPE)
endfunction()

read_os_release(ID LINUX_SYSTEM_NAME)
read_os_release(VERSION_ID LINUX_SYSTEM_VERSION_ID)
