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

#include "ClustalOSupportTask.h"

#include <QCoreApplication>
#include <QDir>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2Dbi.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include "ClustalOSupport.h"

namespace U2 {

ClustalOSupportTaskSettings::ClustalOSupportTaskSettings() {
}

/** Name prefix for the sequences in the temporary MSA object created by the task. */
static constexpr const char* const INDEX_NAME_PREFIX = "EvaUX7cAm";

ClustalOSupportTask::ClustalOSupportTask(const MultipleSequenceAlignment& _inputMsa, const GObjectReference& _objRef, const ClustalOSupportTaskSettings& _settings)
    : ExternalToolSupportTask(tr("ClustalO alignment task"), TaskFlags_NR_FOSCOE),
      inputMsa(_inputMsa->getExplicitCopy()),
      objRef(_objRef),
      settings(_settings),
      lock(nullptr) {
    GCOUNTER(cvar, "ExternalTool_ClustalOSupportTask");
    tmpDoc = nullptr;
    resultMsa->setName(inputMsa->getName());
    resultMsa->setAlphabet(inputMsa->getAlphabet());
}

ClustalOSupportTask::ClustalOSupportTask(const MultipleSequenceAlignment& _inputMsa,
                                         const GObjectReference& _objRef,
                                         const QString& _secondAlignmentFileUrl,
                                         const ClustalOSupportTaskSettings& _settings)
    : ExternalToolSupportTask(tr("ClustalO add sequences to alignment task"), TaskFlags_NR_FOSCOE), inputMsa(_inputMsa->getExplicitCopy()),
      objRef(_objRef),
      settings(_settings),
      lock(nullptr),
      secondAlignmentFileUrl(_secondAlignmentFileUrl) {
    GCOUNTER(cvar, "ExternalTool_ClustalOSupportTask");
}

ClustalOSupportTask::~ClustalOSupportTask() {
    if (tmpDoc != nullptr) {
        delete tmpDoc;
    }
}

const MultipleSequenceAlignment& ClustalOSupportTask::getResultAlignment() const {
    return resultMsa;
}

void ClustalOSupportTask::prepare() {
    algoLog.info(tr("ClustalO alignment started"));

    if (objRef.isValid()) {
        GObject* obj = GObjectUtils::selectObjectByReference(objRef, UOF_LoadedOnly);
        if (nullptr != obj) {
            auto alObj = dynamic_cast<MultipleSequenceAlignmentObject*>(obj);
            SAFE_POINT(nullptr != alObj, "Failed to convert GObject to MultipleSequenceAlignmentObject during applying ClustalW results!", );
            lock = new StateLock("ClustalO");
            alObj->lockState(lock);
        }
    }

    // Add new subdir for temporary files
    // Folder name is ExternalToolName + CurrentDate + CurrentTime
    QString tmpDirName = "ClustalO_" + QString::number(this->getTaskId()) + "_" +
                         QDate::currentDate().toString("dd.MM.yyyy") + "_" +
                         QTime::currentTime().toString("hh.mm.ss.zzz") + "_" +
                         QString::number(QCoreApplication::applicationPid());
    QString clustalOTmpDir = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath(ClustalOSupport::CLUSTALO_TMP_DIR);
    QString taskTmpDir = clustalOTmpDir + "/" + tmpDirName + "/";
    inputMsaTmpFileUrl = taskTmpDir + "tmp.aln";
    ioLog.details(tr("Saving data to temporary file '%1'").arg(inputMsaTmpFileUrl));

    // Check and remove subdir for temporary files
    QDir tmpDir(taskTmpDir);
    if (tmpDir.exists()) {
        foreach (const QString& file, tmpDir.entryList()) {
            tmpDir.remove(file);
        }
        if (!tmpDir.rmdir(tmpDir.absolutePath())) {
            stateInfo.setError(tr("Folder for temporary files exists. Can not remove the folder: %1.").arg(tmpDir.absolutePath()));
            return;
        }
    }
    if (!tmpDir.mkpath(taskTmpDir)) {
        stateInfo.setError(tr("Can not create folder for temporary files."));
        return;
    }

    MultipleSequenceAlignment copiedIndexedMsa = MSAUtils::createCopyWithIndexedRowNames(inputMsa, INDEX_NAME_PREFIX);
    saveTemporaryDocumentTask = new SaveAlignmentTask(copiedIndexedMsa, inputMsaTmpFileUrl, BaseDocumentFormats::CLUSTAL_ALN);
    saveTemporaryDocumentTask->setSubtaskProgressWeight(5);
    addSubTask(saveTemporaryDocumentTask);
}

QList<Task*> ClustalOSupportTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return res;
    }
    if (hasError() || isCanceled()) {
        return res;
    }
    QString outputUrl = inputMsaTmpFileUrl + ".out.aln";
    if (subTask == saveTemporaryDocumentTask) {
        QStringList arguments;
        arguments << "-v";
        bool isTwoAlignmentsMode = !secondAlignmentFileUrl.isEmpty();
        if (isTwoAlignmentsMode) {
            arguments << "--profile1" << inputMsaTmpFileUrl;
            arguments << "--profile2" << secondAlignmentFileUrl;
        } else {
            // Default alignment mode: de-align (reset) the existing alignment and align it again.
            arguments << "--dealign";
            arguments << "--infile=" + inputMsaTmpFileUrl;
        }
        arguments << "--outfmt=clu";
        arguments << "--outfile=" + outputUrl;
        if (settings.numIterations != -1) {
            arguments << "--iterations" << QString::number(settings.numIterations);
        }
        if (settings.maxGuideTreeIterations != -1) {
            arguments << "--max-guidetree-iterations=" + QString::number(settings.maxGuideTreeIterations);
        }
        if (settings.maxHMMIterations != -1) {
            arguments << "--max-hmm-iterations=" + QString::number(settings.maxHMMIterations);
        }
        if (settings.setAutoOptions) {
            arguments << "--auto";
        }
        if (settings.numberOfProcessors > 0) {
            arguments << "--threads=" + QString::number(settings.numberOfProcessors);
        }

        clustalOTask = new ExternalToolRunTask(ClustalOSupport::ET_CLUSTALO_ID, arguments, new ClustalOLogParser());
        setListenerForTask(clustalOTask);
        clustalOTask->setSubtaskProgressWeight(95);
        res.append(clustalOTask);
    } else if (subTask == clustalOTask) {
        if (!QFileInfo(outputUrl).exists()) {
            ExternalTool* clustalOTool = AppContext::getExternalToolRegistry()->getById(ClustalOSupport::ET_CLUSTALO_ID);
            if (clustalOTool->isValid()) {
                stateInfo.setError(tr("Output file %1 not found").arg(outputUrl));
            } else {
                stateInfo.setError(tr("Output file %3 not found. May be %1 tool path '%2' not valid?")
                                       .arg(clustalOTool->getName())
                                       .arg(clustalOTool->getPath())
                                       .arg(outputUrl));
            }
            return res;
        }
        ioLog.details(tr("Loading output file '%1'").arg(outputUrl));
        IOAdapterFactory* ioFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
        loadTemporaryDocumentTask = new LoadDocumentTask(BaseDocumentFormats::CLUSTAL_ALN, outputUrl, ioFactory);
        loadTemporaryDocumentTask->setSubtaskProgressWeight(5);
        res.append(loadTemporaryDocumentTask);
    } else if (subTask == loadTemporaryDocumentTask) {
        tmpDoc = loadTemporaryDocumentTask->takeDocument();
        SAFE_POINT(tmpDoc != nullptr, QString("output document '%1' not loaded").arg(tmpDoc->getURLString()), res);
        SAFE_POINT(tmpDoc->getObjects().length() == 1, QString("no objects in output document '%1'").arg(tmpDoc->getURLString()), res);

        // Get the result alignment
        auto tmpMsaObject = qobject_cast<MultipleSequenceAlignmentObject*>(tmpDoc->getObjects().first());
        SAFE_POINT(tmpMsaObject != nullptr, "newDocument->getObjects().first() is not a MultipleSequenceAlignmentObject", res);

        resultMsa = tmpMsaObject->getMsaCopy();
        bool allRowsRestored = MSAUtils::restoreOriginalRowNamesFromIndexedNames(resultMsa, inputMsa->getRowNames(), INDEX_NAME_PREFIX);
        SAFE_POINT(allRowsRestored, "Failed to restore initial row names!", res);

        // If an alignment object has been specified, save the result to it
        if (objRef.isValid()) {
            GObject* dstObject = GObjectUtils::selectObjectByReference(objRef, UOF_LoadedOnly);
            if (dstObject != nullptr) {
                auto dstMsaObject = dynamic_cast<MultipleSequenceAlignmentObject*>(dstObject);
                SAFE_POINT(dstMsaObject != nullptr, "Failed to convert GObject to MultipleSequenceAlignmentObject during applying ClustalO results!", res);

                QList<int> removedRowIndexes;
                QList<int> addedRowIndexes;
                MSAUtils::assignOriginalDataIds(inputMsa, resultMsa, removedRowIndexes, addedRowIndexes);

                // Save data to the database
                {
                    CHECK_EXT(!lock.isNull(), stateInfo.setError("MultipleSequenceAlignment object has been changed"), res);
                    unlockMsaObject();

                    U2OpStatus2Log os;
                    U2UseCommonUserModStep userModStep(dstObject->getEntityRef(), os);
                    Q_UNUSED(userModStep);
                    CHECK_EXT(!os.hasError(), stateInfo.setError("Failed to apply the result of the alignment!"), res);

                    if (!removedRowIndexes.isEmpty()) {
                        // Find rows that were removed by ClustalO and remove them from MSA.
                        for (int i = removedRowIndexes.size(); --i >= 0;) {
                            int rowIndex = removedRowIndexes[i];
                            dstMsaObject->removeRow(rowIndex);
                        }
                    }
                    if (!addedRowIndexes.isEmpty()) {
                        QList<MultipleSequenceAlignmentRow> addedRows;
                        for (int i = 0; i < addedRowIndexes.size(); i++) {
                            int rowIndex = addedRowIndexes[i];
                            addedRows << resultMsa->getRow(rowIndex);
                        }
                        MSAUtils::addRowsToMsa(objRef.entityRef, addedRows, os);
                        CHECK_EXT(!os.hasError(), stateInfo.setError("Failed to add new rows to the alignment!"), res);

                        // Save rowIds & row-sequence-ids back to the resultMsaModel.
                        for (int i = 0; i < addedRowIndexes.size(); i++) {
                            int rowIndex = addedRowIndexes[i];
                            resultMsa->setRowId(rowIndex, addedRows[i]->getRowId());
                            resultMsa->setSequenceId(rowIndex, addedRows[i]->getRowDbInfo().sequenceId);
                        }
                    }

                    if (!removedRowIndexes.isEmpty() || !addedRowIndexes.isEmpty()) {
                        MaModificationInfo mi;
                        mi.rowContentChanged = false;
                        dstMsaObject->updateCachedMultipleAlignment(mi);
                    }

                    QMap<qint64, QVector<U2MsaGap>> rowsGapModel;
                    for (int i = 0, n = resultMsa->getRowCount(); i < n; ++i) {
                        qint64 rowId = resultMsa->getMsaRow(i)->getRowDbInfo().rowId;
                        const QVector<U2MsaGap>& newGapModel = resultMsa->getMsaRow(i)->getGaps();
                        rowsGapModel.insert(rowId, newGapModel);
                    }

                    dstMsaObject->updateGapModel(stateInfo, rowsGapModel);
                    SAFE_POINT_OP(stateInfo, res);

                    QList<qint64> resultRowIds = resultMsa->getRowsIds();
                    if (resultRowIds != inputMsa->getRowsIds()) {
                        dstMsaObject->updateRowsOrder(stateInfo, resultRowIds);
                        SAFE_POINT_OP(stateInfo, res);
                    }
                }

                Document* currentDocument = dstMsaObject->getDocument();
                SAFE_POINT(currentDocument != nullptr, "Document is NULL!", res);
                currentDocument->setModified(true);
            } else {
                algoLog.error(tr("Failed to apply the result of ClustalO: alignment object is not available!"));
                return res;
            }
        }

        algoLog.info(tr("ClustalO alignment successfully finished"));
        // new document deleted in destructor of LoadDocumentTask
    }
    return res;
}
Task::ReportResult ClustalOSupportTask::report() {
    unlockMsaObject();

    // Remove subdir for temporary files, that created in prepare.
    if (!inputMsaTmpFileUrl.isEmpty()) {
        QDir tmpDir(QFileInfo(inputMsaTmpFileUrl).absoluteDir());
        foreach (QString file, tmpDir.entryList()) {
            tmpDir.remove(file);
        }
        if (!tmpDir.rmdir(tmpDir.absolutePath())) {
            stateInfo.setError(tr("Can not remove folder for temporary files."));
        }
    }

    return ReportResult_Finished;
}

