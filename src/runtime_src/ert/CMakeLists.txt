# SPDX-License-Identifier: Apache-2.0
# Copyright (C) 2019-2021 Xilinx, Inc. All rights reserved.
#
set(ERT_INSTALL_PREFIX "/opt/Xilinx/xrt/share/fw")
set(XRT_INSTALL_PREFIX "/opt/Xilinx")

if (DEFINED XRT_INSTALL_PREFIX)
  set(ERT_INSTALL_PREFIX "${XRT_INSTALL_PREFIX}/xrt/share/fw")
  message(STATUS "XRT_INSTALL_PREFIX: ${XRT_INSTALL_PREFIX}")
  message(STATUS "ERT_INSTALL_PREFIX: ${ERT_INSTALL_PREFIX}")
endif()

set(ERT_INSTALL_FIRMWARE_PREFIX "/lib/firmware/xilinx")

if ((EXISTS $ENV{XILINX_VITIS}/2024.1/gnu/microblaze) AND (${XRT_NATIVE_BUILD} STREQUAL "yes") AND (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86_64"))

message(STATUS "MicroBlaze toolchain found, preparing ERT build")
set(ERT_BUILD_ALL "yes")
add_subdirectory(scheduler)

add_custom_target(ert ALL
 DEPENDS scheduler
 )

elseif (DEFINED ENV{XRT_FIRMWARE_DIR})

file(GLOB ERTFW_FILES
  "$ENV{XRT_FIRMWARE_DIR}/sched*.bin"
  )

message("-- ERT firmare files copied from ${ERTFW_FILES}")

install(FILES
  ${XRTFW_FILES}
  DESTINATION ${ERT_INSTALL_PREFIX}
  )

install(FILES
  ${ERTFW_FILES}
  DESTINATION ${ERT_INSTALL_FIRMWARE_PREFIX}
  )

set(ERT_BUILD_ALL "no")
add_subdirectory(scheduler)

else()

message(WARNING
"****************************************************************\n\
No firmware files built or copied, resulting XRT package will be missing ERT \
scheduler firmware.  Use build.sh -ertfw <dir> to specify path to a directory \
with firmware to copy during XRT build.\n
****************************************************************")
set(ERT_BUILD_ALL "no")
add_subdirectory(scheduler)

endif()
