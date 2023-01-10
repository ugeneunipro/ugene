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

#include "MuscleTask.h"

#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/TaskWatchdog.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/SimpleWorkflowTask.h>

#include "MuscleAdapter.h"
#include "MuscleParallel.h"
#include "TaskLocalStorage.h"
#include "muscle/muscle.h"
#include "muscle/muscle_context.h"

#define MUSCLE_LOCK_REASON "Muscle lock"

namespace U2 {

void MuscleTaskSettings::reset() {
    nThreads = 0;
    op = MuscleTaskOp_Align;
    maxIterations = 8;
    maxSecs = 0;
    stableMode = true;
    regionToAlign.startPos = regionToAlign.length = 0;
    profile = MultipleSequenceAlignment();
    alignRegion = false;
    inputFilePath = "";
    mode = Default;
    rowIndexesToAlign.clear();
}

MuscleTask::MuscleTask(const MultipleSequenceAlignment& ma, const MuscleTaskSettings& _config)
    : Task(tr("MUSCLE alignment"), TaskFlags_FOSCOE | TaskFlag_MinimizeSubtaskErrorText),
      config(_config),
      inputMA(ma->getExplicitCopy()) {
    GCOUNTER(cvar, "MuscleTask");
    config.nThreads = (config.nThreads == 0 ? AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount() : config.nThreads);
    SAFE_POINT_EXT(config.nThreads > 0,
                   setError("Incorrect number of max parallel subtasks"), );
    setMaxParallelSubtasks(config.nThreads);

    algoLog.info(tr("MUSCLE alignment started"));

    ctx = new MuscleContext(config.nThreads);
    ctx->params.g_bStable = config.stableMode;
    ctx->params.g_uMaxIters = config.maxIterations;
    ctx->params.g_ulMaxSecs = config.maxSecs;
    parallelSubTask = nullptr;

    // todo: make more precise estimation, use config.op mode
    int aliLen = ma->getLength();
    int nSeq = ma->getRowCount();
    int memUseMB = qint64(aliLen) * qint64(nSeq) * 200 / (1024 * 1024);  // 200x per char in alignment
    TaskResourceUsage tru(RESOURCE_MEMORY, memUseMB);

    QString inputAlName = inputMA->getName();
    resultMA->setName(inputAlName);
    resultSubMA->setName(inputAlName);

    inputSubMA = inputMA->getExplicitCopy();
    if (config.alignRegion && config.regionToAlign.length != inputMA->getLength()) {
        SAFE_POINT_EXT(config.regionToAlign.length > 0,
                       setError(tr("Incorrect region to align")), );
        inputSubMA = inputMA->mid(config.regionToAlign.startPos, config.regionToAlign.length);
        CHECK_EXT(inputSubMA != MultipleSequenceAlignment(), setError(tr("Stopping MUSCLE task, because of error in MultipleSequenceAlignment::mid function")), );
    }

    if (config.nThreads == 1 || (config.op != MuscleTaskOp_Align)) {
        tpm = Task::Progress_Manual;
    } else {
        setUseDescriptionFromSubtask(true);
        parallelSubTask = new MuscleParallelTask(inputSubMA, resultSubMA, config, ctx);
        addSubTask(parallelSubTask);
        tru.prepareStageLock = true;
    }
    addTaskResource(tru);
}

void MuscleTask::run() {
    TaskLocalData::bindToMuscleTLSContext(ctx);

    CHECK(!hasError(), );

    algoLog.details(tr("Performing MUSCLE alignment..."));

    switch (config.op) {
        case MuscleTaskOp_Align:
            doAlign(false);
            break;
        case MuscleTaskOp_Refine:
            doAlign(true);
            break;
        case MuscleTaskOp_AddUnalignedToProfile:
            doAddUnalignedToProfile();
            break;
        case MuscleTaskOp_ProfileToProfile:
            doProfile2Profile();
            break;
        case MuscleTaskOp_OwnRowsToAlignment:
            alignOwnRowsToAlignment(stateInfo);
            break;
    }
    if (!hasError() && !isCanceled()) {
        SAFE_POINT_EXT(resultMA->getAlphabet() != nullptr,
                       stateInfo.setError("The alphabet of result alignment is null"), );
    }
    TaskLocalData::detachMuscleTLSContext();

    if (!stateInfo.isCoR()) {
        algoLog.info(tr("MUSCLE alignment successfully finished"));
    }
}

void MuscleTask::doAlign(bool refine) {
    if (parallelSubTask == nullptr) {  // align in this thread
        SAFE_POINT_EXT(resultSubMA->isEmpty(), stateInfo.setError("Incorrect result state"), );
        if (refine) {
            MuscleAdapter::refine(inputSubMA, resultSubMA, stateInfo);
        } else {
            MuscleAdapter::align(inputSubMA, resultSubMA, stateInfo, true);
        }
    }
    CHECK(!hasError(), );
    U2OpStatus2Log os;

    if (!isCanceled()) {
        SAFE_POINT_EXT(!resultSubMA->isEmpty(),
                       stateInfo.setError("The result multiple alignment is empty!"), );

        resultMA->setAlphabet(inputMA->getAlphabet());
        QByteArray emptySeq;
        const int nSeq = inputMA->getRowCount();

        const int resNSeq = resultSubMA->getRowCount();
        const int maxSeq = qMax(nSeq, resNSeq);
        QVector<unsigned int> ids(maxSeq, 0);
        QVector<bool> existID(maxSeq, false);
        for (int i = 0; i < resNSeq; i++) {
            ids[i] = ctx->output_uIds.length() > i ? ctx->output_uIds[i] : i;
            existID[ids[i]] = true;
        }
        if (config.stableMode) {
            for (int i = 0; i < nSeq; i++) {
                ids[i] = i;
            }
        }
        int j = resNSeq;
        QByteArray gapSeq(resultSubMA->getLength(), U2Msa::GAP_CHAR);
        for (int i = 0, n = nSeq; i < n; i++) {
            if (!existID[i]) {
                QString rowName = inputMA->getMsaRow(i)->getName();
                if (config.stableMode) {
                    resultSubMA->addRow(rowName, gapSeq, i);
                } else {
                    ids[j] = i;
                    resultSubMA->addRow(rowName, gapSeq);
                }
                j++;
            }
        }

        SAFE_POINT_EXT(resultSubMA->getRowCount() == inputMA->getRowCount(),
                       stateInfo.setError(tr("Unexpected number of rows in the result multiple alignment!")), );

        if (config.alignRegion && config.regionToAlign.length != inputMA->getLength()) {
            for (int i = 0, n = inputMA->getRowCount(); i < n; i++) {
                const MultipleSequenceAlignmentRow row = inputMA->getMsaRow(ids[i]);
                resultMA->addRow(row->getName(), emptySeq);
            }
            if (config.regionToAlign.startPos != 0) {
                for (int i = 0; i < nSeq; i++) {
                    int regionLen = config.regionToAlign.startPos;
                    const MultipleSequenceAlignmentRow inputRow = inputMA->getMsaRow(ids[i])->mid(0, regionLen, os);
                    resultMA->appendChars(i, 0, inputRow->toByteArray(os, regionLen).constData(), regionLen);
                }
            }
            *resultMA += *resultSubMA;
            int resultLen = resultMA->getLength();
            if (config.regionToAlign.endPos() != inputMA->getLength()) {
                int subStart = config.regionToAlign.endPos();
                int subLen = inputMA->getLength() - config.regionToAlign.endPos();
                for (int i = 0; i < nSeq; i++) {
                    const MultipleSequenceAlignmentRow inputRow = inputMA->getMsaRow(ids[i])->mid(subStart, subLen, os);
                    resultMA->appendChars(i, resultLen, inputRow->toByteArray(os, subLen).constData(), subLen);
                }
            }
            // TODO: check if there are GAP columns on borders and remove them
        } else {
            resultMA = resultSubMA;
        }
    }
}

void MuscleTask::doAddUnalignedToProfile() {
    MuscleAdapter::addUnalignedSequencesToProfile(inputMA, config.profile, resultMA, stateInfo);
}

void MuscleTask::doProfile2Profile() {
    MuscleAdapter::align2Profiles(inputMA, config.profile, resultMA, stateInfo);
}

void MuscleTask::alignOwnRowsToAlignment(U2OpStatus& os) {
    // Move own sequences from inputMA to profile.
    MultipleSequenceAlignment inputTrimmedInputMa("inputTrimmedInputMa", inputMA->getAlphabet());
    MultipleSequenceAlignment inputUnalignedMa("inputUnalignedMa", inputMA->getAlphabet());
    for (int rowIndex = 0; rowIndex < inputMA->getRowCount(); rowIndex++) {
        MultipleAlignmentRow row = inputMA->getRow(rowIndex);
        if (config.rowIndexesToAlign.contains(rowIndex)) {
            DNASequence sequence = row->getUngappedSequence();
            inputUnalignedMa->addRow(row->getName(), sequence.seq);
        } else {
            QByteArray sequence = row->getSequenceWithGaps(true, true);
            inputTrimmedInputMa->addRow(row->getName(), sequence);
        }
    }
    SAFE_POINT_EXT(!inputTrimmedInputMa->isEmpty() && !inputUnalignedMa->isEmpty(), os.setError("Invalid rowIdsToAlign row set"), );

    MultipleSequenceAlignment resultUnorderedMa;  // Result alignment with incorrect (non-inputMA) rows order.
    MuscleAdapter::addUnalignedSequencesToProfile(inputTrimmedInputMa, inputUnalignedMa, resultUnorderedMa, stateInfo);

    if (resultUnorderedMa->getRowCount() != inputMA->getRowCount()) {
        os.setError(tr("Failed to align rows with MUSCLE. Result number of rows does not match"));
        return;
    }

    // Add all rows from 'resultUnorderedMa' to 'resultMA' using the original order (inputMA).
    SAFE_POINT_EXT(resultMA->isEmpty(), os.setError(L10N::internalError("resultMA must be empty")), );
    resultMA->setAlphabet(inputMA->getAlphabet());
    int trimmedInputMsaRowCount = inputTrimmedInputMa->getRowCount();
    int trimmedMsaRowIndex = 0;
    int unalignedMsaIndex = 0;
    QList<qint64> inputMaRowIds = inputMA->getRowsIds();
    for (int i = 0; i < inputMA->getRowCount(); i++) {
        int resultUnorderedMsaRowIndex;
        if (config.rowIndexesToAlign.contains(i)) {
            resultUnorderedMsaRowIndex = trimmedInputMsaRowCount + unalignedMsaIndex;
            unalignedMsaIndex++;
        } else {
            resultUnorderedMsaRowIndex = trimmedMsaRowIndex;
            trimmedMsaRowIndex++;
        }
        SAFE_POINT_EXT(trimmedMsaRowIndex <= trimmedInputMsaRowCount, os.setError(L10N::internalError("Invalid trimmedMsaRowIndex")), );
        SAFE_POINT_EXT(resultUnorderedMsaRowIndex < inputMA->getRowCount(), os.setError(L10N::internalError("Invalid resultUnorderedMsaRowIndex")), );
        MultipleAlignmentRow row = resultUnorderedMa->getRow(resultUnorderedMsaRowIndex);
        resultMA->addRow(row->getName(), row->getUngappedSequence(), row->getGaps(), stateInfo);
        CHECK_OP(stateInfo, );

        // Restore original row ids.
        int rowIndex = resultMA->getRowCount() - 1;
        resultMA->setRowId(rowIndex, inputMaRowIds[rowIndex]);
    }
}

Task::ReportResult MuscleTask::report() {
    delete ctx;
    ctx = nullptr;
    return ReportResult_Finished;
}

//////////////////////////////////////////////////////////////////////////
// MuscleAddSequencesToProfileTask

MuscleAddSequencesToProfileTask::MuscleAddSequencesToProfileTask(MultipleSequenceAlignmentObject* _obj,
                                                                 const QString& fileWithSequencesOrProfile,
                                                                 const MMode& _mode)
    : Task("", TaskFlags_NR_FOSCOE), maObj(_obj), mode(_mode) {
    setUseDescriptionFromSubtask(true);
    setVerboseLogMode(true);

    QString aliName = maObj->getDocument()->getName();
    QString fileName = QFileInfo(fileWithSequencesOrProfile).fileName();
    QString tn;
    if (mode == Profile2Profile) {
        tn = tr("MUSCLE align profiles '%1' vs '%2'").arg(aliName).arg(fileName);
    } else {
        tn = tr("MUSCLE align '%2' by profile '%1'").arg(aliName).arg(fileName);
    }
    setTaskName(tn);

    QList<FormatDetectionResult> detectedFormats = DocumentUtils::detectFormat(fileWithSequencesOrProfile);
    if (detectedFormats.isEmpty()) {
        setError("Unknown format");
        return;
    }
    TaskWatchdog::trackResourceExistence(maObj, this, tr("A problem occurred during aligning profile to profile with MUSCLE. The original alignment is no more available."));
    DocumentFormat* format = detectedFormats.first().format;
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(fileWithSequencesOrProfile));
    loadTask = new LoadDocumentTask(format->getFormatId(), fileWithSequencesOrProfile, iof);
    loadTask->setSubtaskProgressWeight(0.01f);
    addSubTask(loadTask);
}

QList<Task*> MuscleAddSequencesToProfileTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;

    if (subTask != loadTask || isCanceled() || hasError()) {
        return res;
    }

    propagateSubtaskError();
    if (hasError()) {
        return res;
    }

    MuscleTaskSettings s;
    s.op = mode == Sequences2Profile ? MuscleTaskOp_AddUnalignedToProfile : MuscleTaskOp_ProfileToProfile;

    QList<GObject*> seqObjects = loadTask->getDocument()->findGObjectByType(GObjectTypes::SEQUENCE);
    // todo: move to utility alphabet reduction
    const DNAAlphabet* al = nullptr;
    foreach (GObject* obj, seqObjects) {
        U2SequenceObject* dnaObj = qobject_cast<U2SequenceObject*>(obj);
        const DNAAlphabet* objAl = dnaObj->getAlphabet();
        if (al == nullptr) {
            al = objAl;
        } else if (al != objAl) {
            al = U2AlphabetUtils::deriveCommonAlphabet(al, objAl);
            CHECK_EXT(al != nullptr, setError(tr("Sequences in file have different alphabets %1").arg(loadTask->getDocument()->getURLString())), res);
        }
        QByteArray seqData = dnaObj->getWholeSequenceData(stateInfo);
        CHECK_OP(stateInfo, res);
        s.profile->addRow(dnaObj->getSequenceName(), seqData);
    }
    if (!seqObjects.isEmpty()) {
        s.profile->setAlphabet(al);
    }

    if (seqObjects.isEmpty()) {
        QList<GObject*> maObjects = loadTask->getDocument()->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
        if (!maObjects.isEmpty()) {
            auto firstMaObj = qobject_cast<MultipleSequenceAlignmentObject*>(maObjects.first());
            s.profile = firstMaObj->getMsaCopy();
        }
    }

    if (s.profile->isEmpty()) {
        if (mode == Sequences2Profile) {
            stateInfo.setError(tr("No sequences found in file %1").arg(loadTask->getDocument()->getURLString()));
        } else {
            stateInfo.setError(tr("No alignment found in file %1").arg(loadTask->getDocument()->getURLString()));
        }
        return res;
    }
    res.append(new MuscleGObjectTask(maObj, s));
    return res;
}

