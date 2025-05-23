# SPDX-License-Identifier: Apache-2.0
# Copyright (C) 2018-2022 Xilinx, Inc. All rights reserved.
# Copyright (C) 2022 Advanced Micro Devices, Inc. All rights reserved.
cmake_minimum_required(VERSION 3.5.0)
project(XRT)
set(PROJECT_DESCRIPTION "https://github.com/Xilinx/XRT")

# Allow Duplicate Target Names
cmake_policy(SET CMP0002 NEW)

# Disable Boost warning
cmake_policy(SET CMP0167 OLD)

# Allow 'xaiengine' as a duplicate custom Target
set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS True)

# Include utility functions
include(CMake/utilities.cmake)

# Include supporting CMake functions
include(CMake/unitTestSupport.cmake)

# Exported interface into XRT for include from implemenation
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/runtime_src
  ${CMAKE_CURRENT_SOURCE_DIR}/runtime_src/core/include
  ${CMAKE_CURRENT_SOURCE_DIR}/runtime_src/core/common/gsl/include
  ${XRT_BINARY_DIR}/gen
  ${XRT_BINARY_DIR}
  )

include(CMake/settings.cmake)

# Default component name for any install() command without the COMPONENT argument
# The default component is the xrt run-time component, if XRT_DEV_COMPONENT is
# set to something different then a development component will be created with
# link libraries and header which are then excluded from runtime component
set (CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "xrt")

# Enable development package by specifying development component name
# If XRT_DEV_COMPONENT is same DEFAULT_COMPONENT then only that package
# is created with both development and run-time content.
set (XRT_DEV_COMPONENT "xrt")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${XRT_SOURCE_DIR}/CMake/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${XRT_SOURCE_DIR}/runtime_src/aie-rt/cmake/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${XRT_SOURCE_DIR}/runtime_src/aie-rt/src/cmake/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${XRT_SOURCE_DIR}/runtime_src/core/common/aiebu/lib/cmake/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${XRT_SOURCE_DIR}/runtime_src/core/common/aiebu/lib/aie-rt/fal/cmake/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${XRT_SOURCE_DIR}/runtime_src/core/common/aiebu/lib/aie-rt/cmake/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${XRT_SOURCE_DIR}/runtime_src/core/common/aiebu/src/cpp/ELFIO/cmake/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${XRT_SOURCE_DIR}/runtime_src/aie-rt/fal/cmake/")
# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${XRT_SOURCE_DIR}/runtime_src/aie-rt/driver/cmake/")
# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${XRT_SOURCE_DIR}/runtime_src/aie-rt/driver/")

message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

# This makes aiebu submodule use ELFIO from XRT
set(AIEBU_ELFIO_SRC_DIR "${XRT_SOURCE_DIR}/runtime_src/core/common/elf")

if (${XRT_NATIVE_BUILD} STREQUAL "yes")
  # Enable testing for this directory and below.  This command should be
  # in the source directory root because ctest expects to find a test
  # file in the build directory root.
  enable_testing()

  # Temporary native wrapper while Linux code is being ported to windows.
  # When completed the two build flows will once again be merged into one
  # common file
  if (WIN32)
    include(CMake/nativeWin.cmake)
  else()
    include(CMake/nativeLnx.cmake)
  endif()
  xrt_include(CMake/nativeTests.cmake)
else()
  include(CMake/embedded_system.cmake)
endif()
