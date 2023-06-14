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

#include "KalignSupportTask.h"

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
#include <U2Core/Log.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include "KalignSupport.h"

namespace U2 {

Kalign3Settings::Kalign3Settings() {
    reset();
}

void Kalign3Settings::reset() {
    gapExtenstionPenalty = -1;
    gapOpenPenalty = -1;
    termGapPenalty = -1;
    secret = -1;
    inputFilePath = "";
}

Kalign3SupportTask::Kalign3SupportTask(const MultipleSequenceAlignment& _inputMsa, const GObjectReference& _objRef, const Kalign3Settings& _settings)
    : ExternalToolSupportTask("Run Kalign tool task", TaskFlags_NR_FOSCOE),
      inputMsa(_inputMsa->getExplicitCopy()),
      objRef(_objRef),
      settings(_settings),
      lock(nullptr) {
    GCOUNTER(cvar, "ExternalTool_Kalign");
    resultMA->setAlphabet(inputMsa->getAlphabet());
    resultMA->setName(inputMsa->getName());
}

Kalign3SupportTask::~Kalign3SupportTask() {
    delete tmpDoc;
    // Unlock the alignment object if the task has been failed
    if (!lock.isNull()) {
        if (objRef.isValid()) {
            GObject* obj = GObjectUtils::selectObjectByReference(objRef, UOF_LoadedOnly);
            if (nullptr != obj) {
                auto alObj = dynamic_cast<MultipleSequenceAlignmentObject*>(obj);
                CHECK(nullptr != alObj, );
                if (alObj->isStateLocked()) {
                    alObj->unlockState(lock);
                }
                delete lock;
                lock = nullptr;
            }
        }
    }
}

void Kalign3SupportTask::prepare() {
    if (inputMsa->getAlphabet()->getId() == BaseDNAAlphabetIds::RAW() ||
        inputMsa->getAlphabet()->getId() == BaseDNAAlphabetIds::AMINO_EXTENDED()) {  // TODO: recheck is extended alphabet is supported.
        setError(tr("Unsupported alphabet: %1").arg(inputMsa->getAlphabet()->getName()));
        return;
    }

    algoLog.info(tr("Kalign alignment started"));

    if (objRef.isValid()) {
        GObject* obj = GObjectUtils::selectObjectByReference(objRef, UOF_LoadedOnly);
        if (obj != nullptr) {
            auto alObj = dynamic_cast<MultipleSequenceAlignmentObject*>(obj);
            SAFE_POINT(nullptr != alObj, "Failed to convert GObject to MultipleSequenceAlignmentObject during applying Kalign results!", );
            lock = new StateLock("KalignAlignment");
            alObj->lockState(lock);
        }
    }

    // Add new subdir for temporary files
    // Folder name is ExternalToolName + CurrentDate + CurrentTime

    QString tmpDirName = "Kalign_" + QString::number(this->getTaskId()) + "_" +
                         QDate::currentDate().toString("dd.MM.yyyy") + "_" +
                         QTime::currentTime().toString("hh.mm.ss.zzz") + "_" +
                         QString::number(QCoreApplication::applicationPid()) + "/";
    QString tmpDirPath = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath(Kalign3Support::KALIGN_TMP_DIR) + "/" + tmpDirName;
    url = tmpDirPath + "tmp.fa";
    ioLog.details(tr("Saving data to temporary file '%1'").arg(url));

    // Check and remove subdir for temporary files
    QDir tmpDir(tmpDirPath);
    if (tmpDir.exists()) {
        foreach (QString file, tmpDir.entryList()) {
            tmpDir.remove(file);
        }
        if (!tmpDir.rmdir(tmpDir.absolutePath())) {
            stateInfo.setError(tr("Subdir for temporary files exists. Can not remove this folder."));
            return;
        }
    }
    if (!tmpDir.mkpath(tmpDirPath)) {
        stateInfo.setError(tr("Can not create folder for temporary files."));
        return;
    }

    saveTemporaryDocumentTask = new SaveMSA2SequencesTask(MSAUtils::createCopyWithIndexedRowNames(inputMsa), url, false, BaseDocumentFormats::FASTA);
    saveTemporaryDocumentTask->setSubtaskProgressWeight(5);
    addSubTask(saveTemporaryDocumentTask);
}

QList<Task*> Kalign3SupportTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (subTask->hasError()) {
        stateInfo.setError(subTask->getError());
        return res;
    }
    if (hasError() || isCanceled()) {
        return res;
    }
    QString outputUrl = url + ".out.aln";
    if (subTask == saveTemporaryDocumentTask) {
        QStringList arguments;
        if (url.contains(" ")) {
            stateInfo.setError("Temporary folder path have space(s). Try select any other folder without spaces.");
            return res;
        }
        // TODO: pass params
        //        arguments << url;
        //        arguments << "-output"
        //                  << "msf";
        //        if (settings.gapOpenPenalty != -1) {
        //            arguments << "-gapopen" << QString::number(settings.gapOpenPenalty);
        //        }
        //        if (settings.gapExtenstionPenalty != -1) {
        //            arguments << "-gapext" << QString::number(settings.gapExtenstionPenalty);
        //        }
        //        if (settings.numIterations != -1) {
        //            arguments << "-iterate" << QString::number(settings.numIterations);
        //        }
        arguments << "-i" << url;
        arguments << "-f" << "clu";
        arguments << "-o" << outputUrl;
        kalignTask = new ExternalToolRunTask(Kalign3Support::ET_KALIGN_ID, arguments, new KalignLogParser());
        setListenerForTask(kalignTask);
        kalignTask->setSubtaskProgressWeight(95);
        res.append(kalignTask);
    } else if (subTask == kalignTask) {
        if (!QFileInfo(outputUrl).exists()) {
            if (AppContext::getExternalToolRegistry()->getById(Kalign3Support::ET_KALIGN_ID)->isValid()) {
                stateInfo.setError(tr("Output file %1 not found").arg(outputUrl));
            } else {
                stateInfo.setError(tr("Output file %3 not found. May be %1 tool path '%2' not valid?")
                                       .arg(AppContext::getExternalToolRegistry()->getById(Kalign3Support::ET_KALIGN_ID)->getName())
                                       .arg(AppContext::getExternalToolRegistry()->getById(Kalign3Support::ET_KALIGN_ID)->getPath())
                                       .arg(outputUrl));
            }
            emit si_stateChanged();
            return res;
        }
        ioLog.details(tr("Loading output file '%1'").arg(outputUrl));

        IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
        loadTmpDocumentTask = new LoadDocumentTask(BaseDocumentFormats::CLUSTAL_ALN, outputUrl, iof);
        loadTmpDocumentTask->setSubtaskProgressWeight(5);
        res.append(loadTmpDocumentTask);
    } else if (subTask == loadTmpDocumentTask) {
        tmpDoc = loadTmpDocumentTask->takeDocument();
        SAFE_POINT(tmpDoc != nullptr, QString("output document '%1' not loaded").arg(tmpDoc->getURLString()), res);
        SAFE_POINT(tmpDoc->getObjects().length() != 0, QString("no objects in output document '%1'").arg(tmpDoc->getURLString()), res);

        // Get the result alignment
        const QList<GObject*>& newDocumentObjects = tmpDoc->getObjects();
        SAFE_POINT(!newDocumentObjects.empty(), "No objects in the temporary document!", res);

        auto newMAligmentObject = qobject_cast<MultipleSequenceAlignmentObject*>(newDocumentObjects.first());
        SAFE_POINT(nullptr != newMAligmentObject, "Failed to cast object from temporary document to an alignment!", res);

        resultMA = newMAligmentObject->getMsaCopy();
        bool renamed = MSAUtils::restoreOriginalRowNamesFromIndexedNames(resultMA, inputMsa->getRowNames());
        SAFE_POINT(renamed, "Failed to restore initial row names!", res);

        // If an alignment object has been specified, save the result to it
        if (objRef.isValid()) {
            GObject* obj = GObjectUtils::selectObjectByReference(objRef, UOF_LoadedOnly);
            if (nullptr != obj) {
                auto alObj = dynamic_cast<MultipleSequenceAlignmentObject*>(obj);
                SAFE_POINT(nullptr != alObj, "Failed to convert GObject to MultipleSequenceAlignmentObject during applying Kalign results!", res);

                MSAUtils::assignOriginalDataIds(inputMsa, resultMA, stateInfo);
                CHECK_OP(stateInfo, res);

                QMap<qint64, QVector<U2MsaGap>> rowsGapModel;
                for (int i = 0, n = resultMA->getRowCount(); i < n; ++i) {
                    qint64 rowId = resultMA->getMsaRow(i)->getRowDbInfo().rowId;
                    const QVector<U2MsaGap>& newGapModel = resultMA->getMsaRow(i)->getGaps();
                    rowsGapModel.insert(rowId, newGapModel);
                }

                // Save data to the database
                {
                    if (!lock.isNull()) {
                        if (alObj->isStateLocked()) {
                            alObj->unlockState(lock);
                        }
                        delete lock;
                        lock = nullptr;
                    } else {
                        stateInfo.setError("MultipleSequenceAlignment object has been changed");
                        return res;
                    }

                    U2OpStatus2Log os;
                    U2UseCommonUserModStep userModStep(obj->getEntityRef(), os);
                    if (os.hasError()) {
                        stateInfo.setError("Failed to apply the result of the alignment!");
                        return res;
                    }

                    alObj->updateGapModel(stateInfo, rowsGapModel);
                    SAFE_POINT_OP(stateInfo, res);

                    QList<qint64> resultRowIds = resultMA->getRowsIds();
                    if (resultRowIds != inputMsa->getRowsIds()) {
                        alObj->updateRowsOrder(stateInfo, resultRowIds);
                        SAFE_POINT_OP(stateInfo, res);
                    }
                }

                Document* currentDocument = alObj->getDocument();
                SAFE_POINT(nullptr != currentDocument, "Document is NULL!", res);
                currentDocument->setModified(true);
            } else {
                algoLog.error(tr("Failed to apply the result of Kalign: alignment object is not available!"));
                return res;
            }
        }

        algoLog.info(tr("KAlign alignment successfully finished"));
        // new document deleted in destructor of LoadDocumentTask
    }
    return res;
}

