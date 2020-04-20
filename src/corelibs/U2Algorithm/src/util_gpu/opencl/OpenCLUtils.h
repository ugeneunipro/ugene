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

#ifndef __OPENCL_UTILS_H__
#define __OPENCL_UTILS_H__

#ifdef OPENCL_SUPPORT

#include "OpenCLHelper.h"

#include <QString>

namespace U2 {

class U2ALGORITHM_EXPORT OpenCLUtils {
public:
    OpenCLUtils();

    static cl_program createProgramByResource(
                    cl_context clContext,
                    cl_device_id deviceId,
                    const QString& resourceName,
                    const OpenCLHelper& openCLHelper,
                    cl_int& err);

    static size_t getPreferredWorkGroupSize(
                    cl_kernel kernel,
                    cl_device_id deviceId,
                    const OpenCLHelper& openCLHelper,
                    cl_int &err); // performance hint, returnes 32 if error, ignoring err
};

}//namespace

#endif /*OPENCL_SUPPORT*/

#endif //__OPENCL_UTILS_H__
