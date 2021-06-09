/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "PCRPrimerDesignForDNAAssemblyOPWidget.h"

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>
#include <U2Gui/U2WidgetStateStorage.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include <QVBoxLayout>

namespace U2 {

const QString PCRPrimerDesignForDNAAssemblyOPWidget::USER_PRIMERS_SHOW_HIDE_ID = "user-primers-show-hide-id";
const QString PCRPrimerDesignForDNAAssemblyOPWidget::PARAMETERS_OF_PRIMING_SEQUENCES_SHOW_HIDE_ID = "parameters-of-priming-sequences-show-hide-id";
const QString PCRPrimerDesignForDNAAssemblyOPWidget::PARAMETERS_2_EXCLUDE_IN_WHOLE_PRIMERS_SHOW_HIDE_ID = "parameters-2-exclude-in-whole-primers-show-hide-id";
const QString PCRPrimerDesignForDNAAssemblyOPWidget::SELECT_AREAS_FOR_PRIMING_SHOW_HIDE_ID = "select-areas-for-priming-show-hide-id";
const QString PCRPrimerDesignForDNAAssemblyOPWidget::OPEN_BACKBONE_SEQUENCE_SHOW_HIDE_ID = "open-backbone-sequence-show-hide-id";
const QString PCRPrimerDesignForDNAAssemblyOPWidget::GENERATE_SEQUENCE_SHOW_HIDE_ID = "generate-sequence-show-hide-id";
const QString PCRPrimerDesignForDNAAssemblyOPWidget::OTHER_SEQUENCES_IN_PCR_REACTION_SHOW_HIDE_ID = "other-sequences-in-pcr-reaction-show-hide-id";

PCRPrimerDesignForDNAAssemblyOPWidget::PCRPrimerDesignForDNAAssemblyOPWidget(AnnotatedDNAView* _annDnaView)
    : QWidget(nullptr),
      annDnaView(_annDnaView),
      savableWidget(this, GObjectViewUtils::findViewByName(annDnaView->getName())) {
    setupUi(this);
    parametersMinMaxSpinBoxes = { { sbMinRequireGibbs, sbMaxRequireGibbs },
                                  { spMinRequireMeltingTeml, spMaxRequireMeltingTeml },
                                  { spMinRequireOverlapLength, spMaxRequireOverlapLength },
                                  { sbMinExcludeGibbs, sbMaxExcludeGibbs },
                                  { spMinExcludeMeltingTeml, spMaxExcludeMeltingTeml },
                                  { spMinExcludeOverlapLength, spMaxExcludeOverlapLength },
                                  { sbLeftAreaStart, sbLeftAreaEnd },
                                  { sbRightAreaStart, sbRightAreaEnd } };

    userPrimersShowHideGroup->init(USER_PRIMERS_SHOW_HIDE_ID, tr("User primers"), wgtUserPrimers, true);
    parametersOfPrimingSequencesShowHideGroup->init(PARAMETERS_OF_PRIMING_SEQUENCES_SHOW_HIDE_ID,
                                                    tr("Parameters of priming sequences"),
                                                    wgtParametersOfPrimingSequences,
                                                    true);
    parameters2ExcludeInWholePrimersShowHideGroup->init(PARAMETERS_2_EXCLUDE_IN_WHOLE_PRIMERS_SHOW_HIDE_ID,
                                                        tr("Parameters to exclude in whole primers"),
                                                        wgtParameters2ExcludeInWholePrimers,
                                                        true);
    areasForPrimingShowHideGroup->init(SELECT_AREAS_FOR_PRIMING_SHOW_HIDE_ID,
                                       tr("Select areas for priming search"),
                                       wgtAreasForPriming,
                                       true);
    openBackboneSequenceShowHideGroup->init(OPEN_BACKBONE_SEQUENCE_SHOW_HIDE_ID,
                                            tr("Open the backbone sequence"),
                                            wgtOpenBackboneSequence,
                                            true);
    generateSequenceShowHideGroup->init(GENERATE_SEQUENCE_SHOW_HIDE_ID,
                                        tr("Generate a sequence"),
                                        wgtGenerateSequence,
                                        true);
    otherSequencesInPCRShowHideGroup->init(OTHER_SEQUENCES_IN_PCR_REACTION_SHOW_HIDE_ID,
                                           tr("Other sequences in PCR reaction"),
                                           wgtOtherSequencesInPcr,
                                           true);

    auto activeSequenceContext = annDnaView->getActiveSequenceContext();
    SAFE_POINT(activeSequenceContext != nullptr, L10N::nullPointerError("ADVSequenceObjectContext"), );

    auto seqLength = activeSequenceContext->getSequenceLength();

    // Default values, have been provided by the customer
    // Left area: from "1" to "10% of sequence length"
    // Right area: from "50% of sequence length" to "60% of sequence length"
    sbLeftAreaStart->setValue(1);
    sbLeftAreaEnd->setValue(seqLength / 10);
    sbRightAreaStart->setValue(seqLength / 10 * 5);
    sbRightAreaEnd->setValue(seqLength / 10 * 6);

    connect(pbStart, &QAbstractButton::clicked, this, &PCRPrimerDesignForDNAAssemblyOPWidget::sl_start);
    connect(tbLeftAreaSelectManually, &QAbstractButton::clicked, this, &PCRPrimerDesignForDNAAssemblyOPWidget::sl_selectManually);
    connect(tbRightAreaSelectManually, &QAbstractButton::clicked, this, &PCRPrimerDesignForDNAAssemblyOPWidget::sl_selectManually);

    U2WidgetStateStorage::restoreWidgetState(savableWidget);

    sl_updateMaxValues();
    for (auto minSb : parametersMinMaxSpinBoxes.keys()) {
        connect(minSb, &QSpinBox::editingFinished, this, &PCRPrimerDesignForDNAAssemblyOPWidget::sl_minValueChanged);
        connect(parametersMinMaxSpinBoxes[minSb], &QSpinBox::editingFinished, this, &PCRPrimerDesignForDNAAssemblyOPWidget::sl_maxValueChanged);
    }
    connect(tbLoadBackbone, &QAbstractButton::clicked, this, &PCRPrimerDesignForDNAAssemblyOPWidget::sl_loadBackbone);
    connect(tbsaveRandomSequences, &QAbstractButton::clicked, this, &PCRPrimerDesignForDNAAssemblyOPWidget::sl_saveRandomSequences);
    connect(tbLoadOtherSequencesInPcr, &QAbstractButton::clicked, this, &PCRPrimerDesignForDNAAssemblyOPWidget::sl_loadOtherSequenceInPcr);
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_start() {
    //TODO
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_selectManually() {
    auto currentSMButton = qobject_cast<QToolButton*>(sender());
    SAFE_POINT(currentSMButton != nullptr, L10N::nullPointerError("QAbstractButton"), );

    if (smButton != nullptr) {
        bool disableState = true;
        if (smButton->isChecked()) {
            smButton->setChecked(false);
            disableState = false;
        }
        smButton = nullptr;
        sbStartRegion = nullptr;
        sbEndRegion = nullptr;
        disconnect(updateRegionConnection);
        updateRegionConnection = QMetaObject::Connection();

        if (disableState) {
            return;
        }
    }

    smButton = currentSMButton;
    if (smButton == tbLeftAreaSelectManually) {
        sbStartRegion = sbLeftAreaStart;
        sbEndRegion = sbLeftAreaEnd;
    } else {
        sbStartRegion = sbRightAreaStart;
        sbEndRegion = sbRightAreaEnd;
    }
    auto activeSequenceContext = annDnaView->getActiveSequenceContext();
    SAFE_POINT(activeSequenceContext != nullptr, L10N::nullPointerError("ADVSequenceObjectContext"), );

    auto sequenceSelection = activeSequenceContext->getSequenceSelection();
    SAFE_POINT(sequenceSelection != nullptr, L10N::nullPointerError("DNASequenceSelection"), );

    updateRegionConnection = connect(sequenceSelection, &DNASequenceSelection::si_onSelectionChanged, this, &PCRPrimerDesignForDNAAssemblyOPWidget::sl_updateRegion);
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_updateRegion() {
    auto sequenceSelection = qobject_cast<DNASequenceSelection*>(sender());
    SAFE_POINT(sequenceSelection != nullptr, L10N::nullPointerError("DNASequenceSelection"), );

    const auto& selectedRegions = sequenceSelection->getSelectedRegions();
    CHECK(!selectedRegions.isEmpty(), );

    if (selectedRegions.size() > 1) {
        coreLog.details(tr("Multiple regions selection, the only first is set as primer search area."));
    }
    const auto& region = selectedRegions.first();
    SAFE_POINT(sbStartRegion != nullptr, L10N::nullPointerError("QSpinBox"), );

    sbStartRegion->setValue(region.startPos + 1);
    SAFE_POINT(sbEndRegion != nullptr, L10N::nullPointerError("QSpinBox"), );

    sbEndRegion->setValue(region.endPos());
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_updateMaxValues() {
    auto activeSequenceContext = annDnaView->getActiveSequenceContext();
    SAFE_POINT(activeSequenceContext != nullptr, L10N::nullPointerError("ADVSequenceObjectContext"), );

    auto seqLength = activeSequenceContext->getSequenceLength();
    for (const auto& minSb : parametersMinMaxSpinBoxes.keys()) {
        auto maxSb = parametersMinMaxSpinBoxes[minSb];
        minSb->setMaximum(maxSb->value() - 1);
        maxSb->setMinimum(minSb->value() + 1);
        maxSb->setMaximum(seqLength);
    }
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_minValueChanged() {
    auto minSb = qobject_cast<QSpinBox*>(sender());
    SAFE_POINT(minSb != nullptr, L10N::nullPointerError("QSpinBox"), );

    auto maxSb = parametersMinMaxSpinBoxes.value(minSb);
    SAFE_POINT(maxSb != nullptr, L10N::nullPointerError("QSpinBox"), );

    maxSb->setMinimum(minSb->value() + 1);
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_maxValueChanged() {
    auto maxSb = qobject_cast<QSpinBox*>(sender());
    SAFE_POINT(maxSb != nullptr, L10N::nullPointerError("QSpinBox"), );

    auto minSb = parametersMinMaxSpinBoxes.key(maxSb);
    SAFE_POINT(minSb != nullptr, L10N::nullPointerError("QSpinBox"), );

    minSb->setMaximum(maxSb->value() - 1);
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_loadBackbone() {
    QString filter = DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::SEQUENCE, true);
    QString selectedFilter = DialogUtils::prepareDocumentsFileFilter(BaseDocumentFormats::FASTA, false);
    LastUsedDirHelper lod;
    QString file = U2FileDialog::getOpenFileName(nullptr, tr("Select a backbone sequence file"), lod, filter, &selectedFilter);
    CHECK(!file.isEmpty(), );

    leBackboneFilePath->setText(file);
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_saveRandomSequences() {
    QString filter = DialogUtils::prepareDocumentsFileFilter(BaseDocumentFormats::FASTA, false);
    LastUsedDirHelper lod;
    QString file = U2FileDialog::getSaveFileName(nullptr, tr("Save a random sequences file"), lod.dir, filter);
    CHECK(!file.isEmpty(), );

    leRandomSequencesFilePath->setText(file);
}

void PCRPrimerDesignForDNAAssemblyOPWidget::sl_loadOtherSequenceInPcr() {
    const QString filter = DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::SEQUENCE, true);
    QString selectedFilter = DialogUtils::prepareDocumentsFileFilter(BaseDocumentFormats::FASTA, false);
    LastUsedDirHelper lod;
    QString file = U2FileDialog::getOpenFileName(nullptr, tr("Select an \"Other sequences in PCR reaction\" file"), lod, filter, &selectedFilter);
    CHECK(!file.isEmpty(), );

    leOtherSequencesInPcrFilePath->setText(file);

}

}
