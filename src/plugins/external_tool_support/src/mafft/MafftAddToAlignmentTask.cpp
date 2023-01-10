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

#include "MafftAddToAlignmentTask.h"

#include <QDir>
#include <QTemporaryFile>

#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>
#include <U2Algorithm/BaseAlignmentAlgorithmsIds.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/Log.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MultipleSequenceAlignmentExporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2Mod.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include "MAFFTSupport.h"
#include "MAFFTSupportTask.h"

namespace U2 {

static const int UNBREAKABLE_SEQUENCE_LENGTH_LIMIT = 50;

/************************************************************************/
/* MafftAddToAlignmentTask */
/************************************************************************/
MafftAddToAlignmentTask::MafftAddToAlignmentTask(const AlignSequencesToAlignmentTaskSettings& settings)
    : AbstractAlignmentTask(tr("Align sequences to alignment task"), TaskFlag_None),
      settings(settings),
      logParser(nullptr),
      saveSequencesDocumentTask(nullptr),
      saveAlignmentDocumentTask(nullptr),
      mafftTask(nullptr),
      loadTmpDocumentTask(nullptr),
      modStep(nullptr) {
    GCOUNTER(cvar, "MafftAddToAlignmentTask");

    SAFE_POINT_EXT(settings.isValid(), setError("Incorrect settings were passed into MafftAddToAlignmentTask"), );

    MultipleSequenceAlignmentExporter alnExporter;
    inputMsa = alnExporter.getAlignment(settings.msaRef.dbiRef, settings.msaRef.entityId, stateInfo);
    int rowNumber = inputMsa->getRowCount();
    for (int i = 0; i < rowNumber; i++) {
        inputMsa->renameRow(i, QString::number(i));
    }
}

static QString generateTmpFileUrl(const QString& filePathAndPattern) {
    QTemporaryFile* generatedFile = new QTemporaryFile(filePathAndPattern);
    QFileInfo generatedFileInfo(generatedFile->fileName());
    while (generatedFile->exists() || generatedFileInfo.baseName().contains(" ") || !generatedFile->open()) {
        delete generatedFile;
        generatedFile = new QTemporaryFile(filePathAndPattern);
    }
    generatedFile->close();
    QString result = generatedFile->fileName();
    delete generatedFile;
    return result;
}

void MafftAddToAlignmentTask::prepare() {
    algoLog.info(tr("Align sequences to alignment with MAFFT started"));

    MSAUtils::removeColumnsWithGaps(inputMsa, inputMsa->getRowCount());

    tmpDirUrl = ExternalToolSupportUtils::createTmpDir("add_to_alignment", stateInfo);

    QString tmpAddedUrl = generateTmpFileUrl(tmpDirUrl + QDir::separator() + "XXXXXXXXXXXXXXXX_add.fa");
    ;

    DocumentFormatRegistry* dfr = AppContext::getDocumentFormatRegistry();
    DocumentFormat* dfd = dfr->getFormatById(BaseDocumentFormats::FASTA);
    Document* tempDocument = dfd->createNewLoadedDocument(IOAdapterUtils::get(BaseIOAdapters::LOCAL_FILE), GUrl(tmpAddedUrl), stateInfo);

    QListIterator<QString> namesIterator(settings.addedSequencesNames);
    int currentRowNumber = inputMsa->getRowCount();
    foreach (const U2EntityRef& sequenceRef, settings.addedSequencesRefs) {
        uniqueIdsToNames[QString::number(currentRowNumber)] = namesIterator.next();
        U2SequenceObject seqObject(QString::number(currentRowNumber), sequenceRef);
        GObject* cloned = seqObject.clone(tempDocument->getDbiRef(), stateInfo);
        CHECK_OP(stateInfo, );
        cloned->setGObjectName(QString::number(currentRowNumber));
        tempDocument->addObject(cloned);
        currentRowNumber++;
    }

    saveSequencesDocumentTask = new SaveDocumentTask(tempDocument, tempDocument->getIOAdapterFactory(), tmpAddedUrl, SaveDocFlags(SaveDoc_Roll) | SaveDoc_DestroyAfter | SaveDoc_ReduceLoggingLevel);
    addSubTask(saveSequencesDocumentTask);

    QString tmpExistingAlignmentUrl = generateTmpFileUrl(tmpDirUrl + QDir::separator() + "XXXXXXXXXXXXXXXX.fa");

    saveAlignmentDocumentTask = new SaveMSA2SequencesTask(inputMsa, tmpExistingAlignmentUrl, false, BaseDocumentFormats::FASTA);
    addSubTask(saveAlignmentDocumentTask);
}

QList<Task*> MafftAddToAlignmentTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> subTasks;

    propagateSubtaskError();
    if (subTask->isCanceled() || isCanceled() || hasError()) {
        return subTasks;
    }

