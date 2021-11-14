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

#include "MSAGeneralTab.h"

#include <U2Algorithm/MSAConsensusAlgorithmRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ShowHideSubgroupWidget.h>
#include <U2Gui/U2WidgetStateStorage.h>

#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorSequenceArea.h>

#include "../sort/MsaEditorSortSequencesWidget.h"

namespace U2 {

MSAGeneralTab::MSAGeneralTab(MSAEditor* msaEditor)
    : Ui_GeneralTabOptionsPanelWidget(), msaEditor(msaEditor), savableTab(this, GObjectViewUtils::findViewByName(msaEditor->getName())) {
    SAFE_POINT(msaEditor != nullptr, "MSA Editor is not defined.", );

    setupUi(this);

    auto alignmentInfo = new ShowHideSubgroupWidget("ALIGNMENT_INFO", tr("Alignment info"), alignmentInfoWidget, true);
    auto consensusMode = new ShowHideSubgroupWidget("CONSENSUS_MODE", tr("Consensus mode"), consensusModeWidget, true);
    // Note: use same action name with context menu in MSA-editor.
    auto copyType = new ShowHideSubgroupWidget("COPY_TYPE", tr("Copy (custom format)"), copyTypeWidget, true);
    auto sortType = new ShowHideSubgroupWidget("SORT_TYPE", tr("Sort sequences"), new MsaEditorSortSequencesWidget(nullptr, msaEditor), true);
    Ui_GeneralTabOptionsPanelWidget::layout->addWidget(alignmentInfo);
    Ui_GeneralTabOptionsPanelWidget::layout->addWidget(consensusMode);
    Ui_GeneralTabOptionsPanelWidget::layout->addWidget(copyType);
    Ui_GeneralTabOptionsPanelWidget::layout->addWidget(sortType);

    initializeParameters();
    connectSignals();

    U2WidgetStateStorage::restoreWidgetState(savableTab);

    updateState();
}

void MSAGeneralTab::sl_convertNucleicAlphabetButtonClicked() {
    if (msaEditor->convertDnaToRnaAction->isEnabled()) {
        msaEditor->convertDnaToRnaAction->trigger();
    } else if (msaEditor->convertRnaToDnaAction->isEnabled()) {
        msaEditor->convertRnaToDnaAction->trigger();
    } else if (msaEditor->convertRawToDnaAction->isEnabled()) {
        msaEditor->convertRawToDnaAction->trigger();
    }
}

void MSAGeneralTab::sl_alignmentChanged() {
    updateState();
}

void MSAGeneralTab::sl_copyFormatSelectionChanged(int index) {
    QString formatId = copyType->itemData(index).toString();
    msaEditor->getUI()->getUI()->getSequenceArea()->sl_changeCopyFormat(formatId);
}

void MSAGeneralTab::connectSignals() {
    MaEditorSequenceArea *sequenceArea = msaEditor->getUI()->getUI()->getSequenceArea();

    // Inner signals
    connect(copyType, SIGNAL(currentIndexChanged(int)), SLOT(sl_copyFormatSelectionChanged(int)));
    connect(copyButton, SIGNAL(clicked()), sequenceArea, SLOT(sl_copySelectionFormatted()));
    connect(convertAminoAlphabetButton, SIGNAL(clicked()), msaEditor->convertRawToAminoAction, SLOT(trigger()));
    connect(convertNucleicAlphabetButton, SIGNAL(clicked()), SLOT(sl_convertNucleicAlphabetButtonClicked()));

    // External signals
    MultipleSequenceAlignmentObject* maObject = msaEditor->getMaObject();
    connect(maObject,
            SIGNAL(si_alignmentChanged(const MultipleAlignment&, const MaModificationInfo&)),
            SLOT(sl_alignmentChanged()));
    connect(maObject, SIGNAL(si_lockedStateChanged()), SLOT(sl_alignmentChanged()));
    connect(sequenceArea, SIGNAL(si_copyFormattedChanging(bool)), copyButton, SLOT(setEnabled(bool)));
}

void MSAGeneralTab::initializeParameters() {
    // Alignment info
    alignmentAlphabet->setText(msaEditor->getMaObject()->getAlphabet()->getName());
    alignmentLength->setText(QString::number(msaEditor->getAlignmentLen()));
    alignmentHeight->setText(QString::number(msaEditor->getNumSequences()));

    // Consensus type combobox
    consensusModeWidget->init(msaEditor->getMaObject(), msaEditor->getUI()->getUI()->getConsensusArea());

    // Copy formatted
    copyButton->setToolTip(msaEditor->getUI()->getUI()->copyFormattedSelectionAction->toolTip());

    DocumentFormatConstraints constr;
    constr.supportedObjectTypes.insert(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    constr.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);
    constr.addFlagToSupport(DocumentFormatFlag_SupportWriting);
    DocumentFormatRegistry* formatRegistry = AppContext::getDocumentFormatRegistry();
    QList<DocumentFormatId> supportedFormats = formatRegistry->selectFormats(constr);
    supportedFormats.append(BaseDocumentFormats::PLAIN_TEXT);

    for (const DocumentFormatId& fid : qAsConst(supportedFormats)) {
        DocumentFormat* format = formatRegistry->getFormatById(fid);
        copyType->addItem(QIcon(), format->getFormatName(), format->getFormatId());
    }
    copyType->addItem(QIcon(), tr("Rich text (HTML)"), "RTF");
    copyType->model()->sort(0);

    QString currentCopyFormattedID = msaEditor->getUI()->getUI()->getSequenceArea()->getCopyFormattedAlgorithmId();
    copyType->setCurrentIndex(copyType->findData(currentCopyFormattedID));
}

void MSAGeneralTab::updateState() {
    alignmentAlphabet->setText(msaEditor->getMaObject()->getAlphabet()->getName());
    alignmentLength->setText(QString::number(msaEditor->getAlignmentLen()));
    alignmentHeight->setText(QString::number(msaEditor->getNumSequences()));

    consensusModeWidget->updateState();
    copyButton->setEnabled(msaEditor->getUI()->getUI()->copyFormattedSelectionAction->isEnabled());
    updateConvertAlphabetButtonState();
}

void MSAGeneralTab::updateConvertAlphabetButtonState() {
    bool isDnaToRnaEnabled = msaEditor->convertDnaToRnaAction->isEnabled();
    bool isRnaToDnaEnabled = msaEditor->convertRnaToDnaAction->isEnabled();
    bool isRawToDnaEnabled = msaEditor->convertRawToDnaAction->isEnabled();
    bool isRawToAminoEnabled = msaEditor->convertRawToAminoAction->isEnabled();
    convertNucleicAlphabetButton->setVisible(isDnaToRnaEnabled || isRnaToDnaEnabled || isRawToDnaEnabled);
    convertAminoAlphabetButton->setVisible(isRawToAminoEnabled);

    if (isDnaToRnaEnabled) {
        convertNucleicAlphabetButton->setText(tr("RNA"));
        convertNucleicAlphabetButton->setToolTip(tr("Convert DNA alignment to RNA alignment"));
    } else if (isRnaToDnaEnabled) {
        convertNucleicAlphabetButton->setText(tr("DNA"));
        convertNucleicAlphabetButton->setToolTip(tr("Convert RNA alignment to DNA alignment"));
    } else if (isRawToDnaEnabled) {
        convertNucleicAlphabetButton->setText(tr("DNA"));
        convertNucleicAlphabetButton->setToolTip(tr("Convert RAW alignment to DNA alignment"));
    } else {
        convertNucleicAlphabetButton->setText("");
        convertNucleicAlphabetButton->setToolTip("");
    }
}

}  // namespace U2
