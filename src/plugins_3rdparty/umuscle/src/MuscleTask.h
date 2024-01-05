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

#ifndef _U2_MUSCLE_TASK_H_
#define _U2_MUSCLE_TASK_H_

#include <QPointer>

#include <U2Algorithm/MsaUtilTasks.h>

#include <U2Core/MsaObject.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2Region.h>

class MuscleContext;

namespace U2 {

class StateLock;
class MsaObject;
class LoadDocumentTask;
class MuscleParallelTask;

enum MuscleTaskOp {
    MuscleTaskOp_Align,
    MuscleTaskOp_Refine,
    MuscleTaskOp_AddUnalignedToProfile,
    MuscleTaskOp_OwnRowsToAlignment,
    MuscleTaskOp_ProfileToProfile
};

enum MuscleMode {
    Default,
    Large,
    Refine
};

class MuscleTaskSettings {
public:
    MuscleTaskSettings() {
        reset();
    }
    void reset();

    MuscleTaskOp op;
    MuscleMode mode;  // used in running muscle in separate process

    int maxIterations;
    unsigned long maxSecs;  // 0 - unlimited
    bool stableMode;

    // used only for MuscleTaskOp_DoAlign
    bool alignRegion;
    U2Region regionToAlign;

    /** Row indexes to align. Effective only when 'mode' is 'MuscleTaskOp_OwnRowsToAlignment'. */
    QSet<int> rowIndexesToAlign;

    // used only for MuscleTaskOp_AddUnalignedToProfile and MuscleTaskOp_ProfileToProfile
    Msa profile;

    // number of threads: 0 - auto, 1 - serial
    int nThreads;
    QString inputFilePath;
    QString outputFilePath;
};

class MuscleTask : public Task {
    Q_OBJECT
public:
    MuscleTask(const Msa& ma, const MuscleTaskSettings& config);

    void run() override;

    void doAlign(bool refineOnlyMode);
    void doAddUnalignedToProfile();
    void doProfile2Profile();

    /** Aligns some rows from the alignment (settings.ownRowIds) to the rest of the alignment. */
    void alignOwnRowsToAlignment(U2OpStatus& os);

    ReportResult report() override;

    MuscleTaskSettings config;
    Msa inputMA;
    Msa resultMA;

    Msa inputSubMA;
    Msa resultSubMA;

    MuscleContext* ctx;
    MuscleParallelTask* parallelSubTask;
};

class MuscleAddSequencesToProfileTask : public Task {
    Q_OBJECT
public:
    enum MMode {
        Profile2Profile,
        Sequences2Profile,
    };
    MuscleAddSequencesToProfileTask(MsaObject* obj, const QString& fileWithSequencesOrProfile, const MMode& mode);

    QList<Task*> onSubTaskFinished(Task* subTask) override;

    ReportResult report() override;

    QPointer<MsaObject> maObj;
    LoadDocumentTask* loadTask;
    MMode mode;
};

/** Re-aligns set of own sequences to the same MSA object. */
class MuscleAlignOwnSequencesToSelfAction : public Task {
    Q_OBJECT
public:
    MuscleAlignOwnSequencesToSelfAction(MsaObject* msaObject, const QList<int>& maRowIndexes);
};

// locks MultipleAlignment object and propagate MuscleTask results to it
class MuscleGObjectTask : public AlignGObjectTask {
    Q_OBJECT
public:
    MuscleGObjectTask(MsaObject* obj, const MuscleTaskSettings& config);
    ~MuscleGObjectTask();

    virtual void prepare();
    ReportResult report();

    QPointer<StateLock> lock;
    MuscleTask* muscleTask;
    MuscleTaskSettings config;
};

class MuscleWithExtFileSpecifySupportTask : public Task {
    Q_OBJECT
public:
    MuscleWithExtFileSpecifySupportTask(const MuscleTaskSettings& config);
    ~MuscleWithExtFileSpecifySupportTask();

    void prepare();
    Task::ReportResult report();

    QList<Task*> onSubTaskFinished(Task* subTask);

private:
    MsaObject* mAObject;
    Document* currentDocument;
    bool cleanDoc;

    SaveDocumentTask* saveDocumentTask;
    LoadDocumentTask* loadDocumentTask;
    Task* muscleGObjectTask;
    MuscleTaskSettings config;
};

/**
 * runs muscle from cmdline schema in separate process
 * using data/cmdline/align.uwl schema
 * schema has following aliases:
 * in - input file with alignment (will be set in WorkflowRunSchemaForTask)
 * out - output file with result (will be set in WorkflowRunSchemaForTask)
 * format - output file format (will be set in WorkflowRunSchemaForTask)
 * mode - muscle mode parameter
 * stable - muscle stable parameter
 * max-iterations - muscle iterations number parameter
 * range - muscle align region parameter
 */
class MuscleGObjectRunFromSchemaTask : public AlignGObjectTask {
    Q_OBJECT
public:
    MuscleGObjectRunFromSchemaTask(MsaObject* obj, const MuscleTaskSettings& config);

    void prepare();
    void setMAObject(MsaObject* maobj);

private:
    MuscleTaskSettings config;
};

}  // namespace U2
#endif