void ClustalOSupportTask::unlockMsaObject() {
    // Unlock the alignment
    if (lock.isNull() || !objRef.isValid()) {
        return;
    }
    GObject* obj = GObjectUtils::selectObjectByReference(objRef, UOF_LoadedOnly);
    if (obj != nullptr) {
        auto msaObject = dynamic_cast<MultipleSequenceAlignmentObject*>(obj);
        if (msaObject != nullptr && msaObject->isStateLocked()) {
            msaObject->unlockState(lock);
        }
        delete lock;
        lock = nullptr;
    }
}

////////////////////////////////////////
// ClustalOWithExtFileSpecifySupportTask
ClustalOWithExtFileSpecifySupportTask::ClustalOWithExtFileSpecifySupportTask(const ClustalOSupportTaskSettings& _settings)
    : Task("Run ClustalO alignment task", TaskFlags_NR_FOSCOE),
      settings(_settings) {
    GCOUNTER(cvar, "ClustalOSupportTask");
}

ClustalOWithExtFileSpecifySupportTask::~ClustalOWithExtFileSpecifySupportTask() {
    if (cleanDoc) {
        delete currentDocument;
    }
}

void ClustalOWithExtFileSpecifySupportTask::prepare() {
    DocumentFormatConstraints c;
    c.checkRawData = true;
    c.supportedObjectTypes += GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    c.rawData = IOAdapterUtils::readFileHeader(settings.inputFilePath);
    c.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);
    QList<DocumentFormatId> formats = AppContext::getDocumentFormatRegistry()->selectFormats(c);
    CHECK_EXT(!formats.isEmpty(), stateInfo.setError(tr("Unrecognized input alignment file format")), );

    DocumentFormatId alnFormat = formats.first();
    QVariantMap hints;
    if (alnFormat == BaseDocumentFormats::FASTA) {
        hints[DocumentReadingMode_SequenceAsAlignmentHint] = true;
    }
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(settings.inputFilePath));
    loadDocumentTask = new LoadDocumentTask(alnFormat, settings.inputFilePath, iof, hints);

    addSubTask(loadDocumentTask);
}

