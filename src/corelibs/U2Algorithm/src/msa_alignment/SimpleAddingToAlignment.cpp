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

#include <U2Algorithm/BaseAlignmentAlgorithmsIds.h>

#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/MSAUtils.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MultipleSequenceAlignmentExporter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2SafePoints.h>

#include "SimpleAddingToAlignment.h"

namespace U2 {

/************************************************************************/
/* SimpleAddToAlignmentTask */
/************************************************************************/
SimpleAddToAlignmentTask::SimpleAddToAlignmentTask(const AlignSequencesToAlignmentTaskSettings& settings)
    : AbstractAlignmentTask("Simple add to alignment task", TaskFlags_NR_FOSCOE), settings(settings) {
    GCOUNTER(cvar, tvar, "SimpleAddToAlignmentTask" );

    SAFE_POINT_EXT(settings.isValid(), setError("Incorrect settings were passed into SimpleAddToAlignmentTask"),);

    MultipleSequenceAlignmentExporter alnExporter;
    inputMsa = alnExporter.getAlignment(settings.msaRef.dbiRef, settings.msaRef.entityId, stateInfo);
}

void SimpleAddToAlignmentTask::prepare() {
    algoLog.info(tr("Align sequences to an existing alignment by UGENE started"));

    MSAUtils::removeColumnsWithGaps(inputMsa, inputMsa->getNumRows());

    QListIterator<QString> namesIterator(settings.addedSequencesNames);
    foreach(const U2EntityRef& sequence, settings.addedSequencesRefs) {
        if(hasError() || isCanceled()) {
            return;
        }
        BestPositionFindTask* findTask = new BestPositionFindTask(inputMsa, sequence, namesIterator.next(), settings.referenceRowId);
        findTask->setSubtaskProgressWeight(100.0 / settings.addedSequencesRefs.size());
        addSubTask(findTask);
    }
}

QList<Task*> SimpleAddToAlignmentTask::onSubTaskFinished(Task* subTask) {
    BestPositionFindTask* findTask = qobject_cast<BestPositionFindTask*>(subTask);
    sequencePositions[findTask->getSequenceId()] = findTask->getPosition();
    return QList<Task*>();
}

Task::ReportResult SimpleAddToAlignmentTask::report() {
    CHECK(!hasError() && !isCanceled(), ReportResult_Finished);
    U2UseCommonUserModStep modStep(settings.msaRef, stateInfo);
    CHECK_OP(stateInfo, ReportResult_Finished);
    U2MsaDbi *dbi = modStep.getDbi()->getMsaDbi();
    int posInMsa = inputMsa->getNumRows();
    bool hasDbiUpdates = false;

    U2AlphabetId currentAlphabetId = dbi->getMsaAlphabet(settings.msaRef.entityId, stateInfo);
    CHECK_OP(stateInfo, ReportResult_Finished);

    if (currentAlphabetId != settings.alphabet) {
        hasDbiUpdates = true;
        dbi->updateMsaAlphabet(settings.msaRef.entityId, settings.alphabet, stateInfo);
        CHECK_OP(stateInfo, ReportResult_Finished);
    }
    QListIterator<QString> namesIterator(settings.addedSequencesNames);

    const QList<qint64> rowsIds = inputMsa->getRowsIds();
    const U2MsaListGapModel msaGapModel = inputMsa->getGapModel();
    for (int i = 0; i < inputMsa->getNumRows(); i++) {
        U2MsaRow row = dbi->getRow(settings.msaRef.entityId, rowsIds[i], stateInfo);
        CHECK_OP(stateInfo, ReportResult_Finished);
        U2MsaRowGapModel modelToChop(msaGapModel[i]);
        MsaRowUtils::chopGapModel(modelToChop, row.length);
        CHECK_CONTINUE(modelToChop != row.gaps);

        hasDbiUpdates = true;
        MaDbiUtils::updateRowGapModel(settings.msaRef, rowsIds[i], msaGapModel[i], stateInfo);
        CHECK_OP(stateInfo, ReportResult_Finished);
    }

    QStringList unalignedSequences;
    foreach(const U2EntityRef& sequence, settings.addedSequencesRefs) {
        QString seqName = namesIterator.peekNext();
        U2SequenceObject seqObject(seqName, sequence);
        U2MsaRow row = MSAUtils::copyRowFromSequence(&seqObject, settings.msaRef.dbiRef, stateInfo);
        CHECK_OP(stateInfo, ReportResult_Finished);

        if (row.length != 0) {
            hasDbiUpdates = true;
            dbi->addRow(settings.msaRef.entityId, posInMsa, row, stateInfo);
            CHECK_OP(stateInfo, ReportResult_Finished);
            posInMsa++;

            if (sequencePositions.contains(seqName) && sequencePositions[seqName] > 0) {
                QList<U2MsaGap> gapModel;
                gapModel << U2MsaGap(0, sequencePositions[seqName]);
                U2MsaRow msaRow = dbi->getRow(settings.msaRef.entityId, row.rowId, stateInfo);
                CHECK_OP(stateInfo, ReportResult_Finished);

                if (msaRow.gaps != gapModel) {
                    hasDbiUpdates = true;
                    dbi->updateGapModel(settings.msaRef.entityId, msaRow.rowId, gapModel, stateInfo);
                    CHECK_OP(stateInfo, ReportResult_Finished);
                }
            }
        } else {
            unalignedSequences << seqName;
        }
        namesIterator.next();
    }

    if (!unalignedSequences.isEmpty()) {
        stateInfo.addWarning(tr("The following sequence(s) were not aligned as they do not contain meaningful characters: \"%1\".")
                                .arg(unalignedSequences.join("\", \"")));
    }

    if (hasDbiUpdates) {
        MsaDbiUtils::trim(settings.msaRef, stateInfo);
    }
    CHECK_OP(stateInfo, ReportResult_Finished);

    return ReportResult_Finished;
}


/************************************************************************/
/* BestPositionFindTask */
/************************************************************************/

BestPositionFindTask::BestPositionFindTask(const MultipleSequenceAlignment& alignment, const U2EntityRef& sequenceRef, const QString& sequenceId, int referenceRowId)
: Task(tr("Best position find task"), TaskFlag_None), inputMsa(alignment), sequenceRef(sequenceRef), sequenceId(sequenceId), bestPosition(0), referenceRowId(referenceRowId) {

}
void BestPositionFindTask::run() {
    U2SequenceObject dnaSeq("sequence", sequenceRef);
    QByteArray sequence = dnaSeq.getWholeSequenceData(stateInfo);
    sequence.replace(U2Msa::GAP_CHAR, "");
    CHECK_OP(stateInfo, );

    if(sequence.isEmpty()) {
        return;
    }
    if(!inputMsa->getAlphabet()->isCaseSensitive()) {
        sequence = sequence.toUpper();
    }
    const int aliLen = inputMsa->getLength();
    const int nSeq = inputMsa->getNumRows();

    int similarity = 0;

    if(referenceRowId >= 0) {
        const MultipleSequenceAlignmentRow row = inputMsa->getMsaRow(referenceRowId);
        int iterationsNum = aliLen - sequence.length() + 1;
        for (int p = 0; p < iterationsNum; p++ ) {
            stateInfo.setProgress(100 * p / iterationsNum);
            char c = row->charAt(p);
            int selLength = 0;
            int patternSimilarity = MSAUtils::getPatternSimilarityIgnoreGaps(row, p, sequence, selLength);
            if (U2Msa::GAP_CHAR != c && patternSimilarity > similarity) {
                similarity = patternSimilarity;
                bestPosition = p;
            }
        }
    } else {
        int processedRows = 0;
        foreach(const MultipleSequenceAlignmentRow &row, inputMsa->getMsaRows()) {
            stateInfo.setProgress(100 * processedRows / nSeq);
            for (int p = 0; p < ( aliLen - sequence.length() + 1 ); p++ ) {
                char c = row->charAt(p);
                int selLength = 0;
                int patternSimilarity = MSAUtils::getPatternSimilarityIgnoreGaps(row, p, sequence, selLength);
                if (U2Msa::GAP_CHAR != c && patternSimilarity > similarity) {
                    similarity = patternSimilarity;
                    bestPosition = p;
                }
            }
            processedRows++;
        }
    }
}

int BestPositionFindTask::getPosition() const {return bestPosition;}
const QString& BestPositionFindTask::getSequenceId() const {return sequenceId;}

AbstractAlignmentTask* SimpleAddToAlignmentTaskFactory::getTaskInstance(AbstractAlignmentTaskSettings *_settings) const {
    AlignSequencesToAlignmentTaskSettings* addSettings = dynamic_cast<AlignSequencesToAlignmentTaskSettings*>(_settings);
    SAFE_POINT(addSettings != NULL,
        "Add sequences to alignment: incorrect settings", NULL);
    return new SimpleAddToAlignmentTask(*addSettings);
}

SimpleAddToAlignmentAlgorithm::SimpleAddToAlignmentAlgorithm() :
AlignmentAlgorithm(AddToAlignment, BaseAlignmentAlgorithmsIds::ALIGN_SEQUENCES_TO_ALIGNMENT_BY_UGENE,
                   new SimpleAddToAlignmentTaskFactory(),
                   NULL)
{
}

} // U2
