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

#ifndef _U2_CUFFLINKS_SUPPORT_TASK_H_
#define _U2_CUFFLINKS_SUPPORT_TASK_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/Task.h>

#include <U2Formats/ConvertAssemblyToSamTask.h>

#include "CufflinksSettings.h"
#include "CufflinksSupport.h"

namespace U2 {

class AnnotationTableObject;
class LoadDocumentTask;

enum CufflinksOutputFormat {
    CufflinksOutputFpkm,
    CufflinksOutputGtf
};

class CufflinksSupportTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    CufflinksSupportTask(const CufflinksSettings& settings);
    ~CufflinksSupportTask();

    void prepare();
    QList<Task*> onSubTaskFinished(Task* subTask);
    ReportResult report();

    QList<AnnotationTableObject *> getIsoformAnnotationTables() const;
    const QStringList &getOutputFiles() const;

private:
    static DocumentFormatId getFormatId(CufflinksOutputFormat format);
    void initLoadIsoformAnnotationsTask(const QString &fileName, CufflinksOutputFormat format);

    CufflinksSettings                   settings;

    QPointer<Document>                  tmpDoc;
    QString                             workingDirectory;
    QString                             url;

    ConvertAssemblyToSamTask *          convertAssToSamTask;
    ExternalToolRunTask *               cufflinksExtToolTask;
    LoadDocumentTask *                  loadIsoformAnnotationsTask;

    QList<AnnotationTableObject *>      isoformLevelAnnotationTables;
    QStringList                         outputFiles;

    static const QString outSubDirBaseName;

private:
    QString initTmpDir();
    ExternalToolRunTask * runCufflinks();
};



}   // namespace U2


#endif // _U2_CUFFLINKS_SUPPORT_TASK_H_
