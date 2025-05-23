/**
 * Copyright (C) 2016-2020 Xilinx, Inc
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You may
 * not use this file except in compliance with the License. A copy of the
 * License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#ifndef xocl_config_h_
#define xocl_config_h_

#ifndef CL_HPP_ENABLE_EXCEPTIONS
# define CL_HPP_ENABLE_EXCEPTIONS
#endif

#ifndef CL_HPP_MINIMUM_OPENCL_VERSION
# define CL_HPP_MINIMUM_OPENCL_VERSION 200
#endif

#ifndef CL_TARGET_OPENCL_VERSION
# define CL_TARGET_OPENCL_VERSION 300
#endif

#ifndef CL_HPP_TARGET_OPENCL_VERSION
# define CL_HPP_TARGET_OPENCL_VERSION 300
#endif

#ifdef _WIN32
# ifdef XRT_XOCL_SOURCE
#  define XRT_XOCL_EXPORT __declspec(dllexport)
# else
#  define XRT_XOCL_EXPORT __declspec(dllimport)
# endif
#endif

#ifdef __linux__
# ifdef XRT_XOCL_SOURCE
#  define XRT_XOCL_EXPORT __attribute__ ((visibility("default")))
# else
#  define XRT_XOCL_EXPORT
# endif
#endif

#include "xrt/config.h"
#include "xocl/api/icd/ocl_icd_bindings.h"
#include "xocl/core/debug.h"
#include "CL/cl_ext_xilinx.h"

#define XOCL_UNUSED XRT_UNUSED

namespace xocl { namespace config {

inline bool
api_checks()
{
  return xrt_xocl::config::get_api_checks();
}

}}

#endif
