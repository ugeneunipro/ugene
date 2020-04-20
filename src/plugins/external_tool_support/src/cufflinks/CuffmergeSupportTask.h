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

#ifndef _U2_CUFFMERGESUPPORTTASK_H_
#define _U2_CUFFMERGESUPPORTTASK_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/Task.h>

#include <U2Lang/DbiDataStorage.h>

namespace U2 {

class AnnotationTableObject;
class LoadDocumentTask;

struct CuffmergeSettings {
    CuffmergeSettings();

    double minIsoformFraction;
    QString refAnnsUrl;
    QString refSeqUrl;
    QString outDir;
    QString workingDir;

    Workflow::DbiDataStorage *storage;
    QList<Workflow::SharedDbiDataHandler> annotationTables;
};

class CuffmergeSupportTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    CuffmergeSupportTask(const CuffmergeSettings &settings);
    ~CuffmergeSupportTask();

    void prepare();
    QList<Task*> onSubTaskFinished(Task *subTask);
    void run();
    QStringList getOutputFiles() const;

    QList<AnnotationTableObject *> takeResult();

private:
    CuffmergeSettings settings;
    QString workingDir;
    QString listFilePath;
    int fileNum;
    QList<Document*> docs;
    QList<Task*> writeTasks;
    ExternalToolRunTask *mergeTask;
    LoadDocumentTask *loadResultTask;
    QList<AnnotationTableObject *> result;
    QStringList outputFiles;

    static const QString outSubDirBaseName;

private:
    Document *prepareDocument(const Workflow::SharedDbiDataHandler &annTableHandler, const QString &filePath);
    Task *createWriteTask(const Workflow::SharedDbiDataHandler &annTableHandler, const QString &filePath);
    Task *createCuffmergeTask();
    LoadDocumentTask *createLoadResultDocumentTask(const QString &fileName);
    QString getAnnsFilePath();
    void setupWorkingDirPath();
    void writeFileList();
};

} // U2

#endif // _U2_CUFFMERGESUPPORTTASK_H_
