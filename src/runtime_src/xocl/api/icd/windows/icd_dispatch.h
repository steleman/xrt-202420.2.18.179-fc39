/*
 * Copyright (c) 2016-2019 The Khronos Group Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * OpenCL is a trademark of Apple Inc. used under license by Khronos.
 */

////////////////////////////////////////////////////////////////
// This file is copied from KhronosGroup/OpenCL-ICD-Loader
// The content has been modified to remove the defintion of
// OpenCL vendor dispatch, which is defined in XRT and shared
// with Linux ocl icd loader (ocl-icd).
////////////////////////////////////////////////////////////////
#ifndef _ICD_DISPATCH_H_
#define _ICD_DISPATCH_H_

#ifndef CL_USE_DEPRECATED_OPENCL_1_0_APIS
#define CL_USE_DEPRECATED_OPENCL_1_0_APIS
#endif

#ifndef CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#endif

#ifndef CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif

#ifndef CL_USE_DEPRECATED_OPENCL_2_0_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#endif

// cl.h
#include <CL/cl.h>

// cl_gl.h and required files
#ifdef _WIN32
#include <windows.h>
#include <d3d9.h>
#include <d3d10_1.h>
#include <CL/cl_d3d10.h>
#include <CL/cl_d3d11.h>
#include <CL/cl_dx9_media_sharing.h>
#endif
#include <CL/cl_gl.h>
#include <CL/cl_gl_ext.h>
#include <CL/cl_ext_xocl.h>
#include <CL/cl_egl.h>

/*
 *
 * function pointer typedefs
 *
 */

// Platform APIs
typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetPlatformIDs)(
                 cl_uint          num_entries,
                 cl_platform_id * platforms,
                 cl_uint *        num_platforms) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetPlatformInfo)(
    cl_platform_id   platform,
    cl_platform_info param_name,
    size_t           param_value_size,
    void *           param_value,
    size_t *         param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;

// Device APIs
typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetDeviceIDs)(
    cl_platform_id   platform,
    cl_device_type   device_type,
    cl_uint          num_entries,
    cl_device_id *   devices,
    cl_uint *        num_devices) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetDeviceInfo)(
    cl_device_id    device,
    cl_device_info  param_name,
    size_t          param_value_size,
    void *          param_value,
    size_t *        param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clCreateSubDevices)(
    cl_device_id     in_device,
    const cl_device_partition_property * partition_properties,
    cl_uint          num_entries,
    cl_device_id *   out_devices,
    cl_uint *        num_devices);

typedef CL_API_ENTRY cl_int (CL_API_CALL * KHRpfn_clRetainDevice)(
    cl_device_id     device) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int (CL_API_CALL * KHRpfn_clReleaseDevice)(
    cl_device_id     device) CL_API_SUFFIX__VERSION_1_2;

// Context APIs
typedef CL_API_ENTRY cl_context (CL_API_CALL *KHRpfn_clCreateContext)(
    const cl_context_properties * properties,
    cl_uint                 num_devices,
    const cl_device_id *    devices,
    void (CL_CALLBACK *pfn_notify)(const char *, const void *, size_t, void *),
    void *                  user_data,
    cl_int *                errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_context (CL_API_CALL *KHRpfn_clCreateContextFromType)(
    const cl_context_properties * properties,
    cl_device_type          device_type,
    void (CL_CALLBACK *pfn_notify)(const char *, const void *, size_t, void *),
    void *                  user_data,
    cl_int *                errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clRetainContext)(
    cl_context context) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clReleaseContext)(
    cl_context context) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetContextInfo)(
    cl_context         context,
    cl_context_info    param_name,
    size_t             param_value_size,
    void *             param_value,
    size_t *           param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;

// Command Queue APIs
typedef CL_API_ENTRY cl_command_queue (CL_API_CALL *KHRpfn_clCreateCommandQueue)(
    cl_context                     context,
    cl_device_id                   device,
    cl_command_queue_properties    properties,
    cl_int *                       errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_command_queue (CL_API_CALL *KHRpfn_clCreateCommandQueueWithProperties)(
    cl_context                  /* context */,
    cl_device_id                /* device */,
    const cl_queue_properties * /* properties */,
    cl_int *                    /* errcode_ret */) CL_API_SUFFIX__VERSION_2_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clRetainCommandQueue)(
    cl_command_queue command_queue) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clReleaseCommandQueue)(
    cl_command_queue command_queue) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetCommandQueueInfo)(
    cl_command_queue      command_queue,
    cl_command_queue_info param_name,
    size_t                param_value_size,
    void *                param_value,
    size_t *              param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;

// Memory Object APIs
typedef CL_API_ENTRY cl_mem (CL_API_CALL *KHRpfn_clCreateBuffer)(
    cl_context   context,
    cl_mem_flags flags,
    size_t       size,
    void *       host_ptr,
    cl_int *     errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *KHRpfn_clCreateImage)(
    cl_context              context,
    cl_mem_flags            flags,
    const cl_image_format * image_format,
    const cl_image_desc *   image_desc,
    void *                  host_ptr,
    cl_int *                errcode_ret) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clRetainMemObject)(cl_mem memobj) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clReleaseMemObject)(cl_mem memobj) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetSupportedImageFormats)(
    cl_context           context,
    cl_mem_flags         flags,
    cl_mem_object_type   image_type,
    cl_uint              num_entries,
    cl_image_format *    image_formats,
    cl_uint *            num_image_formats) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetMemObjectInfo)(
    cl_mem           memobj,
    cl_mem_info      param_name,
    size_t           param_value_size,
    void *           param_value,
    size_t *         param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetImageInfo)(
    cl_mem           image,
    cl_image_info    param_name,
    size_t           param_value_size,
    void *           param_value,
    size_t *         param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *KHRpfn_clCreatePipe)(
    cl_context                 /* context */,
    cl_mem_flags               /* flags */,
    cl_uint                    /* pipe_packet_size */,
    cl_uint                    /* pipe_max_packets */,
    const cl_pipe_properties * /* properties */,
    cl_int *                   /* errcode_ret */) CL_API_SUFFIX__VERSION_2_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetPipeInfo)(
    cl_mem       /* pipe */,
    cl_pipe_info /* param_name */,
    size_t       /* param_value_size */,
    void *       /* param_value */,
    size_t *     /* param_value_size_ret */) CL_API_SUFFIX__VERSION_2_0;

typedef CL_API_ENTRY void * (CL_API_CALL *KHRpfn_clSVMAlloc)(
    cl_context       /* context */,
    cl_svm_mem_flags /* flags */,
    size_t           /* size */,
    unsigned int     /* alignment */) CL_API_SUFFIX__VERSION_2_0;

typedef CL_API_ENTRY void (CL_API_CALL *KHRpfn_clSVMFree)(
    cl_context /* context */,
    void *     /* svm_pointer */) CL_API_SUFFIX__VERSION_2_0;