Task::ReportResult MuscleAddSequencesToProfileTask::report() {
    if (!hasError()) {
        propagateSubtaskError();
    }
    return ReportResult_Finished;
}

//////////////////////////////////////////////////////////////////////////
// MuscleGObjectTask

MuscleGObjectTask::MuscleGObjectTask(MultipleSequenceAlignmentObject* _obj, const MuscleTaskSettings& _config)
    : AlignGObjectTask("", TaskFlags_NR_FOSCOE, _obj), lock(nullptr), muscleTask(nullptr), config(_config) {
    QString aliName;
    if (obj->getDocument() == nullptr) {
        aliName = MA_OBJECT_NAME;
    } else {
        aliName = obj->getDocument()->getName();
    }
    QString tn;
    switch (config.op) {
        case MuscleTaskOp_Align:
            tn = tr("MUSCLE align '%1'").arg(aliName);
            break;
        case MuscleTaskOp_Refine:
            tn = tr("MUSCLE refine '%1'").arg(aliName);
            break;
        case MuscleTaskOp_AddUnalignedToProfile:
            tn = tr("MUSCLE add to profile '%1'").arg(aliName);
            break;
        case MuscleTaskOp_ProfileToProfile:
            tn = tr("MUSCLE align profiles");
            break;
        case MuscleTaskOp_OwnRowsToAlignment:
            tn = tr("MUSCLE align rows to alignment: %1").arg(aliName);
            break;
        default:
            assert(0);
    }
    setTaskName(tn);
    setUseDescriptionFromSubtask(true);
    setVerboseLogMode(true);
}

