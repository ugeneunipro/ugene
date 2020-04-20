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

#ifndef _U2_MAFFT_SUPPORT_TASK_H
#define _U2_MAFFT_SUPPORT_TASK_H

#include <QFile>

#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/Task.h>

#include "utils/ExportTasks.h"

namespace U2 {

/*Options for MAFFT
+ --op # :         Gap opening penalty, default: 1.53
+ --ep # :         Offset (works like gap extension penalty), default: 0.0
+ --maxiterate # : Maximum number of iterative refinement, default: 0
? --clustalout :   Output: clustal format, default: fasta // With this option bad output clustal file is generated.(dkandrov)
? --reorder :      Outorder: aligned, default: input order
? --quiet :        Do not report progress
? --thread # :     Number of threads. (# must be <= number of physical cores - 1)

*/

class LoadDocumentTask;
class MAFFTLogParser;

class MAFFTSupportTaskSettings  {
public:
    MAFFTSupportTaskSettings() {reset();}
    void reset();

    float   gapOpenPenalty;
    float   gapExtenstionPenalty;
    int     maxNumberIterRefinement;
    QString inputFilePath;
    QString outputFilePath;

};


class MAFFTSupportTask : public ExternalToolSupportTask {
    Q_OBJECT
    Q_DISABLE_COPY(MAFFTSupportTask)
public:
    MAFFTSupportTask(const MultipleSequenceAlignment& _inputMsa, const GObjectReference& _objRef, const MAFFTSupportTaskSettings& settings);
    ~MAFFTSupportTask();

    void prepare();
    Task::ReportResult report();

    QList<Task*> onSubTaskFinished(Task* subTask);

    MultipleSequenceAlignment                  resultMA;

private slots:
    void sl_progressUndefined();

private:
    MultipleSequenceAlignment                  inputMsa;
    GObjectReference            objRef;
    QPointer<Document>          tmpDoc;
    QString                     url;
    MAFFTLogParser*             logParser;

    SaveMSA2SequencesTask*      saveTemporaryDocumentTask;
    ExternalToolRunTask*        mAFFTTask;
    LoadDocumentTask*           loadTmpDocumentTask;
    MAFFTSupportTaskSettings    settings;
    QPointer<StateLock>         lock;
};

class MultipleSequenceAlignmentObject;

class MAFFTWithExtFileSpecifySupportTask : public Task {
    Q_OBJECT
    Q_DISABLE_COPY(MAFFTWithExtFileSpecifySupportTask)
public:
    MAFFTWithExtFileSpecifySupportTask(const MAFFTSupportTaskSettings& settings);
    ~MAFFTWithExtFileSpecifySupportTask();
    void prepare();
    Task::ReportResult report();

    QList<Task*> onSubTaskFinished(Task* subTask);
private:
    MultipleSequenceAlignmentObject*           mAObject;
    Document*                   currentDocument;
    bool                        cleanDoc;

    SaveDocumentTask*           saveDocumentTask;
    LoadDocumentTask*           loadDocumentTask;
    MAFFTSupportTask*           mAFFTSupportTask;
    MAFFTSupportTaskSettings    settings;
};

class MAFFTLogParser : public ExternalToolLogParser {
    Q_OBJECT
    Q_DISABLE_COPY(MAFFTLogParser)
public:
    MAFFTLogParser(int countSequencesInMSA, int countRefinementIter, const QString& outputFileName);
    ~MAFFTLogParser(){ cleanup(); }
    int getProgress();
    void parseOutput(const QString& partOfLog);
    void parseErrOutput(const QString& partOfLog);

    bool isOutFileCreated(){ return isOutputFileCreated; }
    void cleanup();

signals:
    void si_progressUndefined();

private:
    int     countSequencesInMSA;
    int     countRefinementIter;
    QString outputFileName;
    QFile   outFile;
    bool    isOutputFileCreated;
    QString lastErrLine;

    bool    isMemSaveModeEnabled;   // there is no progress in the memsave mode
    bool    firstDistanceMatrix;
    bool    secondDistanceMatrix;
    bool    firstUPGMATree;
    bool    secondUPGMATree;
    bool    firstProAlign;
    bool    secondProAlign;
    int     progress;

    static const QString MEM_SAVE_MODE_MESSAGE;
};

}//namespace
#endif // _U2_MAFFT_SUPPORT_TASK_H
