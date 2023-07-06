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
class LoadDocumentTask;

class Kalign3Settings {
public:
    static constexpr double VALUE_IS_NOT_SET = -1000000;

    double gapOpenPenalty = VALUE_IS_NOT_SET;
    double gapExtensionPenalty = VALUE_IS_NOT_SET;
    double terminalGapExtensionPenalty = VALUE_IS_NOT_SET;

    int nThreads = 4;

    QString inputFilePath;
    QString outputFilePath;

    /** See set_subm_gaps_CorBLOSUM66_13plus. */
    static Kalign3Settings getDefaultAminoSettings();

    /* See set_subm_gaps_DNA. */
    static Kalign3Settings getDefaultDnaSettings();

    /** See set_subm_gaps_RNA. */
    static Kalign3Settings getDefaultRnaSettings();

    /**
     * Returns settings per alphabet.
     * Returns VALUE_IS_NOT_SET-initialized settings for the unsupported (RAW) alphabet.
     */
    static Kalign3Settings getDefaultSettings(const DNAAlphabet* alphabet);
};

class Kalign3SupportTask : public ExternalToolSupportTask {
    Q_OBJECT
    Q_DISABLE_COPY(Kalign3SupportTask)
public:
    Kalign3SupportTask(const MultipleSequenceAlignment& inputMsa, const GObjectReference& objRef, const Kalign3Settings& settings);
    ~Kalign3SupportTask() override;

    void prepare() override;
    Task::ReportResult report() override;

    QList<Task*> onSubTaskFinished(Task* subTask) override;

    static bool isAlphabetSupported(const QString& alphabetId);

    MultipleSequenceAlignment resultMA;

private:
    MultipleSequenceAlignment inputMsa;
    GObjectReference objRef;
    QPointer<Document> tmpDoc;
    QString url;

    SaveMSA2SequencesTask* saveTemporaryDocumentTask = nullptr;
    ExternalToolRunTask* kalignTask = nullptr;
    LoadDocumentTask* loadTmpDocumentTask = nullptr;
    Kalign3Settings settings;
    QPointer<StateLock> lock;
};

class MultipleSequenceAlignmentObject;

class Kalign3WithExternalFileSupportTask : public Task {
    Q_OBJECT
    Q_DISABLE_COPY(Kalign3WithExternalFileSupportTask)
public:
    Kalign3WithExternalFileSupportTask(const Kalign3Settings& settings);
    ~Kalign3WithExternalFileSupportTask() override;
    void prepare() override;
    Task::ReportResult report() override;

    QList<Task*> onSubTaskFinished(Task* subTask) override;

private:
    MultipleSequenceAlignmentObject* mAObject = nullptr;
    Document* currentDocument = nullptr;
    bool cleanDoc = true;

    SaveDocumentTask* saveDocumentTask = nullptr;
    LoadDocumentTask* loadDocumentTask = nullptr;
    Kalign3SupportTask* kalign3SupportTask = nullptr;
    Kalign3Settings settings;
};

/**
 * Kalign does not share useful progress info in log.
 * The first error is the log contains most meaningful details.
 */
class KalignLogParser : public ExternalToolLogParser {
public:
    void parseErrOutput(const QString& partOfLog) override;
};

}  // namespace U2