// Sampler APIs
typedef CL_API_ENTRY cl_sampler (CL_API_CALL *KHRpfn_clCreateSampler)(
    cl_context          context,
    cl_bool             normalized_coords,
    cl_addressing_mode  addressing_mode,
    cl_filter_mode      filter_mode,
    cl_int *            errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clRetainSampler)(cl_sampler sampler) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clReleaseSampler)(cl_sampler sampler) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetSamplerInfo)(
    cl_sampler         sampler,
    cl_sampler_info    param_name,
    size_t             param_value_size,
    void *             param_value,
    size_t *           param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_sampler (CL_API_CALL *KHRpfn_clCreateSamplerWithProperties)(
    cl_context                    /* context */,
    const cl_sampler_properties * /* sampler_properties */,
    cl_int *                      /* errcode_ret */) CL_API_SUFFIX__VERSION_2_0;

// Program Object APIs
typedef CL_API_ENTRY cl_program (CL_API_CALL *KHRpfn_clCreateProgramWithSource)(
    cl_context        context,
    cl_uint           count,
    const char **     strings,
    const size_t *    lengths,
    cl_int *          errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_program (CL_API_CALL *KHRpfn_clCreateProgramWithBinary)(
    cl_context                     context,
    cl_uint                        num_devices,
    const cl_device_id *           device_list,
    const size_t *                 lengths,
    const unsigned char **         binaries,
    cl_int *                       binary_status,
    cl_int *                       errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_program (CL_API_CALL *KHRpfn_clCreateProgramWithBuiltInKernels)(
    cl_context            context,
    cl_uint               num_devices,
    const cl_device_id *  device_list,
    const char *          kernel_names,
    cl_int *              errcode_ret) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clRetainProgram)(cl_program program) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clReleaseProgram)(cl_program program) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clBuildProgram)(
    cl_program           program,
    cl_uint              num_devices,
    const cl_device_id * device_list,
    const char *         options,
    void (CL_CALLBACK *pfn_notify)(cl_program program, void * user_data),
    void *               user_data) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clCompileProgram)(
    cl_program           program,
    cl_uint              num_devices,
    const cl_device_id * device_list,
    const char *         options,
    cl_uint              num_input_headers,
    const cl_program *   input_headers,
    const char **        header_include_names,
    void (CL_CALLBACK *  pfn_notify)(cl_program program, void * user_data),
    void *               user_data) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_program (CL_API_CALL *KHRpfn_clLinkProgram)(
    cl_context           context,
    cl_uint              num_devices,
    const cl_device_id * device_list,
    const char *         options,
    cl_uint              num_input_programs,
    const cl_program *   input_programs,
    void (CL_CALLBACK *  pfn_notify)(cl_program program, void * user_data),
    void *               user_data,
    cl_int *             errcode_ret) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clSetProgramSpecializationConstant)(
    cl_program           program,
    cl_uint              spec_id,
    size_t               spec_size,
    const void*          spec_value) CL_API_SUFFIX__VERSION_2_2;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clSetProgramReleaseCallback)(
    cl_program           program,
    void (CL_CALLBACK *  pfn_notify)(cl_program program, void * user_data),
    void *               user_data) CL_API_SUFFIX__VERSION_2_2;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clUnloadPlatformCompiler)(
    cl_platform_id     platform) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetProgramInfo)(
    cl_program         program,
    cl_program_info    param_name,
    size_t             param_value_size,
    void *             param_value,
    size_t *           param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetProgramBuildInfo)(
    cl_program            program,
    cl_device_id          device,
    cl_program_build_info param_name,
    size_t                param_value_size,
    void *                param_value,
    size_t *              param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;

// Kernel Object APIs
typedef CL_API_ENTRY cl_kernel (CL_API_CALL *KHRpfn_clCreateKernel)(
    cl_program      program,
    const char *    kernel_name,
    cl_int *        errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clCreateKernelsInProgram)(
    cl_program     program,
    cl_uint        num_kernels,
    cl_kernel *    kernels,
    cl_uint *      num_kernels_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clRetainKernel)(cl_kernel    kernel) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clReleaseKernel)(cl_kernel   kernel) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clSetKernelArg)(
    cl_kernel    kernel,
    cl_uint      arg_index,
    size_t       arg_size,
    const void * arg_value) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetKernelInfo)(
    cl_kernel       kernel,
    cl_kernel_info  param_name,
    size_t          param_value_size,
    void *          param_value,
    size_t *        param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetKernelArgInfo)(
    cl_kernel       kernel,
    cl_uint         arg_indx,
    cl_kernel_arg_info  param_name,
    size_t          param_value_size,
    void *          param_value,
    size_t *        param_value_size_ret) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetKernelWorkGroupInfo)(
    cl_kernel                  kernel,
    cl_device_id               device,
    cl_kernel_work_group_info  param_name,
    size_t                     param_value_size,
    void *                     param_value,
    size_t *                   param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clSetKernelArgSVMPointer)(
    cl_kernel    /* kernel */,
    cl_uint      /* arg_index */,
    const void * /* arg_value */) CL_API_SUFFIX__VERSION_2_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clSetKernelExecInfo)(
    cl_kernel            /* kernel */,
    cl_kernel_exec_info  /* param_name */,
    size_t               /* param_value_size */,
    const void *         /* param_value */) CL_API_SUFFIX__VERSION_2_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetKernelSubGroupInfoKHR)(
    cl_kernel                /* in_kernel */,
    cl_device_id             /*in_device*/,
    cl_kernel_sub_group_info /* param_name */,
    size_t                   /*input_value_size*/,
    const void *             /*input_value*/,
    size_t                   /*param_value_size*/,
    void*                    /*param_value*/,
    size_t*                  /*param_value_size_ret*/) CL_EXT_SUFFIX__VERSION_2_0;

// Event Object APIs
typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clWaitForEvents)(
    cl_uint             num_events,
    const cl_event *    event_list) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetEventInfo)(
    cl_event         event,
    cl_event_info    param_name,
    size_t           param_value_size,
    void *           param_value,
    size_t *         param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clRetainEvent)(cl_event event) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clReleaseEvent)(cl_event event) CL_API_SUFFIX__VERSION_1_0;

// Profiling APIs
typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetEventProfilingInfo)(
    cl_event            event,
    cl_profiling_info   param_name,
    size_t              param_value_size,
    void *              param_value,
    size_t *            param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;

// Flush and Finish APIs
typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clFlush)(cl_command_queue command_queue) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clFinish)(cl_command_queue command_queue) CL_API_SUFFIX__VERSION_1_0;

