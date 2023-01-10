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

#ifndef _U2_CLUSTALO_SUPPORT_TASK_H
#define _U2_CLUSTALO_SUPPORT_TASK_H

#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/Task.h>

#include "utils/ExportTasks.h"

namespace U2 {

class LoadDocumentTask;

/** ClustalO settings. See http://www.clustal.org/omega/README for details. */
class ClustalOSupportTaskSettings {
public:
    ClustalOSupportTaskSettings();

    /**  --iterations, --iter=<n>.  Number of (combined guide-tree/HMM) iterations. -1 -> no option. */
    int numIterations = -1;

    /**  --max-guidetree-iterations=<n> Maximum number guidetree iterations. -1 -> no option. */
    int maxGuideTreeIterations = -1;

    /**  --max-hmm-iterations=<n> Maximum number of HMM iterations . -1 -> no option. */
    int maxHMMIterations = -1;

    /**  --auto. Set options automatically (might overwrite some of your options). */
    bool setAutoOptions = false;

    // --threads=<n>. Number of processors to use. <=0 -> no option. */
    int numberOfProcessors = -1;

    // -i, --in, --infile={<file>,-}. Multiple sequence input file.
    QString inputFilePath;

    /** -o, --out, --outfile={file,-}. Multiple sequence alignment output file.  */
    QString outputFilePath;
};

class ClustalOSupportTask : public ExternalToolSupportTask {
    Q_OBJECT
    Q_DISABLE_COPY(ClustalOSupportTask)
public:
    /** Initializes ClustalO task that calls ClustalO to align 'msa' and saves the result to 'objRef'. */
    ClustalOSupportTask(const MultipleSequenceAlignment& inputMsa,
                        const GObjectReference& objRef,
                        const ClustalOSupportTaskSettings& settings);

    /**
     * Initializes ClustalO task that calls ClustalO to align 'msa' with another alignment from 'secondAlignmentFileUrl'
     * and saves the result to 'objRef'.
     */
    ClustalOSupportTask(const MultipleSequenceAlignment& inputMsa,
                        const GObjectReference& objRef,
                        const QString& secondAlignmentFileUrl,
                        const ClustalOSupportTaskSettings& settings);

    ~ClustalOSupportTask();

    void prepare() override;

    Task::ReportResult report() override;

    QList<Task*> onSubTaskFinished(Task* subTask) override;

    /** Returns result multiple alignment. The result is non-empty only for successfully finished task. */
    const MultipleSequenceAlignment& getResultAlignment() const;

private:
    /** Removes all object locks set by the task.*/
    void unlockMsaObject();

    MultipleSequenceAlignment inputMsa;
    MultipleSequenceAlignment resultMsa;
    GObjectReference objRef;
    QPointer<Document> tmpDoc;

    SaveAlignmentTask* saveTemporaryDocumentTask = nullptr;
    ExternalToolRunTask* clustalOTask = nullptr;
    LoadDocumentTask* loadTemporaryDocumentTask = nullptr;
    ClustalOSupportTaskSettings settings;
    QPointer<StateLock> lock;

    QString inputMsaTmpFileUrl;
    QString secondAlignmentFileUrl;
};

class ClustalOWithExtFileSpecifySupportTask : public Task {
    Q_OBJECT
    Q_DISABLE_COPY(ClustalOWithExtFileSpecifySupportTask)
public:
    ClustalOWithExtFileSpecifySupportTask(const ClustalOSupportTaskSettings& settings);
    ~ClustalOWithExtFileSpecifySupportTask();

    void prepare() override;

    Task::ReportResult report() override;

    QList<Task*> onSubTaskFinished(Task* subTask) override;

private:
    MultipleSequenceAlignmentObject* mAObject = nullptr;
    Document* currentDocument = nullptr;
    bool cleanDoc = true;

    SaveDocumentTask* saveDocumentTask = nullptr;
    LoadDocumentTask* loadDocumentTask = nullptr;
    ClustalOSupportTask* clustalOSupportTask = nullptr;
    ClustalOSupportTaskSettings settings;
};

class ClustalOLogParser : public ExternalToolLogParser {
public:
    ClustalOLogParser();
    void parseOutput(const QString& partOfLog);
    int getProgress();

private:
    /* Last line printed to stdout */
    QString lastLine;
};

}  // namespace U2
#endif  // _U2_CLUSTALO_SUPPORT_TASK_H
