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

Kalign3Settings Kalign3Settings::getDefaultAminoSettings() {
    Kalign3Settings result;
    result.gapOpenPenalty = 5.5;
    result.gapExtensionPenalty = 2.0;
    result.terminalGapExtensionPenalty = 1;
    return result;
}

Kalign3Settings Kalign3Settings::getDefaultDnaSettings() {
    Kalign3Settings result;
    result.gapOpenPenalty = 8;
    result.gapExtensionPenalty = 6;
    result.terminalGapExtensionPenalty = 0;
    return result;
}

Kalign3Settings Kalign3Settings::getDefaultRnaSettings() {
    Kalign3Settings result;
    result.gapOpenPenalty = 217;
    result.gapExtensionPenalty = 39.4;
    result.terminalGapExtensionPenalty = 292.6;
    return result;
}

Kalign3Settings Kalign3Settings::getDefaultSettings(const DNAAlphabet* alphabet) {
    if (alphabet->isAmino()) {
        return getDefaultAminoSettings();
    }
    if (alphabet->isDNA()) {
        return getDefaultDnaSettings();
    }
    if (alphabet->isRNA()) {
        return getDefaultRnaSettings();
    }
    return {};
}

Kalign3SupportTask::Kalign3SupportTask(const MultipleSequenceAlignment& _inputMsa, const GObjectReference& _objRef, const Kalign3Settings& _settings)
    : ExternalToolSupportTask("Kalign external tool task", TaskFlags_NR_FOSCOE),
      inputMsa(_inputMsa->getExplicitCopy()),
      objRef(_objRef),
      settings(_settings) {
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
            if (obj != nullptr) {
                auto alObj = dynamic_cast<MultipleSequenceAlignmentObject*>(obj);
                CHECK(alObj != nullptr, );
                if (alObj->isStateLocked()) {
                    alObj->unlockState(lock);
                }
                delete lock;
                lock = nullptr;
            }
        }
    }
}

bool Kalign3SupportTask::isAlphabetSupported(const QString& alphabetId) {
    return (alphabetId == BaseDNAAlphabetIds::NUCL_DNA_DEFAULT() ||
            alphabetId == BaseDNAAlphabetIds::NUCL_RNA_DEFAULT() ||
            alphabetId == BaseDNAAlphabetIds::NUCL_DNA_EXTENDED() ||
            alphabetId == BaseDNAAlphabetIds::NUCL_RNA_EXTENDED() ||
            alphabetId == BaseDNAAlphabetIds::AMINO_DEFAULT());  // TODO: recheck is extended alphabet is supported.
}

