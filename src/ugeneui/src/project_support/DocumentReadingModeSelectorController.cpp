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

#include <U2Algorithm/DnaAssemblyAlgRegistry.h>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/DocumentFormatUtils.h>

#include <U2Gui/HelpButton.h>
#include <U2Core/QObjectScopedPointer.h>

#include "DocumentReadingModeSelectorController.h"
#include "ui_SequenceReadingModeSelectorDialog.h"

namespace U2{


bool DocumentReadingModeSelectorController::adjustReadingMode(FormatDetectionResult& dr, bool forceOptions, bool optionsAlreadyChoosed) {
    // only sequence reading mode options are supported today

    // sequence reading: 
        // 1. as separate sequences
        // 2. as a single merged sequence
        // 3. as a multiple sequence alignment
        // 4. as reads to be aligned to reference

    QVariantMap& props = dr.rawDataCheckResult.properties;
    if(optionsAlreadyChoosed){
        return true;
    }
    bool sequenceFound = props.value(RawDataCheckResult_Sequence).toBool();
    if (!sequenceFound && forceOptions) {
        DocumentFormatConstraints dfc;
        dfc.supportedObjectTypes << GObjectTypes::SEQUENCE;
        sequenceFound = dr.format->checkConstraints(dfc);
    }
    bool multipleSequences = props.value(RawDataCheckResult_MultipleSequences).toBool();
    bool sequenceWithGaps = props.value(RawDataCheckResult_SequenceWithGaps).toBool();
    int minSequenceSize = props.value(RawDataCheckResult_MinSequenceSize).toInt();
    int maxSequenceSize = props.value(RawDataCheckResult_MinSequenceSize).toInt();
    
    if (!sequenceFound) {
        return true;
    }

    if (!forceOptions) {
        // if no multiple sequences -> no options, just 
        if (!multipleSequences) {
            return true;
        }
        // if sequence contains gap chars -> open it as alignment
        if (sequenceWithGaps) {
            props[DocumentReadingMode_SequenceAsAlignmentHint] = true;
            return true;
        }
    }
    Ui_SequenceReadingModeSelectorDialog ui;
    QObjectScopedPointer<QDialog> d = new QDialog(QApplication::activeWindow());
    d->setModal(true);
    ui.setupUi(d.data());

    new HelpButton(d.data(), ui.buttonBox, "");

    bool canBeShortReads = minSequenceSize > 0 && maxSequenceSize < 2000;
    bool haveReadAligners = !AppContext::getDnaAssemblyAlgRegistry()->getRegisteredAlgorithmIds().isEmpty();
    ui.refalignmentRB->setEnabled(canBeShortReads && haveReadAligners);
    bool mostProbableAreShortReads = canBeShortReads && (dr.format!=NULL && dr.format->getFormatId() == BaseDocumentFormats::FASTQ);//TODO: move to separate function
    ui.refalignmentRB->setChecked(ui.refalignmentRB->isEnabled() && mostProbableAreShortReads); 

    bool canBeMsa = forceOptions || (multipleSequences && maxSequenceSize/(minSequenceSize+1) < 20);
    ui.malignmentRB->setEnabled(canBeMsa);
    bool mostProbableIsMsa = sequenceWithGaps;
    ui.malignmentRB->setChecked(ui.malignmentRB->isEnabled() && mostProbableIsMsa);

    ui.previewEdit->setPlainText(dr.rawData);
    
    const int rc = d->exec();
    CHECK(!d.isNull(), false);
    
    if (rc == QDialog::Rejected) {
        return false;
    }

    if (ui.malignmentRB->isChecked()) {
        props[DocumentReadingMode_SequenceAsAlignmentHint] = true;
        return true;
    }
    if (ui.refalignmentRB->isChecked()) {
        props[DocumentReadingMode_SequenceAsShortReadsHint] = true;
        return true;
    }
    if (ui.mergeRB->isChecked()) {
        props[DocumentReadingMode_SequenceMergeGapSize] = ui.mergeSpinBox->value();
        return true;
    }
    if (ui.separateRB->isChecked()) {
        props[DocumentReadingMode_SequenceAsSeparateHint] = true;
        return true;
    }
    return true;
}

} //namespace