// Enqueued Commands APIs
typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueReadBuffer)(
    cl_command_queue    command_queue,
    cl_mem              buffer,
    cl_bool             blocking_read,
    size_t              offset,
    size_t              cb,
    void *              ptr,
    cl_uint             num_events_in_wait_list,
    const cl_event *    event_wait_list,
    cl_event *          event) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueReadBufferRect)(
    cl_command_queue    command_queue,
    cl_mem              buffer,
    cl_bool             blocking_read,
    const size_t *      buffer_origin,
    const size_t *      host_origin,
    const size_t *      region,
    size_t              buffer_row_pitch,
    size_t              buffer_slice_pitch,
    size_t              host_row_pitch,
    size_t              host_slice_pitch,
    void *              ptr,
    cl_uint             num_events_in_wait_list,
    const cl_event *    event_wait_list,
    cl_event *          event) CL_API_SUFFIX__VERSION_1_1;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueWriteBuffer)(
    cl_command_queue   command_queue,
    cl_mem             buffer,
    cl_bool            blocking_write,
    size_t             offset,
    size_t             cb,
    const void *       ptr,
    cl_uint            num_events_in_wait_list,
    const cl_event *   event_wait_list,
    cl_event *         event) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueWriteBufferRect)(
    cl_command_queue    command_queue,
    cl_mem              buffer,
    cl_bool             blocking_read,
    const size_t *      buffer_origin,
    const size_t *      host_origin,
    const size_t *      region,
    size_t              buffer_row_pitch,
    size_t              buffer_slice_pitch,
    size_t              host_row_pitch,
    size_t              host_slice_pitch,
    const void *        ptr,
    cl_uint             num_events_in_wait_list,
    const cl_event *    event_wait_list,
    cl_event *          event) CL_API_SUFFIX__VERSION_1_1;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueFillBuffer)(
    cl_command_queue   command_queue,
    cl_mem             buffer,
    const void *       pattern,
    size_t             pattern_size,
    size_t             offset,
    size_t             cb,
    cl_uint            num_events_in_wait_list,
    const cl_event *   event_wait_list,
    cl_event *         event) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueCopyBuffer)(
    cl_command_queue    command_queue,
    cl_mem              src_buffer,
    cl_mem              dst_buffer,
    size_t              src_offset,
    size_t              dst_offset,
    size_t              cb,
    cl_uint             num_events_in_wait_list,
    const cl_event *    event_wait_list,
    cl_event *          event) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueCopyBufferRect)(
    cl_command_queue    command_queue,
    cl_mem              src_buffer,
    cl_mem              dst_buffer,
    const size_t *      src_origin,
    const size_t *      dst_origin,
    const size_t *      region,
    size_t              src_row_pitch,
    size_t              src_slice_pitch,
    size_t              dst_row_pitch,
    size_t              dst_slice_pitch,
    cl_uint             num_events_in_wait_list,
    const cl_event *    event_wait_list,
    cl_event *          event) CL_API_SUFFIX__VERSION_1_1;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueReadImage)(
    cl_command_queue     command_queue,
    cl_mem               image,
    cl_bool              blocking_read,
    const size_t *       origin,
    const size_t *       region,
    size_t               row_pitch,
    size_t               slice_pitch,
    void *               ptr,
    cl_uint              num_events_in_wait_list,
    const cl_event *     event_wait_list,
    cl_event *           event) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueWriteImage)(
    cl_command_queue    command_queue,
    cl_mem              image,
    cl_bool             blocking_write,
    const size_t *      origin,
    const size_t *      region,
    size_t              input_row_pitch,
    size_t              input_slice_pitch,
    const void *        ptr,
    cl_uint             num_events_in_wait_list,
    const cl_event *    event_wait_list,
    cl_event *          event) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueFillImage)(
    cl_command_queue   command_queue,
    cl_mem             image,
    const void *       fill_color,
    const size_t       origin[3],
    const size_t       region[3],
    cl_uint            num_events_in_wait_list,
    const cl_event *   event_wait_list,
    cl_event *         event) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueCopyImage)(
    cl_command_queue     command_queue,
    cl_mem               src_image,
    cl_mem               dst_image,
    const size_t *       src_origin,
    const size_t *       dst_origin,
    const size_t *       region,
    cl_uint              num_events_in_wait_list,
    const cl_event *     event_wait_list,
    cl_event *           event) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueCopyImageToBuffer)(
    cl_command_queue command_queue,
    cl_mem           src_image,
    cl_mem           dst_buffer,
    const size_t *   src_origin,
    const size_t *   region,
    size_t           dst_offset,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueCopyBufferToImage)(
    cl_command_queue command_queue,
    cl_mem           src_buffer,
    cl_mem           dst_image,
    size_t           src_offset,
    const size_t *   dst_origin,
    const size_t *   region,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY void * (CL_API_CALL *KHRpfn_clEnqueueMapBuffer)(
    cl_command_queue command_queue,
    cl_mem           buffer,
    cl_bool          blocking_map,
    cl_map_flags     map_flags,
    size_t           offset,
    size_t           cb,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event,
    cl_int *         errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY void * (CL_API_CALL *KHRpfn_clEnqueueMapImage)(
    cl_command_queue  command_queue,
    cl_mem            image,
    cl_bool           blocking_map,
    cl_map_flags      map_flags,
    const size_t *    origin,
    const size_t *    region,
    size_t *          image_row_pitch,
    size_t *          image_slice_pitch,
    cl_uint           num_events_in_wait_list,
    const cl_event *  event_wait_list,
    cl_event *        event,
    cl_int *          errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueUnmapMemObject)(
    cl_command_queue command_queue,
    cl_mem           memobj,
    void *           mapped_ptr,
    cl_uint          num_events_in_wait_list,
    const cl_event *  event_wait_list,
    cl_event *        event) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueMigrateMemObjects)(
    cl_command_queue       command_queue,
    cl_uint                num_mem_objects,
    const cl_mem *         mem_objects,
    cl_mem_migration_flags flags,
    cl_uint                num_events_in_wait_list,
    const cl_event *       event_wait_list,
    cl_event *             event) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueNDRangeKernel)(
    cl_command_queue command_queue,
    cl_kernel        kernel,
    cl_uint          work_dim,
    const size_t *   global_work_offset,
    const size_t *   global_work_size,
    const size_t *   local_work_size,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueTask)(
    cl_command_queue  command_queue,
    cl_kernel         kernel,
    cl_uint           num_events_in_wait_list,
    const cl_event *  event_wait_list,
    cl_event *        event) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueNativeKernel)(
    cl_command_queue  command_queue,
    void (CL_CALLBACK * user_func)(void *),
    void *            args,
    size_t            cb_args,
    cl_uint           num_mem_objects,
    const cl_mem *    mem_list,
    const void **     args_mem_loc,
    cl_uint           num_events_in_wait_list,
    const cl_event *  event_wait_list,
    cl_event *        event) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueMarkerWithWaitList)(
    cl_command_queue  command_queue,
    cl_uint           num_events_in_wait_list,
    const cl_event *  event_wait_list,
    cl_event *        event) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueBarrierWithWaitList)(
    cl_command_queue  command_queue,
    cl_uint           num_events_in_wait_list,
    const cl_event *  event_wait_list,
    cl_event *        event) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY void * (CL_API_CALL *KHRpfn_clGetExtensionFunctionAddressForPlatform)(
    cl_platform_id platform,
    const char *   function_name) CL_API_SUFFIX__VERSION_1_2;

