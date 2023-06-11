/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#pragma once

#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/Task.h>

#include "utils/ExportTasks.h"

namespace U2 {

class KalignSupportTaskSettings {
public:
    KalignSupportTaskSettings();
    void reset();

    double gapOpenPenalty;
    double gapExtenstionPenalty;
    double termGapPenalty;
    double secret;
    QString inputFilePath;
    QString outputFilePath;
};

class LoadDocumentTask;

class KalignSupportTask : public ExternalToolSupportTask {
    Q_OBJECT
    Q_DISABLE_COPY(KalignSupportTask)
public:
    KalignSupportTask(const MultipleSequenceAlignment& _inputMsa, const GObjectReference& _objRef, const KalignSupportTaskSettings& _settings);
    ~KalignSupportTask() override;

    void prepare() override;
    Task::ReportResult report() override;

    QList<Task*> onSubTaskFinished(Task* subTask) override;

    MultipleSequenceAlignment resultMA;

private:
    MultipleSequenceAlignment inputMsa;
    GObjectReference objRef;
    QPointer<Document> tmpDoc;
    QString url;

    ExternalToolRunTask* KalignTask;
    LoadDocumentTask* loadTmpDocumentTask;
    KalignSupportTaskSettings settings;
    QPointer<StateLock> lock;
};

class MultipleSequenceAlignmentObject;

class KalignWithExtFileSpecifySupportTask : public Task {
    Q_OBJECT
    Q_DISABLE_COPY(KalignWithExtFileSpecifySupportTask)
public:
    KalignWithExtFileSpecifySupportTask(const KalignSupportTaskSettings& settings);
    ~KalignWithExtFileSpecifySupportTask() override;
    void prepare() override;
    Task::ReportResult report() override;

    QList<Task*> onSubTaskFinished(Task* subTask) override;

private:
    MultipleSequenceAlignmentObject* mAObject;
    Document* currentDocument;
    bool cleanDoc;

    SaveDocumentTask* saveDocumentTask;
    LoadDocumentTask* loadDocumentTask;
    KalignSupportTask* KalignSupportTask;
    KalignSupportTaskSettings settings;
};

class KalignLogParser : public ExternalToolLogParser {
public:
    KalignLogParser();

    int getProgress() override;
    void parseOutput(const QString& partOfLog) override;
    void parseErrOutput(const QString& partOfLog) override;

private:
    QString lastErrLine;
    int progress;
};

}  // namespace U2
