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


/** Set of wrappers for typical workflow tasks */

#ifndef _SIMPLE_WORKFLOW_TASK_H_
#define _SIMPLE_WORKFLOW_TASK_H_

#include <QTemporaryFile>

#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentProviderTask.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/Task.h>

#include <U2Lang/Schema.h>
#include <U2Lang/WorkflowIOTasks.h>

namespace U2 {

using namespace Workflow;

class CmdlineTaskRunner;
class LoadDocumentTask;
class MultipleSequenceAlignmentObject;

class U2LANG_EXPORT SimpleInOutWorkflowTaskConfig {
public:
    SimpleInOutWorkflowTaskConfig();
    QList<GObject*>     objects;
    DocumentFormatId    inFormat;
    QVariantMap         inDocHints;
    DocumentFormatId    outFormat;
    QVariantMap         outDocHints;
    QStringList         extraArgs;
    QString             schemaName;
    bool                emptyResultPossible;
};

/**
    Runs workflow in a separate process and handles in-out parameters.
    The result is output document.
    If problems occur during the workflow execution, only the first error is detected and warnings are skipped.
*/
class U2LANG_EXPORT SimpleInOutWorkflowTask : public DocumentProviderTask {
    Q_OBJECT
public:
    SimpleInOutWorkflowTask(const SimpleInOutWorkflowTaskConfig& conf);
    void prepare();
    virtual QList<Task*> onSubTaskFinished(Task* subTask);

private:
    void prepareTmpFile(QTemporaryFile& tmpFile, const QString& tmpl);

    SimpleInOutWorkflowTaskConfig        conf;

    Document*                           inDoc;

    SaveDocumentTask*                   saveInputTask;
    QTemporaryFile                      inputTmpFile;

    CmdlineTaskRunner*                  runWorkflowTask;

    QTemporaryFile                      resultTmpFile;
    LoadDocumentTask*                   loadResultTask;

    QString                             schemaPath;

};


class SimpleMSAWorkflowTaskConfig {
public:
    QString     schemaName;
    QStringList schemaArgs;
    QVariantMap resultDocHints;
};

class U2LANG_EXPORT SimpleMSAWorkflow4GObjectTask : public Task {
    Q_OBJECT

public:
    SimpleMSAWorkflow4GObjectTask(const QString& taskName, MultipleSequenceAlignmentObject* maObj, const SimpleMSAWorkflowTaskConfig& conf);

    void prepare();
    ReportResult report();
    MultipleSequenceAlignment getResult();

private:
    QPointer<MultipleSequenceAlignmentObject>  msaObjectPointer;
    QString                     docName;
    SimpleMSAWorkflowTaskConfig conf;
    SimpleInOutWorkflowTask*    runWorkflowTask;
};

}    // namespace U2

#endif    // #ifndef _RUN_SCHEMA_FOR_TASK_H_