// Shared Virtual Memory APIs
typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueSVMFree)(
    cl_command_queue /* command_queue */,
    cl_uint          /* num_svm_pointers */,
    void **          /* svm_pointers */,
    void (CL_CALLBACK *pfn_free_func)(
        cl_command_queue /* queue */,
        cl_uint          /* num_svm_pointers */,
        void **          /* svm_pointers[] */,
        void *           /* user_data */),
    void *           /* user_data */,
    cl_uint          /* num_events_in_wait_list */,
    const cl_event * /* event_wait_list */,
    cl_event * /* event */) CL_API_SUFFIX__VERSION_2_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueSVMMemcpy)(
    cl_command_queue /* command_queue */,
    cl_bool          /* blocking_copy */,
    void *           /* dst_ptr */,
    const void *     /* src_ptr */,
    size_t           /* size */,
    cl_uint          /* num_events_in_wait_list */,
    const cl_event * /* event_wait_list */,
    cl_event *       /* event */) CL_API_SUFFIX__VERSION_2_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueSVMMemFill)(
    cl_command_queue /* command_queue */,
    void *           /* svm_ptr */,
    const void *     /* pattern */,
    size_t           /* pattern_size */,
    size_t           /* size */,
    cl_uint          /* num_events_in_wait_list */,
    const cl_event * /* event_wait_list */,
    cl_event *       /* event */) CL_API_SUFFIX__VERSION_2_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueSVMMap)(
    cl_command_queue /* command_queue */,
    cl_bool          /* blocking_map */,
    cl_map_flags     /* map_flags */,
    void *           /* svm_ptr */,
    size_t           /* size */,
    cl_uint          /* num_events_in_wait_list */,
    const cl_event * /* event_wait_list */,
    cl_event *       /* event */) CL_API_SUFFIX__VERSION_2_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueSVMUnmap)(
    cl_command_queue /* command_queue */,
    void *           /* svm_ptr */,
    cl_uint          /* num_events_in_wait_list */,
    const cl_event * /* event_wait_list */,
    cl_event *       /* event */) CL_API_SUFFIX__VERSION_2_0;

// Deprecated APIs
typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clSetCommandQueueProperty)(
    cl_command_queue              command_queue,
    cl_command_queue_properties   properties,
    cl_bool                       enable,
    cl_command_queue_properties * old_properties) CL_EXT_SUFFIX__VERSION_1_0_DEPRECATED;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *KHRpfn_clCreateImage2D)(
    cl_context              context,
    cl_mem_flags            flags,
    const cl_image_format * image_format,
    size_t                  image_width,
    size_t                  image_height,
    size_t                  image_row_pitch,
    void *                  host_ptr,
    cl_int *                errcode_ret) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *KHRpfn_clCreateImage3D)(
    cl_context              context,
    cl_mem_flags            flags,
    const cl_image_format * image_format,
    size_t                  image_width,
    size_t                  image_height,
    size_t                  image_depth,
    size_t                  image_row_pitch,
    size_t                  image_slice_pitch,
    void *                  host_ptr,
    cl_int *                errcode_ret) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clUnloadCompiler)(void) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueMarker)(
    cl_command_queue    command_queue,
    cl_event *          event) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueWaitForEvents)(
    cl_command_queue command_queue,
    cl_uint          num_events,
    const cl_event * event_list) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueBarrier)(cl_command_queue command_queue) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;

typedef CL_API_ENTRY void * (CL_API_CALL *KHRpfn_clGetExtensionFunctionAddress)(const char *function_name) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;

// GL and other APIs
typedef CL_API_ENTRY cl_mem (CL_API_CALL *KHRpfn_clCreateFromGLBuffer)(
    cl_context    context,
    cl_mem_flags  flags,
    cl_GLuint     bufobj,
    int *         errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *KHRpfn_clCreateFromGLTexture)(
    cl_context      context,
    cl_mem_flags    flags,
    cl_GLenum       target,
    cl_GLint        miplevel,
    cl_GLuint       texture,
    cl_int *        errcode_ret) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *KHRpfn_clCreateFromGLTexture2D)(
    cl_context      context,
    cl_mem_flags    flags,
    cl_GLenum       target,
    cl_GLint        miplevel,
    cl_GLuint       texture,
    cl_int *        errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *KHRpfn_clCreateFromGLTexture3D)(
    cl_context      context,
    cl_mem_flags    flags,
    cl_GLenum       target,
    cl_GLint        miplevel,
    cl_GLuint       texture,
    cl_int *        errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *KHRpfn_clCreateFromGLRenderbuffer)(
    cl_context           context,
    cl_mem_flags         flags,
    cl_GLuint            renderbuffer,
    cl_int *             errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetGLObjectInfo)(
    cl_mem               memobj,
    cl_gl_object_type *  gl_object_type,
    cl_GLuint *          gl_object_name) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetGLTextureInfo)(
    cl_mem               memobj,
    cl_gl_texture_info   param_name,
    size_t               param_value_size,
    void *               param_value,
    size_t *             param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueAcquireGLObjects)(
    cl_command_queue     command_queue,
    cl_uint              num_objects,
    const cl_mem *       mem_objects,
    cl_uint              num_events_in_wait_list,
    const cl_event *     event_wait_list,
    cl_event *           event) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueReleaseGLObjects)(
    cl_command_queue     command_queue,
    cl_uint              num_objects,
    const cl_mem *       mem_objects,
    cl_uint              num_events_in_wait_list,
    const cl_event *     event_wait_list,
    cl_event *           event) CL_API_SUFFIX__VERSION_1_0;

/* cl_khr_gl_sharing */
typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetGLContextInfoKHR)(
    const cl_context_properties *properties,
    cl_gl_context_info param_name,
    size_t param_value_size,
    void *param_value,
    size_t *param_value_size_ret);

/* cl_khr_gl_event */
typedef CL_API_ENTRY cl_event (CL_API_CALL *KHRpfn_clCreateEventFromGLsyncKHR)(
    cl_context context,
    cl_GLsync sync,
    cl_int *errcode_ret);


#if defined(_WIN32)

