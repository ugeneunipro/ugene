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

#ifndef _U2_CONVERT_ASSEMBLY_TO_SAM_TASK_H_
#define _U2_CONVERT_ASSEMBLY_TO_SAM_TASK_H_

#include <U2Core/AssemblyObject.h>
#include <U2Core/Task.h>
#include <U2Core/GUrl.h>

namespace U2 {

class DbiConnection;

class U2FORMATS_EXPORT ConvertAssemblyToSamTask : public Task {
    Q_OBJECT
public:
    ConvertAssemblyToSamTask(GUrl dbFileUrl, GUrl samFileUrl);
    ConvertAssemblyToSamTask(const DbiConnection *handle, GUrl samFileUrl);

    /** Converts only one assembly object to a SAM file */
    ConvertAssemblyToSamTask(const U2EntityRef& entityRef, GUrl samFileUrl);

    void run();
    QString generateReport() const;

private:
    GUrl dbFileUrl;
    GUrl samFileUrl;

    U2EntityRef assemblyEntityRef;

    const DbiConnection *handle;
};

}// U2

#endif //_U2_CONVERT_ASSEMBLY_TO_SAM_TASK_H_