MuscleGObjectTask::~MuscleGObjectTask() {
    // Unlock the alignment object if the task has been failed
    if (!lock.isNull()) {
        if (!obj.isNull()) {
            if (obj->isStateLocked()) {
                obj->unlockState(lock);
            }
            delete lock;
            lock = nullptr;
        }
    }
}

void MuscleGObjectTask::prepare() {
    if (obj.isNull()) {
        stateInfo.setError("object_removed");
        return;
    }
    if (obj->isStateLocked()) {
        stateInfo.setError("object_is_state_locked");
        return;
    }

    lock = new StateLock(MUSCLE_LOCK_REASON);
    obj->lockState(lock);
    muscleTask = new MuscleTask(obj->getMultipleAlignment(), config);

    addSubTask(muscleTask);
}

Task::ReportResult MuscleGObjectTask::report() {
    if (!lock.isNull()) {
        obj->unlockState(lock);
        delete lock;
        lock = nullptr;
    } else {
        if (!stateInfo.isCoR()) {
            stateInfo.setError(tr("MultipleSequenceAlignment object has been changed"));
        }
        return ReportResult_Finished;
    }
    propagateSubtaskError();
    if (hasError() || isCanceled()) {
        return ReportResult_Finished;
    }

    DbiOperationsBlock op(obj->getEntityRef().dbiRef, stateInfo);

    SAFE_POINT_EXT(!obj.isNull(), stateInfo.setError("Failed to apply the result of Muscle: alignment object is not available!"), ReportResult_Finished);
    if (obj->isStateLocked()) {
        stateInfo.setError(L10N::errorObjectIsReadOnly(muscleTask->inputMA->getName()));
        return ReportResult_Finished;
    }
    if (config.op == MuscleTaskOp_Align || config.op == MuscleTaskOp_Refine) {
        MSAUtils::assignOriginalDataIds(muscleTask->inputMA, muscleTask->resultMA, stateInfo);
        CHECK_OP(stateInfo, ReportResult_Finished);

        QMap<qint64, QVector<U2MsaGap>> rowsGapModel;
        for (int i = 0, n = muscleTask->resultMA->getRowCount(); i < n; ++i) {
            qint64 rowId = muscleTask->resultMA->getMsaRow(i)->getRowDbInfo().rowId;
            const QVector<U2MsaGap>& newGapModel = muscleTask->resultMA->getMsaRow(i)->getGaps();
            rowsGapModel.insert(rowId, newGapModel);
        }

        U2UseCommonUserModStep userModStep(obj->getEntityRef(), stateInfo);
        CHECK_OP(stateInfo, ReportResult_Finished);
        obj->updateGapModel(stateInfo, rowsGapModel);

        QList<qint64> resultRowIds = muscleTask->resultMA->getRowsIds();
        if (resultRowIds != muscleTask->inputMA->getRowsIds()) {
            obj->updateRowsOrder(stateInfo, resultRowIds);
        }
    } else if (config.op == MuscleTaskOp_AddUnalignedToProfile || config.op == MuscleTaskOp_ProfileToProfile || config.op == MuscleTaskOp_OwnRowsToAlignment) {
        SAFE_POINT_EXT(muscleTask->inputMA->getRowCount() + config.profile->getRowCount() == muscleTask->resultMA->getRowCount(),
                       stateInfo.setError(tr("Illegal number of sequences in the MUSCLE alignment result")),
                       ReportResult_Finished);

        if (*obj->getMultipleAlignment().data() != *muscleTask->resultMA.data()) {
            U2UseCommonUserModStep userModStep(obj->getEntityRef(), stateInfo);
            CHECK_OP(stateInfo, ReportResult_Finished);
            obj->setMultipleAlignment(muscleTask->resultMA);
        }
    } else {
        FAIL("Unsupported Muscle Align task mode: " + QString::number(config.op), ReportResult_Finished);
    }

    return ReportResult_Finished;
}

