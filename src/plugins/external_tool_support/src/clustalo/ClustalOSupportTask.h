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

#ifndef _U2_CLUSTALO_SUPPORT_TASK_H
#define _U2_CLUSTALO_SUPPORT_TASK_H

#include "utils/ExportTasks.h"

#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/Task.h>


namespace U2 {

/*Options for ClustalO
Sequence Input:
+  -i, --in, --infile={<file>,-} Multiple sequence input file (- for stdin)
  --hmm-in=<file>           HMM input files
  --dealign                 Dealign input sequences
  --profile1, --p1=<file>   Pre-aligned multiple sequence file (aligned columns will be kept fix)
  --profile2, --p2=<file>   Pre-aligned multiple sequence file (aligned columns will be kept fix)

Clustering:
  --distmat-in=<file>       Pairwise distance matrix input file (skips distance computation)
  --distmat-out=<file>      Pairwise distance matrix output file
  --guidetree-in=<file>     Guide tree input file (skips distance computation and guide-tree clustering step)
  --guidetree-out=<file>    Guide tree output file
  --full                    Use full distance matrix for guide-tree calculation (might be slow; mBed is default)
  --full-iter               Use full distance matrix for guide-tree calculation during iteration (might be slowish; mBed is default)

Alignment Output:
  -o, --out, --outfile={file,-} Multiple sequence alignment output file (default: stdout)
  --outfmt={a2m=fa[sta],clu[stal],msf,phy[lip],selex,st[ockholm],vie[nna]} MSA output file format (default: fasta)

Iteration:
+  --iterations, --iter=<n>  Number of (combined guide-tree/HMM) iterations
+  --max-guidetree-iterations=<n> Maximum number guidetree iterations
+  --max-hmm-iterations=<n>  Maximum number of HMM iterations

Limits (will exit early, if exceeded):
  --maxnumseq=<n>           Maximum allowed number of sequences
  --maxseqlen=<l>           Maximum allowed sequence length

Miscellaneous:
+  --auto                    Set options automatically (might overwrite some of your options)
+  --threads=<n>             Number of processors to use
  -l, --log=<file>          Log all non-essential output to this file
  -h, --help                Print this help and exit
  -v, --verbose             Verbose output (increases if given multiple times)
  --version                 Print version information and exit
  --long-version            Print long version information and exit
  --force                   Force file overwriting

*/

class LoadDocumentTask;

class ClustalOSupportTaskSettings {
public:
    ClustalOSupportTaskSettings() {reset();}
    void reset();

    int     numIterations;          // --iterations, --iter=<n>       Number of (combined guide-tree/HMM) iterations
    int     maxGuidetreeIterations; // --max-guidetree-iterations=<n> Maximum number guidetree iterations
    int     maxHMMIterations;       // --max-hmm-iterations=<n>       Maximum number of HMM iterations

    bool    setAutoOptions;         // --auto                         Set options automatically (might overwrite some of your options)
    int     numberOfProcessors;     // --threads=<n>                  Number of processors to use
    QString inputFilePath;          // -i, --in, --infile={<file>,-}  Multiple sequence input file (- for stdin)
    QString outputFilePath;
};


class ClustalOSupportTask : public ExternalToolSupportTask {
    Q_OBJECT
    Q_DISABLE_COPY(ClustalOSupportTask)
public:
    ClustalOSupportTask(const MultipleSequenceAlignment& _inputMsa, const GObjectReference& _objRef, const ClustalOSupportTaskSettings& settings);
    ~ClustalOSupportTask();

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
    ExternalToolRunTask*        clustalOTask;
    LoadDocumentTask*           loadTemporyDocumentTask;
    ClustalOSupportTaskSettings settings;
    QPointer<StateLock>         lock;
};

class ClustalOWithExtFileSpecifySupportTask : public Task {
    Q_OBJECT
    Q_DISABLE_COPY(ClustalOWithExtFileSpecifySupportTask)
public:
    ClustalOWithExtFileSpecifySupportTask(const ClustalOSupportTaskSettings& settings);
    ~ClustalOWithExtFileSpecifySupportTask();
    void prepare();
    Task::ReportResult report();

    QList<Task*> onSubTaskFinished(Task* subTask);
private:
    MultipleSequenceAlignmentObject*           mAObject;
    Document*                   currentDocument;
    bool                        cleanDoc;

    SaveDocumentTask*           saveDocumentTask;
    LoadDocumentTask*           loadDocumentTask;
    ClustalOSupportTask*        clustalOSupportTask;
    ClustalOSupportTaskSettings settings;
};

class ClustalOLogParser : public ExternalToolLogParser {
public:
    ClustalOLogParser();
    void parseOutput(const QString& partOfLog);
    int getProgress();
private:
    /* Last line printed to stdout */
    QString     lastLine;
    /* If any error occurred, this variable will be non-empty */
    QString     lastError;
};

}//namespace
#endif // _U2_CLUSTALO_SUPPORT_TASK_H
