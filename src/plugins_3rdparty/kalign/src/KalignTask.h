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

#ifndef _U2_KALIGN_TASK_H_
#define _U2_KALIGN_TASK_H_

#include <U2Algorithm/MsaUtilTasks.h>

#include <U2Core/Task.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/TLSTask.h>
#include <U2Core/U2Mod.h>

#define KALIGN_CONTEXT_ID "kalign"

struct kalign_context;

namespace U2 {

class StateLock;
class MultipleSequenceAlignmentObject;
class LoadDocumentTask;

class KalignContext : public TLSContext {
public:
    KalignContext(kalign_context* _d) : TLSContext(KALIGN_CONTEXT_ID), d(_d) { }
    kalign_context* d;
};

class KalignTaskSettings {
public:
    KalignTaskSettings() {reset();}
    void reset();
    
    float   gapOpenPenalty;
    float   gapExtenstionPenalty;
    float   termGapPenalty;
    float   secret;
    QString inputFilePath;
    QString outputFilePath;
};

class KalignTask : public TLSTask {
    Q_OBJECT
public:
    KalignTask(const MultipleSequenceAlignment& ma, const KalignTaskSettings& config);
    
    void _run();
    void doAlign();
    ReportResult report();
    
    KalignTaskSettings          config;
    MultipleSequenceAlignment                  inputMA;
    MultipleSequenceAlignment                  resultMA;
    
    MultipleSequenceAlignment                  inputSubMA;
    MultipleSequenceAlignment                  resultSubMA;
    
protected:
    TLSContext* createContextInstance();
};

//locks MultipleSequenceAlignment object and propagate KalignTask results to it
class  KalignGObjectTask : public AlignGObjectTask {
    Q_OBJECT
public:
    KalignGObjectTask(MultipleSequenceAlignmentObject* obj, const KalignTaskSettings& config);
    ~KalignGObjectTask();     

    virtual void prepare();
    ReportResult report();

    QPointer<StateLock>         lock;
    KalignTask*                 kalignTask;
    KalignTaskSettings          config;
    LoadDocumentTask*           loadDocumentTask;

};

/**
* runs kalign from cmdline schema in separate process
* using data/cmdline/align-kalign.uwl schema
* schema has following aliases:
* in - input file with alignment (will be set in WorkflowRunSchemaForTask)
* out - output file with result (will be set in WorkflowRunSchemaForTask)
* format - output file format (will be set in WorkflowRunSchemaForTask)
* bonus-score - bonus score of kalign task
* gap-ext-penalty - kalign parameter
* gap-open-penalty - kalign parameter
* gap-terminal-penalty - kalign parameter
*/
class KalignGObjectRunFromSchemaTask : public AlignGObjectTask {
    Q_OBJECT
public:
    KalignGObjectRunFromSchemaTask(MultipleSequenceAlignmentObject * obj, const KalignTaskSettings & config);

    void prepare();
    void setMAObject(MultipleSequenceAlignmentObject* maobj);
private:
    KalignTaskSettings      config;
};



class KalignWithExtFileSpecifySupportTask : public Task {
public:
    KalignWithExtFileSpecifySupportTask(const KalignTaskSettings& config);
    ~KalignWithExtFileSpecifySupportTask();

    void prepare();
    QList<Task*> onSubTaskFinished(Task* subTask);
private:
    MultipleSequenceAlignmentObject*   mAObject;
    Document*           currentDocument;
    bool                cleanDoc;
    SaveDocumentTask*   saveDocumentTask;
    Task*               kalignGObjectTask;
    KalignTaskSettings  config;
    LoadDocumentTask*   loadDocumentTask;
};


}//namespace

#endif