////////////////////////////////////////
// MuscleWithExtFileSpecifySupportTask
MuscleWithExtFileSpecifySupportTask::MuscleWithExtFileSpecifySupportTask(const MuscleTaskSettings& _config)
    : Task(tr("Run Muscle alignment task"), TaskFlags_NR_FOSCOE),
      config(_config) {
    mAObject = nullptr;
    currentDocument = nullptr;
    saveDocumentTask = nullptr;
    loadDocumentTask = nullptr;
    muscleGObjectTask = nullptr;
    cleanDoc = true;
}

MuscleWithExtFileSpecifySupportTask::~MuscleWithExtFileSpecifySupportTask() {
    if (cleanDoc) {
        delete currentDocument;
    }
}

void MuscleWithExtFileSpecifySupportTask::prepare() {
    DocumentFormatConstraints c;
    c.checkRawData = true;
    c.supportedObjectTypes += GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    c.rawData = IOAdapterUtils::readFileHeader(config.inputFilePath);
    c.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);
    QList<DocumentFormatId> formats = AppContext::getDocumentFormatRegistry()->selectFormats(c);
    if (formats.isEmpty()) {
        stateInfo.setError("input_format_error");
        return;
    }

    DocumentFormatId alnFormat = formats.first();
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(config.inputFilePath));
    QVariantMap hints;
    if (alnFormat == BaseDocumentFormats::FASTA) {
        hints[DocumentReadingMode_SequenceAsAlignmentHint] = true;
    }
    loadDocumentTask = new LoadDocumentTask(alnFormat, config.inputFilePath, iof, hints);
    addSubTask(loadDocumentTask);
}

