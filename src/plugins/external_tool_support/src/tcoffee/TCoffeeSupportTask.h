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

#ifndef _U2_TCOFFEE_SUPPORT_TASK_H
#define _U2_TCOFFEE_SUPPORT_TASK_H


#include "utils/ExportTasks.h"

#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/Task.h>

namespace U2 {

class TCoffeeSupportTaskSettings  {
public:
    TCoffeeSupportTaskSettings() {reset();}
    void reset();

    float   gapOpenPenalty;
    float   gapExtenstionPenalty;
    int     numIterations;
    QString inputFilePath;
    QString outputFilePath;
};

class LoadDocumentTask;

class TCoffeeSupportTask : public ExternalToolSupportTask {
    Q_OBJECT
    Q_DISABLE_COPY(TCoffeeSupportTask)
public:
    TCoffeeSupportTask(const MultipleSequenceAlignment& _inputMsa, const GObjectReference& _objRef, const TCoffeeSupportTaskSettings& _settings);
    ~TCoffeeSupportTask();

    void prepare();
    Task::ReportResult report();

    QList<Task*> onSubTaskFinished(Task* subTask);

    MultipleSequenceAlignment                  resultMA;
private:
    MultipleSequenceAlignment                  inputMsa;
    GObjectReference            objRef;
    QPointer<Document>          tmpDoc;
    QString                     url;

    SaveMSA2SequencesTask*      saveTemporaryDocumentTask;
    ExternalToolRunTask*        tCoffeeTask;
    LoadDocumentTask*           loadTmpDocumentTask;
    TCoffeeSupportTaskSettings  settings;
    QPointer<StateLock>         lock;
};

class MultipleSequenceAlignmentObject;

class TCoffeeWithExtFileSpecifySupportTask : public Task {
    Q_OBJECT
    Q_DISABLE_COPY(TCoffeeWithExtFileSpecifySupportTask)
public:
    TCoffeeWithExtFileSpecifySupportTask(const TCoffeeSupportTaskSettings& settings);
    ~TCoffeeWithExtFileSpecifySupportTask();
    void prepare();
    Task::ReportResult report();

    QList<Task*> onSubTaskFinished(Task* subTask);
private:
    MultipleSequenceAlignmentObject*           mAObject;
    Document*                   currentDocument;
    bool                        cleanDoc;

    SaveDocumentTask*               saveDocumentTask;
    LoadDocumentTask*               loadDocumentTask;
    TCoffeeSupportTask*             tCoffeeSupportTask;
    TCoffeeSupportTaskSettings      settings;
};

class TCoffeeLogParser : public ExternalToolLogParser {
public:
    TCoffeeLogParser();

    int getProgress();
    void parseOutput(const QString& partOfLog);
    void parseErrOutput(const QString& partOfLog);

private:
    QString lastErrLine;
    int     progress;
};

}//namespace
#endif // _U2_TCOFFEE_SUPPORT_TASK_H