Task::ReportResult Kalign3SupportTask::report() {
    // Remove subdir for temporary files, that created in prepare
    if (!url.isEmpty()) {
        QDir tmpDir(QFileInfo(url).absoluteDir());
        foreach (QString file, tmpDir.entryList()) {
            tmpDir.remove(file);
        }
        if (!tmpDir.rmdir(tmpDir.absolutePath())) {
            stateInfo.setError(tr("Can not remove folder for temporary files."));
            emit si_stateChanged();
        }
    }

    return ReportResult_Finished;
}
////////////////////////////////////////
// KalignWithExtFileSpecifySupportTask
Kalign3WithExternalFileSupportTask::Kalign3WithExternalFileSupportTask(const Kalign3Settings& _settings)
    : Task("Run Kalign tool task", TaskFlags_NR_FOSCOE),
      settings(_settings) {
    GCOUNTER(cvar, "Kalign3SupportTask");
}

Kalign3WithExternalFileSupportTask::~Kalign3WithExternalFileSupportTask() {
    if (cleanDoc) {
        delete currentDocument;
    }
}

void Kalign3WithExternalFileSupportTask::prepare() {
    DocumentFormatConstraints c;
    c.checkRawData = true;
    c.supportedObjectTypes += GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    c.rawData = IOAdapterUtils::readFileHeader(settings.inputFilePath);
    c.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);
    QList<DocumentFormatId> formats = AppContext::getDocumentFormatRegistry()->selectFormats(c);
    if (formats.isEmpty()) {
        stateInfo.setError(tr("Unrecognized input alignment file format"));
        return;
    }

    DocumentFormatId alnFormat = formats.first();
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(settings.inputFilePath));
    QVariantMap hints;
    if (alnFormat == BaseDocumentFormats::FASTA) {
        hints[DocumentReadingMode_SequenceAsAlignmentHint] = true;
    }
    loadDocumentTask = new LoadDocumentTask(alnFormat, settings.inputFilePath, iof, hints);
    addSubTask(loadDocumentTask);
}