QList<Task*> ClustalOWithExtFileSpecifySupportTask::onSubTaskFinished(Task* subTask) {
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

        // Launch the task, objRef is empty - the input document maybe not in project
        clustalOSupportTask = new ClustalOSupportTask(mAObject->getMultipleAlignment(), GObjectReference(), settings);
        res.append(clustalOSupportTask);
    } else if (subTask == clustalOSupportTask) {
        // Set the result alignment to the alignment object of the current document
        mAObject = qobject_cast<MultipleSequenceAlignmentObject*>(currentDocument->getObjects().first());
        SAFE_POINT(mAObject != nullptr, QString("MA object not found!: %1").arg(loadDocumentTask->getURLString()), res);
        mAObject->updateGapModel(clustalOSupportTask->getResultAlignment()->getMsaRows());

        // Save the current document
        saveDocumentTask = new SaveDocumentTask(currentDocument,
                                                AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(settings.outputFilePath)),
                                                settings.outputFilePath);
        res.append(saveDocumentTask);
    } else if (subTask == saveDocumentTask) {
        Task* openTask = AppContext::getProjectLoader()->openWithProjectTask(settings.outputFilePath);
        if (openTask != nullptr) {
            res << openTask;
        }
    }
    return res;
}

Task::ReportResult ClustalOWithExtFileSpecifySupportTask::report() {
    return ReportResult_Finished;
}

