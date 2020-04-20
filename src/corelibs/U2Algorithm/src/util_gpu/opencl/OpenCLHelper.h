/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#ifndef __OPENCL_HELPER__
#define __OPENCL_HELPER__

#ifdef OPENCL_SUPPORT

#include <U2Core/global.h>

#include <QLibrary>
#include <QString>

#include <opencl/legacy/cl.h>

#define OPENCL_DRIVER_LIB "OpenCL"

namespace U2 {

class U2ALGORITHM_EXPORT OpenCLHelper {

public:

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clGetPlatformIDs_f)(
        cl_uint num_entries,
        cl_platform_id *platforms,
        cl_uint *num_platforms) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clGetPlatformInfo_f)(
        cl_platform_id platform,
        cl_platform_info param_name,
        size_t param_value_size,
        void *param_value,
        size_t *param_value_size_ret) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clGetDeviceIDs_f)(
        cl_platform_id platform,
        cl_device_type device_type,
        cl_uint num_entries,
        cl_device_id *devices,
        cl_uint *num_devices) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clGetDeviceInfo_f)(
        cl_device_id device,
        cl_device_info param_name,
        size_t param_value_size,
        void *param_value,
        size_t *param_value_size_ret) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_context ( CL_API_CALL *clCreateContext_f)(
        const cl_context_properties *properties,
        cl_uint num_devices,
        const cl_device_id *devices,
        void (CL_CALLBACK *pfn_notify)(const char *errinfo,
        const void *private_info, size_t cb,
        void *user_data),
        void *user_data,
        cl_int *errcode_ret) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_mem ( CL_API_CALL *clCreateBuffer_f)(
        cl_context context,
        cl_mem_flags flags,
        size_t size,
        void *host_ptr,
        cl_int *errcode_ret) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_program ( CL_API_CALL *clCreateProgramWithSource_f)(
        cl_context context,
        cl_uint count,
        const char **strings,
        const size_t *lengths,
        cl_int *errcode_ret) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clGetProgramBuildInfo_f)(
        cl_program program,
        cl_device_id device,
        cl_program_build_info param_name,
        size_t param_value_size,
        void *param_value,
        size_t *param_value_size_ret) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_kernel ( CL_API_CALL *clCreateKernel_f)(
        cl_program program,
        const char *kernel_name,
        cl_int *errcode_ret) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clSetKernelArg_f)(
        cl_kernel kernel,
        cl_uint arg_index,
        size_t arg_size,
        const void *arg_value) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_command_queue ( CL_API_CALL *clCreateCommandQueue_f)(
        cl_context context,
        cl_device_id device,
        cl_command_queue_properties properties,
        cl_int *errcode_ret) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clEnqueueNDRangeKernel_f)(
        cl_command_queue command_queue,
        cl_kernel kernel,
        cl_uint work_dim,
        const size_t *global_work_offset,
        const size_t *global_work_size,
        const size_t *local_work_size,
        cl_uint num_events_in_wait_list,
        const cl_event *event_wait_list,
        cl_event *event) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clWaitForEvents_f)(
        cl_uint num_events,
        const cl_event *event_list) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clEnqueueReadBuffer_f)(cl_command_queue command_queue,
        cl_mem buffer,
        cl_bool blocking_read,
        size_t offset,
        size_t cb,
        void *ptr,
        cl_uint num_events_in_wait_list,
        const cl_event *event_wait_list,
        cl_event *event) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clFlush_f)(cl_command_queue command_queue) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clFinish_f)(cl_command_queue command_queue) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clReleaseEvent_f)(cl_event event) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clReleaseKernel_f)(cl_kernel kernel) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clReleaseProgram_f)(cl_program program) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clReleaseCommandQueue_f)(cl_command_queue command_queue) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clReleaseContext_f)(cl_context context) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clReleaseMemObject_f)(cl_mem memobj) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clBuildProgram_f)(cl_program program,
        cl_uint num_devices,
        const cl_device_id *device_list,
        const char *options,
        void (CL_CALLBACK *pfn_notify)(cl_program program,
        void *user_data),
        void *user_data) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clGetKernelWorkGroupInfo_f)(
        cl_kernel kernel,
        cl_device_id device,
        cl_kernel_work_group_info param_name,
        size_t param_value_size,
        void *param_value,
        size_t *param_value_size_ret) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int ( CL_API_CALL *clGetEventProfilingInfo_f)(
        cl_event event,
        cl_profiling_info param_name,
        size_t param_value_size,
        void *param_value,
        size_t *param_value_size_ret) CL_API_SUFFIX__VERSION_1_1;


    clGetPlatformIDs_f clGetPlatformIDs_p;
    clGetPlatformInfo_f clGetPlatformInfo_p;
    clGetDeviceIDs_f clGetDeviceIDs_p;
    clGetDeviceInfo_f clGetDeviceInfo_p;

    clCreateContext_f clCreateContext_p;
    clCreateBuffer_f clCreateBuffer_p;
    clCreateProgramWithSource_f clCreateProgramWithSource_p;
    clGetProgramBuildInfo_f clGetProgramBuildInfo_p;
    clCreateKernel_f clCreateKernel_p;
    clSetKernelArg_f clSetKernelArg_p;
    clCreateCommandQueue_f clCreateCommandQueue_p;
    clEnqueueNDRangeKernel_f clEnqueueNDRangeKernel_p;
    clWaitForEvents_f clWaitForEvents_p;
    clEnqueueReadBuffer_f clEnqueueReadBuffer_p;
    clFlush_f clFlush_p;
    clFinish_f clFinish_p;
    clBuildProgram_f clBuildProgram_p;
    clReleaseEvent_f clReleaseEvent_p;

    clReleaseKernel_f clReleaseKernel_p;
    clReleaseProgram_f clReleaseProgram_p;
    clReleaseCommandQueue_f clReleaseCommandQueue_p;
    clReleaseContext_f clReleaseContext_p;
    clReleaseMemObject_f clReleaseMemObject_p;

    clGetKernelWorkGroupInfo_f clGetKernelWorkGroupInfo_p;
    clGetEventProfilingInfo_f clGetEventProfilingInfo_p;

    enum OpenCLHelperError {
        Error_NoError,
        Error_NoDriverLib,
        Error_BadDriverLib,
    };

    bool isLoaded() const {return (status == Error_NoError);}

    QString getErrorString() const;

    OpenCLHelper();
    ~OpenCLHelper();

private:
    OpenCLHelper& operator=( const OpenCLHelper &s );
    OpenCLHelper(const OpenCLHelper &);

private:
    OpenCLHelperError status;
    QLibrary openclLib;
};

}

#endif /* OPENCL_SUPPORT */

#endif //__OPENCL_HELPER__