QList<Task*> Kalign3WithExternalFileSupportTask::onSubTaskFinished(Task* subTask) {
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
        kalign3SupportTask = new Kalign3SupportTask(mAObject->getMultipleAlignment(), GObjectReference(), settings);
        res.append(kalign3SupportTask);
    } else if (subTask == kalign3SupportTask) {
        // Set the result alignment to the alignment object of the current document
        mAObject = qobject_cast<MultipleSequenceAlignmentObject*>(currentDocument->getObjects().first());
        SAFE_POINT(mAObject != nullptr, QString("MA object not found!: %1").arg(loadDocumentTask->getURLString()), res);
        mAObject->updateGapModel(kalign3SupportTask->resultMA->getMsaRows());

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

Task::ReportResult Kalign3WithExternalFileSupportTask::report() {
    return ReportResult_Finished;
}

////////////////////////////////////////
// KalignLogParser
KalignLogParser::KalignLogParser() {
    progress = 0;
}

void KalignLogParser::parseOutput(const QString&) {
}

void KalignLogParser::parseErrOutput(const QString&) {
    // TODO
    //    lastPartOfLog = partOfLog.split(QRegExp("(\n|\r)"));
    //    lastPartOfLog.first() = lastErrLine + lastPartOfLog.first();
    //    lastErrLine = lastPartOfLog.takeLast();
    //    foreach (QString buf, lastPartOfLog) {
    //        if (buf.contains("WARNING")) {
    //            algoLog.info("Kalign: " + buf);
    //        } else {
    //            algoLog.trace(buf);
    //        }
    //    }
}

int KalignLogParser::getProgress() {
    // TODO    if (!lastPartOfLog.isEmpty()) {
    //        QString lastMessage = lastPartOfLog.last();
    //        if (lastMessage.contains(QRegExp("\\[Submit +Job\\]\\[TOT="))) {
    //            QRegExp rx("(.*)\\[ +(\\d+) %\\](.*)");
    //            rx.indexIn(lastMessage);
    //            CHECK(rx.captureCount() > 1, 0);
    //            return rx.cap(2).toInt();
    //        }
    //    }
    return progress;
}
}  // namespace U2
