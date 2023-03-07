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

#include <QFileInfo>
#include <QTextStream>

#include <U2Algorithm/BaseAlignmentAlgorithmsIds.h>

#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/ADVSequenceObjectContext.h>

namespace U2 {
EntropyCalculationTask::EntropyCalculationTask(AnnotatedDNAView* _annotatedDNAView, 
                                               const QString& _alignmentFilePath, 
                                               const QString& _saveToPath, 
                                               const QString& _alignmentAlgorithm)
    : Task(tr("Alignment entropy calculation"), TaskFlags_FOSE_COSC), 
    annotatedDNAView(_annotatedDNAView),
    alignmentFilePath(_alignmentFilePath), 
    saveToPath(_saveToPath),
    alignmentAlgorithm(_alignmentAlgorithm) {
}

void EntropyCalculationTask::prepare() {
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
        CHECK_EXT(alignment != nullptr, setError(tr("Cannot cast to MultipleSequenceAlignmentObject: %1").arg(alignmentFilePath)), res);
        rollSequenceName();
        auto context = annotatedDNAView->getActiveSequenceContext();
        SAFE_POINT_EXT(context != nullptr, setError(L10N::nullPointerError("Active sequence context")), res);
        auto seqObject = context->getSequenceObject();
        SAFE_POINT_EXT(seqObject != nullptr, setError(L10N::nullPointerError("Sequence object")), res);
        auto sequence = seqObject->getSequence(U2_REGION_MAX, stateInfo);
        CHECK_OP(stateInfo, res);
        sequence.setName(newSequenceName);
        chainId = seqObject->getSequenceInfo().value("CHAIN_ID").toInt();
        addSequenceTask = new AddSequenceObjectsToAlignmentTask(alignment, {sequence});
        CHECK_OP(stateInfo, res);
        res << addSequenceTask;
    } else if (subTask == addSequenceTask) {
        qint64 rowId = alignment->getMultipleAlignment()->getRow(newSequenceName)->getRowId();
        if (alignmentAlgorithm == "UGENE") {
            realignSequencesTask = new RealignSequencesInAlignmentTask(alignment, {rowId}, 
                BaseAlignmentAlgorithmsIds::ALIGN_SEQUENCES_TO_ALIGNMENT_BY_UGENE);
        } else if (alignmentAlgorithm == "MAFFT") {
            //TODO
        } else if (alignmentAlgorithm == "MUSCLE") {
            //TODO  
        }
        CHECK_OP(stateInfo, res);
        res << realignSequencesTask;
    }
    return res;
}

void EntropyCalculationTask::rollSequenceName() {
    QSet<QString> rowNames;
    for (const auto& row : alignment->getRows()) {
        rowNames << row->getName();
    }
    newSequenceName = GUrlUtils::rollFileName(newSequenceName, "_", rowNames);
}

void EntropyCalculationTask::calculateShannonEntropy() {
    auto alignedSequence = alignment->getMultipleAlignment()->getRow(newSequenceName);
    int size = alignment->getMultipleAlignment()->getRowCount() - 1;
    CHECK_EXT(size != 0, setError(tr("Alignment is empty!")) ,);
    for (int i = alignedSequence->getCoreRegion().startPos; i < alignedSequence->getCoreRegion().endPos(); i++) {
        CHECK_CONTINUE(!alignedSequence->isGap(i));
        double columnEntropy = 0;
        QMap<char, qint64> counts;
        for (const auto& row : alignment->getRows()) {
            CHECK_CONTINUE(row->getName() != newSequenceName);
            counts[row->charAt(i)]++;
        }
        auto const values = counts.values();
        for (const char& aminoAcid : qAsConst(values)) {
            double p = (double)aminoAcid / size;
            columnEntropy -= p * log(p);
        }
        entropyForEveryColumn.append(columnEntropy);
    }
}

void EntropyCalculationTask::normalizeEntropy() {
    double maxValue = *std::max_element(entropyForEveryColumn.constBegin(), entropyForEveryColumn.constEnd());
    for (double& value : entropyForEveryColumn) {
        value /= maxValue;
    }
}

void EntropyCalculationTask::writeEntropyToFile() {
    auto context = annotatedDNAView->getActiveSequenceContext();
    SAFE_POINT_EXT(context != nullptr, setError(L10N::nullPointerError("Active sequence context")), );
    auto seqObject = context->getSequenceObject();
    SAFE_POINT_EXT(seqObject != nullptr, setError(L10N::nullPointerError("Sequence object")), );
    auto document = seqObject->getDocument();
    SAFE_POINT_EXT(document != nullptr, setError(L10N::nullPointerError("Document")), );
    QString filePath = document->getURLString();
    CHECK_EXT(QString::compare(filePath, saveToPath, Qt::CaseInsensitive), 
        setError(tr("Original and destination files cannot have the same name: %1. Please change the 'Save to' path.").arg(filePath)), );
    QFile readFile(filePath);
    CHECK_EXT(readFile.open(QIODevice::ReadOnly), setError(L10N::errorOpeningFileRead(filePath)), );
    QFile writeFile(saveToPath);
    CHECK_EXT(writeFile.open(QIODevice::WriteOnly), setError(L10N::errorOpeningFileWrite(saveToPath)), );
    QTextStream in(&readFile);
    QTextStream out(&writeFile);
    QVector<double>::const_iterator it = entropyForEveryColumn.constBegin();
    int currentChainIndex = 1;
    int residueSequenceNumber = -1;
    QString roundedEntropy;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("TER")) {
            currentChainIndex++;
        } else if ((line.startsWith("ATOM") || line.startsWith("HETATM")) && it != entropyForEveryColumn.constEnd() && currentChainIndex == chainId) {
            int newResidueSeqNumber = line.mid(22, 4).trimmed().toInt();
            if (newResidueSeqNumber != residueSequenceNumber) {
                if (residueSequenceNumber != -1) {
                    it++;
                }
                residueSequenceNumber = newResidueSeqNumber;
                roundedEntropy = QString::number(*it, 'f', 2);
            }
            //entropy can have from 1 to 3 digits before decimal point, so we add spaces accordingly
            //temperature factor: positions 60-65
            line.replace(60, 6, roundedEntropy.prepend(QString(" ").repeated(6 - roundedEntropy.length())));
        }
        out << line << endl;
    }
    readFile.close();
    writeFile.close();
}

}  // namespace U2
