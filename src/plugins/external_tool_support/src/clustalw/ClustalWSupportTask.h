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

#ifndef _U2_CLUSTALW_SUPPORT_TASK_H
#define _U2_CLUSTALW_SUPPORT_TASK_H

#include "utils/ExportTasks.h"

#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/Task.h>


namespace U2 {

/*Options for clustalW
    ***Multiple Alignments:***
? -NEWTREE=      :file for new guide tree
? -USETREE=      :file for old guide tree
+ -MATRIX=       :Protein weight matrix=BLOSUM, PAM, GONNET, ID or filename
+ -DNAMATRIX=    :DNA weight matrix=IUB, CLUSTALW or filename
+ -GAPOPEN=f     :gap opening penalty -- 100.0?
+ -GAPEXT=f      :gap extension penalty -- 10.0?
+ -ENDGAPS       :no end gap separation pen.
+ -GAPDIST=n     :gap separation pen. range
+ -NOPGAP        :residue-specific gaps off
+ -NOHGAP        :hydrophilic gaps off
? -HGAPRESIDUES= :list hydrophilic res.
? -MAXDIV=n      :% ident. for delay
? -TYPE=         :PROTEIN or DNA
- -TRANSWEIGHT=f :transitions weighting
+ -ITERATION=    :NONE or TREE or ALIGNMENT
+ -NUMITER=n     :maximum number of iterations to perform
- -NOWEIGHTS     :disable sequence weighting

+ -OUTORDER=     :INPUT or ALIGNED
*/

class LoadDocumentTask;

class ClustalWSupportTaskSettings {
public:
    ClustalWSupportTaskSettings() {reset();}
    void reset();

    float   gapOpenPenalty;
    float   gapExtenstionPenalty;
    bool    endGaps;
    bool    noPGaps;
    bool    noHGaps;
    int     gapDist;
    QString iterationType;
    int     numIterations;
    QString inputFilePath;
    QString outputFilePath;
    QString matrix;
    bool    outOrderInput; // false - aligned, true - input
};


class ClustalWSupportTask : public ExternalToolSupportTask {
    Q_OBJECT
    Q_DISABLE_COPY(ClustalWSupportTask)
public:
    ClustalWSupportTask(const MultipleSequenceAlignment& _inputMsa, const GObjectReference& _objRef, const ClustalWSupportTaskSettings& _settings);
    ~ClustalWSupportTask();

    void prepare();
    Task::ReportResult report();

    QList<Task*> onSubTaskFinished(Task* subTask);

    MultipleSequenceAlignment                  resultMA;
private:
    MultipleSequenceAlignment                  inputMsa;
    GObjectReference            objRef;
    QPointer<Document>          tmpDoc;
    QString                     url;

    SaveAlignmentTask*          saveTemporaryDocumentTask;
    ExternalToolRunTask*        clustalWTask;
    LoadDocumentTask*           loadTemporyDocumentTask;
    ClustalWSupportTaskSettings settings;
    QPointer<StateLock>         lock;
};

class MultipleSequenceAlignmentObject;

class ClustalWWithExtFileSpecifySupportTask : public Task {
    Q_OBJECT
    Q_DISABLE_COPY(ClustalWWithExtFileSpecifySupportTask)
public:
    ClustalWWithExtFileSpecifySupportTask(const ClustalWSupportTaskSettings& settings);
    ~ClustalWWithExtFileSpecifySupportTask();
    void prepare();
    Task::ReportResult report();

    QList<Task*> onSubTaskFinished(Task* subTask);
private:
    MultipleSequenceAlignmentObject*           mAObject;
    Document*                   currentDocument;
    bool                        cleanDoc;

    SaveDocumentTask*           saveDocumentTask;
    LoadDocumentTask*           loadDocumentTask;
    ClustalWSupportTask*        clustalWSupportTask;
    ClustalWSupportTaskSettings settings;

};

class ClustalWLogParser : public ExternalToolLogParser {
public:
    ClustalWLogParser(int countSequencesInMSA);
    int getProgress();
private:
    int countSequencesInMSA;
};

}//namespace
#endif // _U2_CLUSTALW_SUPPORT_TASK_H