QList<Task*> MuscleWithExtFileSpecifySupportTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return res;
    }
    if (hasError() || isCanceled()) {
        return res;
    }
    if (subTask == loadDocumentTask) {
        currentDocument = loadDocumentTask->takeDocument();
        SAFE_POINT(currentDocument != nullptr, QString("Failed loading document: %1").arg(loadDocumentTask->getURLString()), res);
        SAFE_POINT(currentDocument->getObjects().length() == 1, QString("Number of objects != 1 : %1").arg(loadDocumentTask->getURLString()), res);
        mAObject = qobject_cast<MultipleSequenceAlignmentObject*>(currentDocument->getObjects().first());
        SAFE_POINT(mAObject != nullptr, QString("MA object not found!: %1").arg(loadDocumentTask->getURLString()), res);
        if (config.alignRegion) {
            if ((config.regionToAlign.startPos > mAObject->getLength()) || ((config.regionToAlign.startPos + config.regionToAlign.length) > mAObject->getLength())) {
                config.alignRegion = false;
                config.regionToAlign = U2Region(0, mAObject->getLength());
            }
        } else {
            config.regionToAlign = U2Region(0, mAObject->getLength());
        }

        muscleGObjectTask = new MuscleGObjectRunFromSchemaTask(mAObject, config);
        res.append(muscleGObjectTask);
    } else if (subTask == muscleGObjectTask) {
        saveDocumentTask = new SaveDocumentTask(currentDocument, AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(config.outputFilePath)), config.outputFilePath);
        res.append(saveDocumentTask);
    } else if (subTask == saveDocumentTask) {
        Task* openTask = AppContext::getProjectLoader()->openWithProjectTask(config.outputFilePath);
        if (openTask != nullptr) {
            res << openTask;
        }
    }
    return res;
}

