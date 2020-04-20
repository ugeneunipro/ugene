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

#ifdef OPENCL_SUPPORT

#include "OpenCLUtils.h"

#include <QFile>
#include <QTextStream>
#include <U2Core/GAutoDeleteList.h>

#include <U2Core/Log.h>

namespace U2 {

OpenCLUtils::OpenCLUtils(){}

cl_program OpenCLUtils::createProgramByResource(
                cl_context clContext,
                cl_device_id deviceId,
                const QString& resourceName,
                const OpenCLHelper& openCLHelper,
                cl_int& err) {
    //open and read file contains OPENCL code
    QByteArray file;
    QFile data(resourceName);

    if (data.open(QFile::ReadOnly)) {
        file = data.readAll();
        data.close();
    } else {
        ioLog.error(QString("No source file: %1").arg(data.errorString()));
        return 0;
    }

    const char* sourceCode = file.constData();
    const size_t sourceLength = file.size();
    cl_program clProgram = openCLHelper.clCreateProgramWithSource_p(clContext, 1, &sourceCode,
        &sourceLength, &err);

    if(err != 0) {
        coreLog.error(QString("OPENCL: clCreateProgramWithSource, Error code (%2)").arg(err));
        return 0;
    }

    err = openCLHelper.clBuildProgram_p(clProgram, 0, NULL, NULL, NULL, NULL);

    if (err != CL_SUCCESS) {
        size_t logSize = 1;
        openCLHelper.clGetProgramBuildInfo_p(clProgram, deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
        gauto_array<char> logStr(new char[logSize]);
        openCLHelper.clGetProgramBuildInfo_p(clProgram, deviceId, CL_PROGRAM_BUILD_LOG, logSize, logStr.get(), &logSize);

        coreLog.error(QObject::tr("OPENCL: BUILD LOG \n ******************** \n %1 \n ********************").arg(logStr.get()));
        coreLog.error(QObject::tr("OPENCL: Program::build() failed. (%1)").arg(err));
        return 0;
    }

    return clProgram;
}

size_t OpenCLUtils::getPreferredWorkGroupSize(
                cl_kernel kernel,
                cl_device_id deviceId,
                const OpenCLHelper& openCLHelper,
                cl_int &/*err*/) {

    cl_int err2 = 0;
    size_t preferredWorkGroupSize = 0;
    err2 |= openCLHelper.clGetKernelWorkGroupInfo_p(kernel, deviceId, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE,
        sizeof(size_t), &preferredWorkGroupSize, NULL);

    if (err2 != CL_SUCCESS) {
        preferredWorkGroupSize = 32; // set default value to prevent calculation error because of this "performance hint" error
    }

    return preferredWorkGroupSize;
}

}//namespace

#endif /*OPENCL_SUPPORT*/
