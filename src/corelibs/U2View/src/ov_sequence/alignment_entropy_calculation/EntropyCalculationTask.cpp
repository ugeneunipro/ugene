/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "EntropyCalculationTask.h"

#include <cmath>

#include <QFileInfo>
#include <QTextStream>

#include <U2Algorithm/BaseAlignmentAlgorithmsIds.h>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/PDBFormat.h>


#include <U2View/ADVSequenceObjectContext.h>

namespace U2 {
EntropyCalculationTask::EntropyCalculationTask(const QString& _alignmentFilePath, 
                                               const QString& _saveToPath,
                                               const QString& _originalFilePath,
                                               DNASequence& _sequence, 
                                               int _chainId)
    : Task(tr("Alignment entropy calculation"), TaskFlags_FOSE_COSC), 
    alignmentFilePath(_alignmentFilePath), 
    saveToPath(_saveToPath),
    originalFilePath(_originalFilePath),
    sequence(_sequence),
    chainId(_chainId){
}

void EntropyCalculationTask::prepare() {
    CHECK_EXT(sequence.alphabet->isAmino(), setError(tr("Sequence in focus does not have amino alphabet")), );
    CHECK_EXT(QFileInfo::exists(alignmentFilePath), setError(tr("File '%1' doesn't exist.").arg(alignmentFilePath)), );
    QVariantMap hints;
    hints[DocumentReadingMode_SequenceAsAlignmentHint] = true;
    loadDocumentTask = LoadDocumentTask::getDefaultLoadDocTask(stateInfo, alignmentFilePath, hints);
    CHECK_OP(stateInfo, );
    addSubTask(loadDocumentTask);
}


void EntropyCalculationTask::run() {
    calculateShannonEntropy();
    normalizeEntropy();
    writeEntropyToFile();
}

QList<Task*> EntropyCalculationTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (subTask == loadDocumentTask) {
        Document* doc = loadDocumentTask->getDocument();
        CHECK_EXT(doc != nullptr, setError(tr("Cannot load document from %1").arg(alignmentFilePath)), res);
        auto objects = doc->getObjects();
        CHECK_EXT(objects.size() == 1, setError(tr("More than 1 object found at %1").arg(alignmentFilePath)), res);
        alignment = qobject_cast<MultipleSequenceAlignmentObject*>(objects.at(0));
        CHECK_EXT(alignment != nullptr, setError(tr("Multiple alignment is expected as the input file: %1").arg(alignmentFilePath)), res);
        CHECK_EXT(alignment->getAlphabet()->isAmino(), setError(tr("Alignment does not have amino alphabet")), res);
        rollSequenceName();
        sequence.setName(newSequenceName);
        addSequenceTask = new AddSequenceObjectsToAlignmentTask(alignment, {sequence});
        res << addSequenceTask;
    } else if (subTask == addSequenceTask) {
        qint64 rowId = alignment->getMultipleAlignment()->getRow(newSequenceName)->getRowId();
        realignSequencesTask = new RealignSequencesInAlignmentTask(alignment, {rowId}, 
            BaseAlignmentAlgorithmsIds::ALIGN_SEQUENCES_TO_ALIGNMENT_BY_UGENE);
        res << realignSequencesTask;
    }
    return res;
}

/*save sequence name as it might be changed after alignment*/
void EntropyCalculationTask::rollSequenceName() {
    QSet<QString> rowNames;
    for (const auto& row : qAsConst(alignment->getRows())) {
        rowNames << row->getName();
    }
    newSequenceName = GUrlUtils::rollFileName(newSequenceName, "_", rowNames);
}

/*calculate Shannon entropy for every column of alignment where initial sequence does not have a gap*/
void EntropyCalculationTask::calculateShannonEntropy() {
    auto alignedSequence = alignment->getMultipleAlignment()->getRow(newSequenceName);
    int size = alignment->getMultipleAlignment()->getRowCount() - 1;
    CHECK_EXT(size != 0, setError(tr("Alignment is empty!")) ,);
    for (int i = alignedSequence->getCoreRegion().startPos; i < alignedSequence->getCoreRegion().endPos(); i++) {
        CHECK_CONTINUE(!alignedSequence->isGap(i));
        double columnEntropy = 0;
        QMap<char, qint64> counts;
        for (const auto& row : qAsConst(alignment->getRows())) {
            CHECK_CONTINUE(row->getName() != newSequenceName);
            counts[row->charAt(i)]++;
        }
        auto const values = counts.values();
        for (const char& aminoAcid : qAsConst(values)) {
            double p = (double)aminoAcid / size;
            columnEntropy -= p * std::log(p);
        }
        entropyForEveryColumn.append(columnEntropy);
    }
}

/*divide Shannon entropy values by max value (as temperature factor is greater than 1)*/
void EntropyCalculationTask::normalizeEntropy() {
    double maxValue = *std::max_element(entropyForEveryColumn.constBegin(), entropyForEveryColumn.constEnd());
    for (double& value : entropyForEveryColumn) {
        value /= maxValue;
    }
}

/*write Shannon entropy values to 'temperature factor' column of 'saveTo' file*/
void EntropyCalculationTask::writeEntropyToFile() {
    CHECK_EXT(QString::compare(originalFilePath, saveToPath, Qt::CaseInsensitive), 
        setError(tr("Original and destination files cannot have the same name: %1. Please change the 'Save to' path.").arg(originalFilePath)), );
    
    QScopedPointer<IOAdapter> readIO(IOAdapterUtils::open(originalFilePath, stateInfo, IOAdapterMode_Read));
    CHECK_OP(stateInfo, );
    QScopedPointer<IOAdapter> writeIO(IOAdapterUtils::open(saveToPath, stateInfo, IOAdapterMode_Write));
    CHECK_OP(stateInfo, );

    QVector<double>::const_iterator it = entropyForEveryColumn.constBegin();
    int currentChainIndex = 1;
    int residueSequenceNumber = -1;
    QString roundedEntropy;
    int seqResidueNumber = 0;
    while (!readIO->isEof()) {
        //PDB rows have 80 symbols
        const int max_size = 81;
        QByteArray byteLine(max_size + 1, 0);
        QString line(byteLine);
        CHECK_CONTINUE(!line.isEmpty());
       
        if (line.startsWith("TER")) {
            currentChainIndex++;
        } else if ((line.startsWith("ATOM") || line.startsWith("HETATM")) && it != entropyForEveryColumn.constEnd() && currentChainIndex == chainId) {
            int newResidueSeqNumber = line.mid(22, 4).trimmed().toInt();
            if (newResidueSeqNumber != residueSequenceNumber) {
                if (residueSequenceNumber != -1) {
                    it++;
                }
                QByteArray residueName = (line.mid(17, 3).trimmed()).toLocal8Bit();
                char acronym = PDBFormat::getAcronymByName(residueName);
                SAFE_POINT(acronym == sequence.constSequence().at(seqResidueNumber), " ", );
                seqResidueNumber++;

                residueSequenceNumber = newResidueSeqNumber;
                roundedEntropy = QString::number(*it, 'f', 2);
            }
            //entropy can have from 1 to 3 digits before decimal point, so we add spaces accordingly
            //temperature factor: positions 60-65
            line.replace(60, 6, roundedEntropy.prepend(QString(" ").repeated(6 - roundedEntropy.length())));
        }
        writeIO->writeBlock(line.toLocal8Bit());
    }
}

}  // namespace U2