void Kalign3SupportTask::prepare() {
    if (!isAlphabetSupported(inputMsa->getAlphabet()->getId())) {
        setError(tr("Unsupported alphabet: %1").arg(inputMsa->getAlphabet()->getName()));
        return;
    }

    algoLog.info(tr("Kalign alignment started"));

    if (objRef.isValid()) {
        GObject* obj = GObjectUtils::selectObjectByReference(objRef, UOF_LoadedOnly);
        if (obj != nullptr) {
            auto alObj = dynamic_cast<MultipleSequenceAlignmentObject*>(obj);
            SAFE_POINT(alObj != nullptr, "Failed to convert GObject to MultipleSequenceAlignmentObject during applying Kalign results!", );
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

static const char* getAlignmentTypeParameterValue(const DNAAlphabet* alphabet) {
    if (alphabet->isRNA()) {
        return "rna";
    }
    if (alphabet->isDNA()) {
        return "dna";
    }
    if (alphabet->isAmino()) {
        return "protein";
    }
    return nullptr;
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
    QString outputUrl = url + ".out.msf";
    if (subTask == saveTemporaryDocumentTask) {
        QStringList arguments;
        if (url.contains(" ")) {
            stateInfo.setError("Temporary folder path have space(s). Try select any other folder without spaces.");
            return res;
        }
        arguments << "-i" << url;
        const DNAAlphabet* alphabet = this->inputMsa->getAlphabet();
        const char* alphabetType = getAlignmentTypeParameterValue(alphabet);
        if (alphabetType == nullptr) {
            stateInfo.setError(tr("Illegal MSA alphabet: %1").arg(alphabet->getName()));
            return res;
        }
        arguments << "--type" << alphabetType;
        arguments << "--format"
                  << "msf";
        arguments << "-o" << outputUrl;
        if (settings.gapOpenPenalty != Kalign3Settings::VALUE_IS_NOT_SET) {
            arguments << "--gpo" << QString::number(settings.gapOpenPenalty);
        }
        if (settings.gapExtensionPenalty != Kalign3Settings::VALUE_IS_NOT_SET) {
            arguments << "--gpe" << QString::number(settings.gapExtensionPenalty);
        }
        if (settings.terminalGapExtensionPenalty != Kalign3Settings::VALUE_IS_NOT_SET) {
            arguments << "--tgpe" << QString::number(settings.terminalGapExtensionPenalty);
        }
        if (settings.nThreads > 0) {
            arguments << "--nthreads" << QString::number(settings.nThreads);
        }

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
        loadTmpDocumentTask = new LoadDocumentTask(BaseDocumentFormats::MSF, outputUrl, iof);
        loadTmpDocumentTask->setSubtaskProgressWeight(5);
        res.append(loadTmpDocumentTask);
    } else if (subTask == loadTmpDocumentTask) {
        tmpDoc = loadTmpDocumentTask->takeDocument();
        SAFE_POINT(tmpDoc != nullptr, QString("output document '%1' not loaded").arg(tmpDoc->getURLString()), res);
        SAFE_POINT(tmpDoc->getObjects().length() != 0, QString("no objects in output document '%1'").arg(tmpDoc->getURLString()), res);

        // Get the result alignment
        const QList<GObject*>& newDocumentObjects = tmpDoc->getObjects();
        SAFE_POINT(!newDocumentObjects.empty(), "No objects in the temporary document!", res);
        SAFE_POINT(newDocumentObjects.size() == 1, "Result file contains multiple objects", res);
        auto newMsaObject = qobject_cast<MultipleSequenceAlignmentObject*>(newDocumentObjects.first());
        SAFE_POINT(newMsaObject != nullptr, "Failed to cast object from temporary document to alignment!", res);

        resultMA = newMsaObject->getMsaCopy();
        bool renamed = MSAUtils::restoreOriginalRowNamesFromIndexedNames(resultMA, inputMsa->getRowNames());
        SAFE_POINT(renamed, "Failed to restore initial row names!", res);

        // If an alignment object has been specified, save the result to it.
        if (objRef.isValid()) {
            GObject* obj = GObjectUtils::selectObjectByReference(objRef, UOF_LoadedOnly);
            if (obj == nullptr) {
                algoLog.error(tr("Failed to apply the result of Kalign: alignment object is not available!"));
                return res;
            }
            auto targetMsaObject = dynamic_cast<MultipleSequenceAlignmentObject*>(obj);
            SAFE_POINT(targetMsaObject != nullptr, "Failed to convert GObject to MultipleSequenceAlignmentObject during applying Kalign results!", res);

            MSAUtils::assignOriginalDataIds(inputMsa, resultMA, stateInfo);
            CHECK_OP(stateInfo, res);

            QMap<qint64, QVector<U2MsaGap>> rowsGapModel;
            for (int i = 0, n = resultMA->getRowCount(); i < n; ++i) {
                qint64 rowId = resultMA->getMsaRow(i)->getRowDbInfo().rowId;
                const QVector<U2MsaGap>& newGapModel = resultMA->getMsaRow(i)->getGaps();
                rowsGapModel.insert(rowId, newGapModel);
            }

            // Save data to the database
            if (!lock.isNull()) {
                if (targetMsaObject->isStateLocked()) {
                    targetMsaObject->unlockState(lock);
                }
                delete lock;
                lock = nullptr;
            } else {
                stateInfo.setError("MultipleSequenceAlignment object has been changed");
                return res;
            }
            Document* targetDocument = targetMsaObject->getDocument();
            SAFE_POINT(targetDocument != nullptr, "Document is NULL!", res);

            if (targetDocument->isStateLocked()) {
                stateInfo.setError(tr("Document '%1' is locked").arg(targetDocument->getName()));
                return res;
            }

            U2OpStatus2Log os;
            U2UseCommonUserModStep userModStep(obj->getEntityRef(), os);
            if (os.hasError()) {
                stateInfo.setError("Failed to apply the result of the alignment!");
                return res;
            }

            targetMsaObject->updateGapModel(stateInfo, rowsGapModel);
            SAFE_POINT_OP(stateInfo, res);

            QList<qint64> resultRowIds = resultMA->getRowsIds();
            if (resultRowIds != inputMsa->getRowsIds()) {
                targetMsaObject->updateRowsOrder(stateInfo, resultRowIds);
                SAFE_POINT_OP(stateInfo, res);
            }

            targetDocument->setModified(true);
        }

        algoLog.info(tr("KAlign alignment successfully finished"));
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
// Kalign3WithExternalFileSupportTask
Kalign3WithExternalFileSupportTask::Kalign3WithExternalFileSupportTask(const Kalign3Settings& _settings)
    : Task(tr("Kalign external tool task"), TaskFlags_NR_FOSCOE),
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
void KalignLogParser::parseErrOutput(const QString& err) {
    // Error output example:
    //[2023-07-06 16:36:21] :   ERROR : File: 12345 does not exist. (/home/ugene/yalgaer/kalign/kalign-3.3.5/lib/src/msa_io.c line 86)
    //[2023-07-06 16:36:21] :   ERROR : Function "kalign_read_input(param->infile[0], &msa,param->quiet)" failed. (/home/ugene/yalgaer/kalign/kalign-3.3.5/src/run_kalign.c line 346)
    //[2023-07-06 16:36:21] :   ERROR : Function "run_kalign(param)" failed. (/home/ugene/yalgaer/kalign/kalign-3.3.5/src/run_kalign.c line 326)
    QString firstErrorLine = err.split("\n")[0];
    QString prefixToken = "ERROR : ";
    int prefixTokenIndex = firstErrorLine.indexOf(prefixToken);
    if (prefixToken > 0) {
        firstErrorLine = firstErrorLine.mid(prefixTokenIndex + prefixToken.length()).trimmed();
        int suffixToken = firstErrorLine.lastIndexOf("(");
        if (suffixToken > 0) {
            firstErrorLine = firstErrorLine.mid(0, suffixToken - 1).trimmed();
        }
    }
    if (!firstErrorLine.isEmpty()) {
        setLastError(firstErrorLine);
    } else {
        ExternalToolLogParser::parseErrOutput(err);
    }
}

}  // namespace U2