/* cl_khr_d3d10_sharing */

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetDeviceIDsFromD3D10KHR)(
    cl_platform_id             platform,
    cl_d3d10_device_source_khr d3d_device_source,
    void *                     d3d_object,
    cl_d3d10_device_set_khr    d3d_device_set,
    cl_uint                    num_entries,
    cl_device_id *             devices,
    cl_uint *                  num_devices) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *KHRpfn_clCreateFromD3D10BufferKHR)(
    cl_context     context,
    cl_mem_flags   flags,
    ID3D10Buffer * resource,
    cl_int *       errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *KHRpfn_clCreateFromD3D10Texture2DKHR)(
    cl_context        context,
    cl_mem_flags      flags,
    ID3D10Texture2D * resource,
    UINT              subresource,
    cl_int *          errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *KHRpfn_clCreateFromD3D10Texture3DKHR)(
    cl_context        context,
    cl_mem_flags      flags,
    ID3D10Texture3D * resource,
    UINT              subresource,
    cl_int *          errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueAcquireD3D10ObjectsKHR)(
    cl_command_queue command_queue,
    cl_uint          num_objects,
    const cl_mem *   mem_objects,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueReleaseD3D10ObjectsKHR)(
    cl_command_queue command_queue,
    cl_uint          num_objects,
    const cl_mem *   mem_objects,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event) CL_API_SUFFIX__VERSION_1_0;

extern CL_API_ENTRY cl_int CL_API_CALL
clGetDeviceIDsFromD3D10KHR(
    cl_platform_id platform,
    cl_d3d10_device_source_khr d3d_device_source,
    void *d3d_object,
    cl_d3d10_device_set_khr d3d_device_set,
    cl_uint num_entries,
    cl_device_id *devices,
    cl_uint *num_devices);

extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromD3D10BufferKHR(
    cl_context context,
    cl_mem_flags flags,
    ID3D10Buffer *resource,
    cl_int *errcode_ret);

extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromD3D10Texture2DKHR(
    cl_context        context,
    cl_mem_flags      flags,
    ID3D10Texture2D * resource,
    UINT              subresource,
    cl_int *          errcode_ret);

extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromD3D10Texture3DKHR(
    cl_context context,
    cl_mem_flags flags,
    ID3D10Texture3D *resource,
    UINT subresource,
    cl_int *errcode_ret);

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueAcquireD3D10ObjectsKHR(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem *mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event *event_wait_list,
    cl_event *event);

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueReleaseD3D10ObjectsKHR(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem *mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event *event_wait_list,
    cl_event *event);

/* cl_khr_d3d11_sharing */
typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetDeviceIDsFromD3D11KHR)(
    cl_platform_id             platform,
    cl_d3d11_device_source_khr d3d_device_source,
    void *                     d3d_object,
    cl_d3d11_device_set_khr    d3d_device_set,
    cl_uint                    num_entries,
    cl_device_id *             devices,
    cl_uint *                  num_devices) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *KHRpfn_clCreateFromD3D11BufferKHR)(
    cl_context     context,
    cl_mem_flags   flags,
    ID3D11Buffer * resource,
    cl_int *       errcode_ret) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *KHRpfn_clCreateFromD3D11Texture2DKHR)(
    cl_context        context,
    cl_mem_flags      flags,
    ID3D11Texture2D * resource,
    UINT              subresource,
    cl_int *          errcode_ret) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *KHRpfn_clCreateFromD3D11Texture3DKHR)(
    cl_context        context,
    cl_mem_flags      flags,
    ID3D11Texture3D * resource,
    UINT              subresource,
    cl_int *          errcode_ret) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueAcquireD3D11ObjectsKHR)(
    cl_command_queue command_queue,
    cl_uint          num_objects,
    const cl_mem *   mem_objects,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueReleaseD3D11ObjectsKHR)(
    cl_command_queue command_queue,
    cl_uint          num_objects,
    const cl_mem *   mem_objects,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event) CL_API_SUFFIX__VERSION_1_2;

/* cl_khr_dx9_media_sharing */
typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetDeviceIDsFromDX9MediaAdapterKHR)(
    cl_platform_id                  platform,
    cl_uint                         num_media_adapters,
    cl_dx9_media_adapter_type_khr * media_adapters_type,
    void *                          media_adapters,
    cl_dx9_media_adapter_set_khr    media_adapter_set,
    cl_uint                         num_entries,
    cl_device_id *                  devices,
    cl_uint *                       num_devices) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *KHRpfn_clCreateFromDX9MediaSurfaceKHR)(
    cl_context                    context,
    cl_mem_flags                  flags,
    cl_dx9_media_adapter_type_khr adapter_type,
    void *                        surface_info,
    cl_uint                       plane,
    cl_int *                      errcode_ret) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueAcquireDX9MediaSurfacesKHR)(
    cl_command_queue command_queue,
    cl_uint          num_objects,
    const cl_mem *   mem_objects,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event) CL_API_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueReleaseDX9MediaSurfacesKHR)(
    cl_command_queue command_queue,
    cl_uint          num_objects,
    const cl_mem *   mem_objects,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event) CL_API_SUFFIX__VERSION_1_2;

/* cl_khr_d3d11_sharing */
extern CL_API_ENTRY cl_int CL_API_CALL
clGetDeviceIDsFromD3D11KHR(
    cl_platform_id             platform,
    cl_d3d11_device_source_khr d3d_device_source,
    void *                     d3d_object,
    cl_d3d11_device_set_khr    d3d_device_set,
    cl_uint                    num_entries,
    cl_device_id *             devices,
    cl_uint *                  num_devices);

extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromD3D11BufferKHR(
    cl_context     context,
    cl_mem_flags   flags,
    ID3D11Buffer * resource,
    cl_int *       errcode_ret);

extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromD3D11Texture2DKHR(
    cl_context        context,
    cl_mem_flags      flags,
    ID3D11Texture2D * resource,
    UINT              subresource,
    cl_int *          errcode_ret);

extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromD3D11Texture3DKHR(
    cl_context        context,
    cl_mem_flags      flags,
    ID3D11Texture3D * resource,
    UINT              subresource,
    cl_int *          errcode_ret);

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueAcquireD3D11ObjectsKHR(
    cl_command_queue command_queue,
    cl_uint          num_objects,
    const cl_mem *   mem_objects,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event);

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueReleaseD3D11ObjectsKHR(
    cl_command_queue command_queue,
    cl_uint          num_objects,
    const cl_mem *   mem_objects,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event);

/* cl_khr_dx9_media_sharing */
extern CL_API_ENTRY cl_int CL_API_CALL
clGetDeviceIDsFromDX9MediaAdapterKHR(
    cl_platform_id                 platform,
    cl_uint                        num_media_adapters,
    cl_dx9_media_adapter_type_khr * media_adapter_type,
    void *                         media_adapters,
    cl_dx9_media_adapter_set_khr   media_adapter_set,
    cl_uint                        num_entries,
    cl_device_id *                 devices,
    cl_uint *                      num_devices);

extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromDX9MediaSurfaceKHR(
    cl_context                    context,
    cl_mem_flags                  flags,
    cl_dx9_media_adapter_type_khr adapter_type,
    void *                        surface_info,
    cl_uint                       plane,
    cl_int *                      errcode_ret);

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueAcquireDX9MediaSurfacesKHR(
    cl_command_queue command_queue,
    cl_uint          num_objects,
    const cl_mem *   mem_objects,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event);

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueReleaseDX9MediaSurfacesKHR(
    cl_command_queue command_queue,
    cl_uint          num_objects,
    const cl_mem *   mem_objects,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event);

#else

/* cl_khr_d3d10_sharing */
typedef void *KHRpfn_clGetDeviceIDsFromD3D10KHR;
typedef void *KHRpfn_clCreateFromD3D10BufferKHR;
typedef void *KHRpfn_clCreateFromD3D10Texture2DKHR;
typedef void *KHRpfn_clCreateFromD3D10Texture3DKHR;
typedef void *KHRpfn_clEnqueueAcquireD3D10ObjectsKHR;
typedef void *KHRpfn_clEnqueueReleaseD3D10ObjectsKHR;

/* cl_khr_d3d11_sharing */
typedef void *KHRpfn_clGetDeviceIDsFromD3D11KHR;
typedef void *KHRpfn_clCreateFromD3D11BufferKHR;
typedef void *KHRpfn_clCreateFromD3D11Texture2DKHR;
typedef void *KHRpfn_clCreateFromD3D11Texture3DKHR;
typedef void *KHRpfn_clEnqueueAcquireD3D11ObjectsKHR;
typedef void *KHRpfn_clEnqueueReleaseD3D11ObjectsKHR;

/* cl_khr_dx9_media_sharing */
typedef void *KHRpfn_clCreateFromDX9MediaSurfaceKHR;
typedef void *KHRpfn_clEnqueueAcquireDX9MediaSurfacesKHR;
typedef void *KHRpfn_clEnqueueReleaseDX9MediaSurfacesKHR;
typedef void *KHRpfn_clGetDeviceIDsFromDX9MediaAdapterKHR;

#endif

/* OpenCL 1.1 */

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clSetEventCallback)(
    cl_event            /* event */,
    cl_int              /* command_exec_callback_type */,
    void (CL_CALLBACK * /* pfn_notify */)(cl_event, cl_int, void *),
    void *              /* user_data */) CL_API_SUFFIX__VERSION_1_1;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *KHRpfn_clCreateSubBuffer)(
    cl_mem                   /* buffer */,
    cl_mem_flags             /* flags */,
    cl_buffer_create_type    /* buffer_create_type */,
    const void *             /* buffer_create_info */,
    cl_int *                 /* errcode_ret */) CL_API_SUFFIX__VERSION_1_1;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clSetMemObjectDestructorCallback)(
    cl_mem /* memobj */,
    void (CL_CALLBACK * /*pfn_notify*/)( cl_mem /* memobj */, void* /*user_data*/),
    void * /*user_data */ ) CL_API_SUFFIX__VERSION_1_1;

typedef CL_API_ENTRY cl_event (CL_API_CALL *KHRpfn_clCreateUserEvent)(
    cl_context    /* context */,
    cl_int *      /* errcode_ret */) CL_API_SUFFIX__VERSION_1_1;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clSetUserEventStatus)(
    cl_event   /* event */,
    cl_int     /* execution_status */) CL_API_SUFFIX__VERSION_1_1;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clCreateSubDevicesEXT)(
    cl_device_id     in_device,
    const cl_device_partition_property_ext * partition_properties,
    cl_uint          num_entries,
    cl_device_id *   out_devices,
    cl_uint *        num_devices);

typedef CL_API_ENTRY cl_int (CL_API_CALL * KHRpfn_clRetainDeviceEXT)(
    cl_device_id     device) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL * KHRpfn_clReleaseDeviceEXT)(
    cl_device_id     device) CL_API_SUFFIX__VERSION_1_0;

/* cl_khr_egl_image */
typedef CL_API_ENTRY cl_mem (CL_API_CALL *KHRpfn_clCreateFromEGLImageKHR)(
    cl_context context,
    CLeglDisplayKHR display,
    CLeglImageKHR image,
    cl_mem_flags flags,
    const cl_egl_image_properties_khr *properties,
    cl_int *errcode_ret);

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueAcquireEGLObjectsKHR)(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem *mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event *event_wait_list,
    cl_event *event);

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueReleaseEGLObjectsKHR)(
    cl_command_queue command_queue,
    cl_uint num_objects,
    const cl_mem *mem_objects,
    cl_uint num_events_in_wait_list,
    const cl_event *event_wait_list,
    cl_event *event);

/* cl_khr_egl_event */
typedef CL_API_ENTRY cl_event (CL_API_CALL *KHRpfn_clCreateEventFromEGLSyncKHR)(
    cl_context context,
    CLeglSyncKHR sync,
    CLeglDisplayKHR display,
    cl_int *errcode_ret);

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clSetDefaultDeviceCommandQueue)(
    cl_context context,
    cl_device_id device,
    cl_command_queue command_queue) CL_API_SUFFIX__VERSION_2_1;

typedef CL_API_ENTRY cl_program (CL_API_CALL *KHRpfn_clCreateProgramWithIL)(
    cl_context context,
    const void * il,
    size_t length,
    cl_int * errcode_ret) CL_API_SUFFIX__VERSION_2_1;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetKernelSubGroupInfo )(
    cl_kernel kernel,
    cl_device_id device,
    cl_kernel_sub_group_info param_name,
    size_t input_value_size,
    const void * input_value,
    size_t param_value_size,
    void * param_value,
    size_t * param_value_size_ret) CL_API_SUFFIX__VERSION_2_1;

typedef CL_API_ENTRY cl_kernel (CL_API_CALL *KHRpfn_clCloneKernel)(
    cl_kernel source_kernel,
    cl_int * errcode_ret) CL_API_SUFFIX__VERSION_2_1;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clEnqueueSVMMigrateMem)(
    cl_command_queue command_queue,
    cl_uint num_svm_pointers,
    const void ** svm_pointers,
    const size_t * sizes,
    cl_mem_migration_flags flags,
    cl_uint num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event * event) CL_API_SUFFIX__VERSION_2_1;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetDeviceAndHostTimer)(
    cl_device_id device,
    cl_ulong * device_timestamp,
    cl_ulong * host_timestamp) CL_API_SUFFIX__VERSION_2_1;

typedef CL_API_ENTRY cl_int (CL_API_CALL *KHRpfn_clGetHostTimer)(
    cl_device_id device,
    cl_ulong * host_timestamp) CL_API_SUFFIX__VERSION_2_1;

/*
 *
 * vendor dispatch table structure
 *
 * note that the types in the structure KHRicdVendorDispatch mirror the function
 * names listed in the string table khrIcdVendorDispatchFunctionNames
 *
 */

typedef struct KHRicdVendorDispatchRec KHRicdVendorDispatch;