    if ((subTask == saveAlignmentDocumentTask || subTask == saveSequencesDocumentTask) && saveAlignmentDocumentTask->isFinished() && saveSequencesDocumentTask->isFinished()) {
        resultFilePath = settings.resultFileName.isEmpty() ? tmpDirUrl + QDir::separator() + "result_aln.fa" : settings.resultFileName.getURLString();
        QStringList arguments;
        if (settings.addAsFragments) {
            arguments << "--addfragments";
        } else {
            arguments << "--add";
        }
        arguments << saveSequencesDocumentTask->getURL().getURLString();
        const DNAAlphabet* alphabet = U2AlphabetUtils::getById(settings.alphabet);
        SAFE_POINT_EXT(alphabet != nullptr, setError("Albhabet is invalid."), subTasks);
        if (alphabet->isRaw()) {
            arguments << "--anysymbol";
        }
        if (useMemsaveOption()) {
            arguments << "--memsave";
        }
        if (settings.reorderSequences) {
            arguments << "--reorder";
        }
        arguments << saveAlignmentDocumentTask->getDocument()->getURLString();
        QString outputUrl = resultFilePath + ".out.fa";

        logParser = new MAFFTLogParser(inputMsa->getRowCount(), 1, outputUrl);
        mafftTask = new ExternalToolRunTask(MAFFTSupport::ET_MAFFT_ID, arguments, logParser);
        mafftTask->setStandartOutputFile(resultFilePath);
        mafftTask->setSubtaskProgressWeight(65);
        subTasks.append(mafftTask);
    } else if (subTask == mafftTask) {
        SAFE_POINT(logParser != nullptr, "logParser is null", subTasks);
        logParser->cleanup();
        if (!QFileInfo(resultFilePath).exists()) {
            if (AppContext::getExternalToolRegistry()->getById(MAFFTSupport::ET_MAFFT_ID)->isValid()) {
                stateInfo.setError(tr("Output file '%1' not found").arg(resultFilePath));
            } else {
                stateInfo.setError(tr("Output file '%3' not found. May be %1 tool path '%2' not valid?")
                                       .arg(AppContext::getExternalToolRegistry()->getById(MAFFTSupport::ET_MAFFT_ID)->getName())
                                       .arg(AppContext::getExternalToolRegistry()->getById(MAFFTSupport::ET_MAFFT_ID)->getPath())
                                       .arg(resultFilePath));
            }
            return subTasks;
        }
        ioLog.details(tr("Loading output file '%1'").arg(resultFilePath));
        IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
        loadTmpDocumentTask = new LoadDocumentTask(BaseDocumentFormats::FASTA, resultFilePath, iof);
        loadTmpDocumentTask->setSubtaskProgressWeight(5);
        subTasks.append(loadTmpDocumentTask);
    } else if (subTask == loadTmpDocumentTask) {
        modStep = new U2UseCommonUserModStep(settings.msaRef, stateInfo);
    }

    return subTasks;
}