Task::ReportResult MuscleWithExtFileSpecifySupportTask::report() {
    return ReportResult_Finished;
}

//////////////////////////////////
// MuscleGObjectRunFromSchemaTask
MuscleGObjectRunFromSchemaTask::MuscleGObjectRunFromSchemaTask(MultipleSequenceAlignmentObject* obj, const MuscleTaskSettings& c)
    : AlignGObjectTask("", TaskFlags_NR_FOSCOE, obj), config(c) {
    setMAObject(obj);
    SAFE_POINT_EXT(config.profile->isEmpty(), setError("Invalid config profile detected"), );

    setUseDescriptionFromSubtask(true);
    setVerboseLogMode(true);
}

void MuscleGObjectRunFromSchemaTask::prepare() {
    SimpleMSAWorkflowTaskConfig conf;
    conf.schemaName = "align";
    conf.schemaArgs << QString("--mode=%1").arg(config.mode);
    conf.schemaArgs << QString("--max-iterations=%1").arg(config.maxIterations);
    conf.schemaArgs << QString("--stable=%1").arg(config.stableMode);
    if (config.alignRegion) {
        conf.schemaArgs << QString("--range=%1").arg(QString("%1..%2").arg(config.regionToAlign.startPos + 1).arg(config.regionToAlign.endPos()));
    }

    addSubTask(new SimpleMSAWorkflow4GObjectTask(tr("Workflow wrapper '%1'").arg(getTaskName()), obj, conf));
}

void MuscleGObjectRunFromSchemaTask::setMAObject(MultipleSequenceAlignmentObject* maobj) {
    SAFE_POINT_EXT(maobj != nullptr, setError("Invalid MSA object detected"), );
    Document* maDoc = maobj->getDocument();
    SAFE_POINT_EXT(maDoc != nullptr, setError("Invalid MSA document detected"), );
    const QString objName = maDoc->getName();
    SAFE_POINT_EXT(!objName.isEmpty(), setError("Invalid MSA object name detected"), );
    AlignGObjectTask::setMAObject(maobj);
    QString tName;
    switch (config.op) {
        case MuscleTaskOp_Align:
            tName = tr("MUSCLE align '%1'").arg(objName);
            break;
        case MuscleTaskOp_Refine:
            tName = tr("MUSCLE refine '%1'").arg(objName);
            break;
        default:
            SAFE_POINT_EXT(false, setError("Invalid config detected"), );
    }
    setTaskName(tName);
}

MuscleAlignOwnSequencesToSelfAction::MuscleAlignOwnSequencesToSelfAction(MultipleSequenceAlignmentObject* msaObject, const QList<int>& maRowIndexes)
    : Task(tr("MUSCLE align rows to alignment '%1'").arg(msaObject->getGObjectName()), TaskFlags_NR_FOSCOE) {
    MuscleTaskSettings settings;
    settings.op = MuscleTaskOp_OwnRowsToAlignment;
    settings.rowIndexesToAlign = maRowIndexes.toSet();
    addSubTask(new MuscleGObjectTask(msaObject, settings));
}

}  // namespace U2