struct KHRicdVendorDispatchRec
{
    /* OpenCL 1.0 */
    KHRpfn_clGetPlatformIDs                         clGetPlatformIDs;
    KHRpfn_clGetPlatformInfo                        clGetPlatformInfo;
    KHRpfn_clGetDeviceIDs                           clGetDeviceIDs;
    KHRpfn_clGetDeviceInfo                          clGetDeviceInfo;
    KHRpfn_clCreateContext                          clCreateContext;
    KHRpfn_clCreateContextFromType                  clCreateContextFromType;
    KHRpfn_clRetainContext                          clRetainContext;
    KHRpfn_clReleaseContext                         clReleaseContext;
    KHRpfn_clGetContextInfo                         clGetContextInfo;
    KHRpfn_clCreateCommandQueue                     clCreateCommandQueue;
    KHRpfn_clRetainCommandQueue                     clRetainCommandQueue;
    KHRpfn_clReleaseCommandQueue                    clReleaseCommandQueue;
    KHRpfn_clGetCommandQueueInfo                    clGetCommandQueueInfo;
    KHRpfn_clSetCommandQueueProperty                clSetCommandQueueProperty;
    KHRpfn_clCreateBuffer                           clCreateBuffer;
    KHRpfn_clCreateImage2D                          clCreateImage2D;
    KHRpfn_clCreateImage3D                          clCreateImage3D;
    KHRpfn_clRetainMemObject                        clRetainMemObject;
    KHRpfn_clReleaseMemObject                       clReleaseMemObject;
    KHRpfn_clGetSupportedImageFormats               clGetSupportedImageFormats;
    KHRpfn_clGetMemObjectInfo                       clGetMemObjectInfo;
    KHRpfn_clGetImageInfo                           clGetImageInfo;
    KHRpfn_clCreateSampler                          clCreateSampler;
    KHRpfn_clRetainSampler                          clRetainSampler;
    KHRpfn_clReleaseSampler                         clReleaseSampler;
    KHRpfn_clGetSamplerInfo                         clGetSamplerInfo;
    KHRpfn_clCreateProgramWithSource                clCreateProgramWithSource;
    KHRpfn_clCreateProgramWithBinary                clCreateProgramWithBinary;
    KHRpfn_clRetainProgram                          clRetainProgram;
    KHRpfn_clReleaseProgram                         clReleaseProgram;
    KHRpfn_clBuildProgram                           clBuildProgram;
    KHRpfn_clUnloadCompiler                         clUnloadCompiler;
    KHRpfn_clGetProgramInfo                         clGetProgramInfo;
    KHRpfn_clGetProgramBuildInfo                    clGetProgramBuildInfo;
    KHRpfn_clCreateKernel                           clCreateKernel;
    KHRpfn_clCreateKernelsInProgram                 clCreateKernelsInProgram;
    KHRpfn_clRetainKernel                           clRetainKernel;
    KHRpfn_clReleaseKernel                          clReleaseKernel;
    KHRpfn_clSetKernelArg                           clSetKernelArg;
    KHRpfn_clGetKernelInfo                          clGetKernelInfo;
    KHRpfn_clGetKernelWorkGroupInfo                 clGetKernelWorkGroupInfo;
    KHRpfn_clWaitForEvents                          clWaitForEvents;
    KHRpfn_clGetEventInfo                           clGetEventInfo;
    KHRpfn_clRetainEvent                            clRetainEvent;
    KHRpfn_clReleaseEvent                           clReleaseEvent;
    KHRpfn_clGetEventProfilingInfo                  clGetEventProfilingInfo;
    KHRpfn_clFlush                                  clFlush;
    KHRpfn_clFinish                                 clFinish;
    KHRpfn_clEnqueueReadBuffer                      clEnqueueReadBuffer;
    KHRpfn_clEnqueueWriteBuffer                     clEnqueueWriteBuffer;
    KHRpfn_clEnqueueCopyBuffer                      clEnqueueCopyBuffer;
    KHRpfn_clEnqueueReadImage                       clEnqueueReadImage;
    KHRpfn_clEnqueueWriteImage                      clEnqueueWriteImage;
    KHRpfn_clEnqueueCopyImage                       clEnqueueCopyImage;
    KHRpfn_clEnqueueCopyImageToBuffer               clEnqueueCopyImageToBuffer;
    KHRpfn_clEnqueueCopyBufferToImage               clEnqueueCopyBufferToImage;
    KHRpfn_clEnqueueMapBuffer                       clEnqueueMapBuffer;
    KHRpfn_clEnqueueMapImage                        clEnqueueMapImage;
    KHRpfn_clEnqueueUnmapMemObject                  clEnqueueUnmapMemObject;
    KHRpfn_clEnqueueNDRangeKernel                   clEnqueueNDRangeKernel;
    KHRpfn_clEnqueueTask                            clEnqueueTask;
    KHRpfn_clEnqueueNativeKernel                    clEnqueueNativeKernel;
    KHRpfn_clEnqueueMarker                          clEnqueueMarker;
    KHRpfn_clEnqueueWaitForEvents                   clEnqueueWaitForEvents;
    KHRpfn_clEnqueueBarrier                         clEnqueueBarrier;
    KHRpfn_clGetExtensionFunctionAddress            clGetExtensionFunctionAddress;
    KHRpfn_clCreateFromGLBuffer                     clCreateFromGLBuffer;
    KHRpfn_clCreateFromGLTexture2D                  clCreateFromGLTexture2D;
    KHRpfn_clCreateFromGLTexture3D                  clCreateFromGLTexture3D;
    KHRpfn_clCreateFromGLRenderbuffer               clCreateFromGLRenderbuffer;
    KHRpfn_clGetGLObjectInfo                        clGetGLObjectInfo;
    KHRpfn_clGetGLTextureInfo                       clGetGLTextureInfo;
    KHRpfn_clEnqueueAcquireGLObjects                clEnqueueAcquireGLObjects;
    KHRpfn_clEnqueueReleaseGLObjects                clEnqueueReleaseGLObjects;
    KHRpfn_clGetGLContextInfoKHR                    clGetGLContextInfoKHR;

    /* cl_khr_d3d10_sharing */
    KHRpfn_clGetDeviceIDsFromD3D10KHR               clGetDeviceIDsFromD3D10KHR;
    KHRpfn_clCreateFromD3D10BufferKHR               clCreateFromD3D10BufferKHR;
    KHRpfn_clCreateFromD3D10Texture2DKHR            clCreateFromD3D10Texture2DKHR;
    KHRpfn_clCreateFromD3D10Texture3DKHR            clCreateFromD3D10Texture3DKHR;
    KHRpfn_clEnqueueAcquireD3D10ObjectsKHR          clEnqueueAcquireD3D10ObjectsKHR;
    KHRpfn_clEnqueueReleaseD3D10ObjectsKHR          clEnqueueReleaseD3D10ObjectsKHR;

