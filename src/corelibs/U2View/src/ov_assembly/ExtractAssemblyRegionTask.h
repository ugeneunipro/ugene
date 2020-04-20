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

#ifndef _U2_EXTRACT_ASSEMBLY_TASK_H_
#define _U2_EXTRACT_ASSEMBLY_TASK_H_

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Task.h>
#include <U2Core/U2Region.h>

namespace U2 {

class AssemblyObject;
class DocumentProviderTask;
class ExtractAssemblyRegionTask;

struct ExtractAssemblyRegionTaskSettings {
    ExtractAssemblyRegionTaskSettings(QString fileUrl, int assemblyLength, AssemblyObject *o) : fileUrl(fileUrl), regionToExtract(U2_REGION_MAX),
        fileFormat(BaseDocumentFormats::UGENEDB), assemblyLength(assemblyLength), obj(o), addToProject(false) {}
    QString fileUrl;
    U2Region regionToExtract;
    DocumentFormatId fileFormat;
    int assemblyLength;
    AssemblyObject *obj;
    bool addToProject;
};

class ExtractAssemblyRegionAndOpenViewTask : public Task {
    Q_OBJECT
public:
    ExtractAssemblyRegionAndOpenViewTask(const ExtractAssemblyRegionTaskSettings& settings);

    void prepare();

    QList<Task*>  onSubTaskFinished(Task *subTask);
private:
    ExtractAssemblyRegionTaskSettings settings;
    ExtractAssemblyRegionTask *extractTask;
};

class ExtractAssemblyRegionTask : public Task {
    Q_OBJECT
public:
    ExtractAssemblyRegionTask(const ExtractAssemblyRegionTaskSettings& settings);

    void run();
private:
    ExtractAssemblyRegionTaskSettings settings;
};

}

#endif