void MafftAddToAlignmentTask::run() {
    CHECK_OP(stateInfo, );
    tpm = Progress_Manual;
    SAFE_POINT(loadTmpDocumentTask != nullptr, QString("Load task is NULL"), );
    tmpDoc = QSharedPointer<Document>(loadTmpDocumentTask->takeDocument(false));
    SAFE_POINT(tmpDoc != nullptr, QString("output document '%1' not loaded").arg(tmpDoc->getURLString()), );
    SAFE_POINT(tmpDoc->getObjects().length() != 0, QString("no objects in output document '%1'").arg(tmpDoc->getURLString()), );

    U2MsaDbi* dbi = modStep->getDbi()->getMsaDbi();

    QStringList rowNames = inputMsa->getRowNames();

    int posInMsa = 0;
    int objectsCount = tmpDoc->getObjects().count();
    bool hasDbiUpdates = false;

    U2AlphabetId currentAlphabet = dbi->getMsaAlphabet(settings.msaRef.entityId, stateInfo);
    CHECK_OP(stateInfo, );

    if (currentAlphabet != settings.alphabet) {
        hasDbiUpdates = true;
        dbi->updateMsaAlphabet(settings.msaRef.entityId, settings.alphabet, stateInfo);
        CHECK_OP(stateInfo, );
    }
    QMap<QString, qint64> uniqueNamesToIds;
    foreach (const MultipleSequenceAlignmentRow& refRow, inputMsa->getMsaRows()) {
        uniqueNamesToIds[refRow->getName()] = refRow->getRowId();
    }

    bool additionalModificationPerformed = false;
    QStringList unalignedSequences;
    foreach (GObject* object, tmpDoc->getObjects()) {
        if (hasError() || isCanceled()) {
            return;
        }
        stateInfo.setProgress(70 + 30 * posInMsa / objectsCount);
        U2SequenceObject* sequenceObject = qobject_cast<U2SequenceObject*>(object);
        bool rowWasAdded = true;
        if (!rowNames.contains(sequenceObject->getSequenceName())) {
            // inserting new rows
            sequenceObject->setGObjectName(uniqueIdsToNames[sequenceObject->getGObjectName()]);
            SAFE_POINT(sequenceObject != nullptr, "U2SequenceObject is null", );

            U2MsaRow row = MSAUtils::copyRowFromSequence(sequenceObject, settings.msaRef.dbiRef, stateInfo);

            rowWasAdded = row.length != 0;
            if (row.length - MsaRowUtils::getGapsLength(row.gaps) <= UNBREAKABLE_SEQUENCE_LENGTH_LIMIT) {
                if (MsaRowUtils::hasLeadingGaps(row.gaps)) {
                    row.gaps = row.gaps.mid(0, 1);
                } else {
                    row.gaps.clear();
                }
                additionalModificationPerformed = true;
            }

            if (rowWasAdded) {
                hasDbiUpdates = true;
                dbi->addRow(settings.msaRef.entityId, posInMsa, row, stateInfo);
                CHECK_OP(stateInfo, );
            } else {
                unalignedSequences << object->getGObjectName();
            }
        } else {
            // maybe need add leading gaps to original rows
            U2MsaRow row = MSAUtils::copyRowFromSequence(sequenceObject, settings.msaRef.dbiRef, stateInfo);
            qint64 rowId = uniqueNamesToIds.value(sequenceObject->getSequenceName(), -1);
            if (rowId == -1) {
                stateInfo.setError(tr("Row for updating doesn't found"));
                CHECK_OP(stateInfo, );
            }

            U2MsaRow currentRow = dbi->getRow(settings.msaRef.entityId, rowId, stateInfo);
            CHECK_OP(stateInfo, );
            QVector<U2MsaGap> modelToChop(currentRow.gaps);
            MsaRowUtils::chopGapModel(modelToChop, row.length);

            if (modelToChop != row.gaps) {
                hasDbiUpdates = true;
                dbi->updateGapModel(settings.msaRef.entityId, rowId, row.gaps, stateInfo);
                CHECK_OP(stateInfo, );
            }
        }

        if (additionalModificationPerformed) {
            algoLog.info(tr("Additional enhancement of short sequences alignment performed"));
        }

        if (rowWasAdded) {
            posInMsa++;
        }
    }

    if (!unalignedSequences.isEmpty()) {
        stateInfo.addWarning(tr("The following sequence(s) were not aligned as they do not contain meaningful characters: \"%1\".")
                                 .arg(unalignedSequences.join("\", \"")));
    }

    if (hasDbiUpdates) {
        MsaDbiUtils::trim(settings.msaRef, stateInfo);
        CHECK_OP(stateInfo, );
    }

    if (hasError()) {
        return;
    }
    algoLog.info(tr("MAFFT alignment successfully finished"));
}

Task::ReportResult MafftAddToAlignmentTask::report() {
    ExternalToolSupportUtils::removeTmpDir(tmpDirUrl, stateInfo);
    delete modStep;

    return ReportResult_Finished;
}

bool MafftAddToAlignmentTask::useMemsaveOption() const {
    qint64 maxLength = qMax(qint64(inputMsa->getLength()), settings.maxSequenceLength);
    qint64 memoryInMB = 10 * maxLength * maxLength / 1024 / 1024;
    AppResourcePool* pool = AppContext::getAppSettings()->getAppResourcePool();
    return memoryInMB > qMin(pool->getMaxMemorySizeInMB(), pool->getTotalPhysicalMemory() / 2);
}

AbstractAlignmentTask* MafftAddToAlignmentTaskFactory::getTaskInstance(AbstractAlignmentTaskSettings* _settings) const {
    AlignSequencesToAlignmentTaskSettings* addSettings = dynamic_cast<AlignSequencesToAlignmentTaskSettings*>(_settings);
    SAFE_POINT(addSettings != nullptr,
               "Add sequences to alignment: incorrect settings",
               nullptr);
    return new MafftAddToAlignmentTask(*addSettings);
}

MafftAlignSequencesToAlignmentAlgorithm::MafftAlignSequencesToAlignmentAlgorithm(const AlignmentAlgorithmType& type)
    : AlignmentAlgorithm(type,
                         type == AlignNewSequencesToAlignment
                             ? BaseAlignmentAlgorithmsIds::ALIGN_SEQUENCES_TO_ALIGNMENT_BY_MAFFT
                             : BaseAlignmentAlgorithmsIds::ALIGN_SELECTED_SEQUENCES_TO_ALIGNMENT_BY_MAFFT,
                         type == AlignNewSequencesToAlignment
                             ? AlignmentAlgorithmsRegistry::tr("Align sequences to alignment with MAFFT…")
                             : AlignmentAlgorithmsRegistry::tr("Align selected sequences to alignment with MAFFT…"),
                         new MafftAddToAlignmentTaskFactory()) {
}

bool MafftAlignSequencesToAlignmentAlgorithm::isAlgorithmAvailable() const {
    return AppContext::getExternalToolRegistry()->getById(MAFFTSupport::ET_MAFFT_ID)->isValid();
}

}  // namespace U2