////////////////////////////////////////
// ClustalOLogParser
ClustalOLogParser::ClustalOLogParser()
    : ExternalToolLogParser() {
}
void ClustalOLogParser::parseOutput(const QString& partOfLog) {
    lastPartOfLog = partOfLog.split(QChar('\r'));
    lastPartOfLog.first() = lastLine + lastPartOfLog.first();
    lastLine = lastPartOfLog.takeLast();
    foreach (QString buf, lastPartOfLog) {
        if (buf.contains("error", Qt::CaseInsensitive)) {
            ioLog.error(buf);
        } else {
            ioLog.trace(buf);
        }
    }
}
int ClustalOLogParser::getProgress() {
    if (!lastPartOfLog.isEmpty()) {
        QString lastMessage = lastPartOfLog.last();
        // 0..10% progress
        if (lastMessage.contains(QRegExp("Pairwise distance calculation progress: \\d+ %"))) {
            QRegExp rx("Pairwise distance calculation progress: (\\d+) %");
            rx.indexIn(lastMessage);
            CHECK(rx.captureCount() > 0, 0);
            return rx.cap(1).toInt() / 10;
        }
        // 10..20% progress
        if (lastMessage.contains(QRegExp("Distance calculation within sub-clusters: \\d+ %"))) {
            QRegExp rx("Distance calculation within sub-clusters: (\\d+) %");
            rx.indexIn(lastMessage);
            CHECK(rx.captureCount() > 0, 0);
            return rx.cap(1).toInt() / 10 + 10;
        }
        // 20..100% progress
        if (lastMessage.contains(QRegExp("Progressive alignment progress: (\\d+) %"))) {
            QRegExp rx("Progressive alignment progress: (\\d+) %");
            rx.indexIn(lastMessage);
            CHECK(rx.captureCount() > 0, 0);
            return (int)(rx.cap(1).toInt() * 0.8 + 20);
        }
    }
    return 0;
}
}  // namespace U2