    /* OpenCL 1.1 */
    KHRpfn_clSetEventCallback                       clSetEventCallback;
    KHRpfn_clCreateSubBuffer                        clCreateSubBuffer;
    KHRpfn_clSetMemObjectDestructorCallback         clSetMemObjectDestructorCallback;
    KHRpfn_clCreateUserEvent                        clCreateUserEvent;
    KHRpfn_clSetUserEventStatus                     clSetUserEventStatus;
    KHRpfn_clEnqueueReadBufferRect                  clEnqueueReadBufferRect;
    KHRpfn_clEnqueueWriteBufferRect                 clEnqueueWriteBufferRect;
    KHRpfn_clEnqueueCopyBufferRect                  clEnqueueCopyBufferRect;

    /* cl_ext_device_fission */
    KHRpfn_clCreateSubDevicesEXT                    clCreateSubDevicesEXT;
    KHRpfn_clRetainDeviceEXT                        clRetainDeviceEXT;
    KHRpfn_clReleaseDeviceEXT                       clReleaseDeviceEXT;

    /* cl_khr_gl_event */
    KHRpfn_clCreateEventFromGLsyncKHR               clCreateEventFromGLsyncKHR;

    /* OpenCL 1.2 */
    KHRpfn_clCreateSubDevices                       clCreateSubDevices;
    KHRpfn_clRetainDevice                           clRetainDevice;
    KHRpfn_clReleaseDevice                          clReleaseDevice;
    KHRpfn_clCreateImage                            clCreateImage;
    KHRpfn_clCreateProgramWithBuiltInKernels        clCreateProgramWithBuiltInKernels;
    KHRpfn_clCompileProgram                         clCompileProgram;
    KHRpfn_clLinkProgram                            clLinkProgram;
    KHRpfn_clUnloadPlatformCompiler                 clUnloadPlatformCompiler;
    KHRpfn_clGetKernelArgInfo                       clGetKernelArgInfo;
    KHRpfn_clEnqueueFillBuffer                      clEnqueueFillBuffer;
    KHRpfn_clEnqueueFillImage                       clEnqueueFillImage;
    KHRpfn_clEnqueueMigrateMemObjects               clEnqueueMigrateMemObjects;
    KHRpfn_clEnqueueMarkerWithWaitList              clEnqueueMarkerWithWaitList;
    KHRpfn_clEnqueueBarrierWithWaitList             clEnqueueBarrierWithWaitList;
    KHRpfn_clGetExtensionFunctionAddressForPlatform clGetExtensionFunctionAddressForPlatform;
    KHRpfn_clCreateFromGLTexture                    clCreateFromGLTexture;

    /* cl_khr_d3d11_sharing */
    KHRpfn_clGetDeviceIDsFromD3D11KHR               clGetDeviceIDsFromD3D11KHR;
    KHRpfn_clCreateFromD3D11BufferKHR               clCreateFromD3D11BufferKHR;
    KHRpfn_clCreateFromD3D11Texture2DKHR            clCreateFromD3D11Texture2DKHR;
    KHRpfn_clCreateFromD3D11Texture3DKHR            clCreateFromD3D11Texture3DKHR;
    KHRpfn_clCreateFromDX9MediaSurfaceKHR           clCreateFromDX9MediaSurfaceKHR;
    KHRpfn_clEnqueueAcquireD3D11ObjectsKHR          clEnqueueAcquireD3D11ObjectsKHR;
    KHRpfn_clEnqueueReleaseD3D11ObjectsKHR          clEnqueueReleaseD3D11ObjectsKHR;

    /* cl_khr_dx9_media_sharing */
    KHRpfn_clGetDeviceIDsFromDX9MediaAdapterKHR     clGetDeviceIDsFromDX9MediaAdapterKHR;
    KHRpfn_clEnqueueAcquireDX9MediaSurfacesKHR      clEnqueueAcquireDX9MediaSurfacesKHR;
    KHRpfn_clEnqueueReleaseDX9MediaSurfacesKHR      clEnqueueReleaseDX9MediaSurfacesKHR;

    /* cl_khr_egl_image */
    KHRpfn_clCreateFromEGLImageKHR                  clCreateFromEGLImageKHR;
    KHRpfn_clEnqueueAcquireEGLObjectsKHR            clEnqueueAcquireEGLObjectsKHR;
    KHRpfn_clEnqueueReleaseEGLObjectsKHR            clEnqueueReleaseEGLObjectsKHR;

    /* cl_khr_egl_event */
    KHRpfn_clCreateEventFromEGLSyncKHR              clCreateEventFromEGLSyncKHR;

    /* OpenCL 2.0 */
    KHRpfn_clCreateCommandQueueWithProperties       clCreateCommandQueueWithProperties;
    KHRpfn_clCreatePipe                             clCreatePipe;
    KHRpfn_clGetPipeInfo                            clGetPipeInfo;
    KHRpfn_clSVMAlloc                               clSVMAlloc;
    KHRpfn_clSVMFree                                clSVMFree;
    KHRpfn_clEnqueueSVMFree                         clEnqueueSVMFree;
    KHRpfn_clEnqueueSVMMemcpy                       clEnqueueSVMMemcpy;
    KHRpfn_clEnqueueSVMMemFill                      clEnqueueSVMMemFill;
    KHRpfn_clEnqueueSVMMap                          clEnqueueSVMMap;
    KHRpfn_clEnqueueSVMUnmap                        clEnqueueSVMUnmap;
    KHRpfn_clCreateSamplerWithProperties            clCreateSamplerWithProperties;
    KHRpfn_clSetKernelArgSVMPointer                 clSetKernelArgSVMPointer;
    KHRpfn_clSetKernelExecInfo                      clSetKernelExecInfo;

    /* cl_khr_sub_groups */
    KHRpfn_clGetKernelSubGroupInfoKHR               clGetKernelSubGroupInfoKHR;

    /* OpenCL 2.1 */
    KHRpfn_clCloneKernel                            clCloneKernel;
    KHRpfn_clCreateProgramWithIL                    clCreateProgramWithIL;
    KHRpfn_clEnqueueSVMMigrateMem                   clEnqueueSVMMigrateMem;
    KHRpfn_clGetDeviceAndHostTimer                  clGetDeviceAndHostTimer;
    KHRpfn_clGetHostTimer                           clGetHostTimer;
    KHRpfn_clGetKernelSubGroupInfo                  clGetKernelSubGroupInfo;
    KHRpfn_clSetDefaultDeviceCommandQueue           clSetDefaultDeviceCommandQueue;

    /* OpenCL 2.2 */
    KHRpfn_clSetProgramReleaseCallback              clSetProgramReleaseCallback;
    KHRpfn_clSetProgramSpecializationConstant       clSetProgramSpecializationConstant;
};

#endif // _ICD_DISPATCH_H_
