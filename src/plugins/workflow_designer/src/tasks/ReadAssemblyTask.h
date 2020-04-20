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

#ifndef _U2_READ_ASSEMBLY_TASK_
#define _U2_READ_ASSEMBLY_TASK_

#include <U2Lang/ReadDocumentTaskFactory.h>

namespace U2 {

class Document;
class DocumentFormat;
class DocumentProviderTask;

namespace Workflow {

class ConvertToIndexedBamTask;
class WorkflowContext;

class ReadAssemblyTask : public ReadDocumentTask {
    Q_OBJECT
public:
    ReadAssemblyTask(const QString &url, const QString &datasetName, WorkflowContext *ctx);
    virtual void prepare();
    virtual void run();
    virtual QList<Task*> onSubTaskFinished(Task *subTask);

private:
    WorkflowContext *ctx;
    DocumentFormat *format;
    Document *doc;

    ConvertToIndexedBamTask *convertTask;
    DocumentProviderTask *importTask;
};

class ConvertToIndexedBamTask : public Task {
public:
    ConvertToIndexedBamTask(const DocumentFormatId &formatId, const GUrl &url, WorkflowContext *ctx);

    virtual void run();
    GUrl getResultUrl() const;
    const QStringList & getConvertedFiles() const;

private:
    DocumentFormatId formatId;
    GUrl url;
    GUrl result;
    WorkflowContext *ctx;
    QStringList convertedFiles;

private:
    void addConvertedFile(const GUrl &url);
};

class ReadAssemblyTaskFactory : public ReadDocumentTaskFactory {
public:
    ReadAssemblyTaskFactory();

    virtual ReadDocumentTask *createTask(const QString &url, const QVariantMap &hints, WorkflowContext *ctx);
};

} // LocalWorkflow
} // U2

#endif // _U2_READ_ASSEMBLY_TASK_
