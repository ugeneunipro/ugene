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

#include <U2Algorithm/MSAConsensusAlgorithmRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/ShowHideSubgroupWidget.h>
#include <U2Gui/U2WidgetStateStorage.h>

#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorSequenceArea.h>

#include "MSAGeneralTab.h"

namespace U2 {

MSAGeneralTab::MSAGeneralTab(MSAEditor* _msa)
    : msa(_msa), savableTab(this, GObjectViewUtils::findViewByName(_msa->getName()))
{
    SAFE_POINT(NULL != msa, "MSA Editor not defined.", );

    setupUi(this);

    ShowHideSubgroupWidget* alignmentInfo = new ShowHideSubgroupWidget("ALIGNMENT_INFO", tr("Alignment info"), alignmentInfoWidget, true);
    ShowHideSubgroupWidget* consensusMode = new ShowHideSubgroupWidget("CONSENSUS_MODE", tr("Consensus mode"), consensusModeWidget, true);
    ShowHideSubgroupWidget* copyType = new ShowHideSubgroupWidget("COPY_TYPE", tr("Copy to clipboard"), copyTypeWidget, true);
    Ui_GeneralTabOptionsPanelWidget::layout->addWidget(alignmentInfo);
    Ui_GeneralTabOptionsPanelWidget::layout->addWidget(consensusMode);
    Ui_GeneralTabOptionsPanelWidget::layout->addWidget(copyType);

    initializeParameters();
    connectSignals();

    copyButton->setEnabled(msa->getUI()->getCopyFormattedSelectionAction()->isEnabled());

    U2WidgetStateStorage::restoreWidgetState(savableTab);

#ifdef Q_OS_MAC
    copyButton->setToolTip("Cmd+Shift+C");
#else
    copyButton->setToolTip("Ctrl+Shift+C");
#endif

}

void MSAGeneralTab::sl_alignmentChanged() {
    alignmentAlphabet->setText(msa->getMaObject()->getAlphabet()->getName());
    alignmentLength->setText(QString::number(msa->getAlignmentLen()));
    alignmentHeight->setText(QString::number(msa->getNumSequences()));
}

void MSAGeneralTab::sl_copyFormatSelectionChanged(int index) {
    QString selectedFormatId = copyType->itemData(index).toString();
    emit si_copyFormatChanged(selectedFormatId);
}

void MSAGeneralTab::sl_copyFormatted(){
    emit si_copyFormatted();
}

void MSAGeneralTab::sl_copyFormatStatusChanged(bool enabled){
    copyButton->setEnabled(enabled);
}

void MSAGeneralTab::connectSignals() {
    // Inner signals
    connect(copyType,               SIGNAL(currentIndexChanged(int)),   SLOT(sl_copyFormatSelectionChanged(int)));
    connect(copyButton,             SIGNAL(clicked()),                  SLOT(sl_copyFormatted()));

    // Extern signals
    connect(msa->getMaObject(),
            SIGNAL(si_alignmentChanged(MultipleAlignment, MaModificationInfo)),
            SLOT(sl_alignmentChanged()));

    //out
    connect(this, SIGNAL(si_copyFormatChanged(QString)),
            msa->getUI()->getSequenceArea(), SLOT(sl_changeCopyFormat(QString)));

    connect(this, SIGNAL(si_copyFormatted()),
            msa->getUI()->getSequenceArea(), SLOT(sl_copyFormattedSelection()));

    //in
    connect(msa->getUI()->getSequenceArea(), SIGNAL(si_copyFormattedChanging(bool)),
            SLOT(sl_copyFormatStatusChanged(bool)));
}

void MSAGeneralTab::initializeParameters() {
    // Alignment info
    alignmentAlphabet->setText(msa->getMaObject()->getAlphabet()->getName());
    alignmentLength->setText(QString::number(msa->getAlignmentLen()));
    alignmentHeight->setText(QString::number(msa->getNumSequences()));

    // Consensus type combobox
    consensusModeWidget->init(msa->getMaObject(), msa->getUI()->getConsensusArea());

    //Copy formatted
    DocumentFormatConstraints constr;
    constr.supportedObjectTypes.insert( GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT );
    constr.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);
    constr.addFlagToSupport(DocumentFormatFlag_SupportWriting);
    DocumentFormatRegistry* freg = AppContext::getDocumentFormatRegistry();
    QList<DocumentFormatId> supportedFormats = freg->selectFormats(constr);

    foreach(const DocumentFormatId& fid, supportedFormats) {
        DocumentFormat* f = freg->getFormatById(fid);
        copyType->addItem(QIcon(), f->getFormatName(), f->getFormatId());
    }

    //RTF
    copyType->addItem(QIcon(), "Rich text (HTML)", "RTF");

    QString currentCopyFormattedID = msa->getUI()->getSequenceArea()->getCopyFormattedAlgorithmId();
    copyType->setCurrentIndex(copyType->findData(currentCopyFormattedID));

    //in
    connect(msa->getUI()->getSequenceArea(), SIGNAL(si_copyFormattedChanging(bool)),
            SLOT(sl_copyFormatStatusChanged(bool)));
}

void MSAGeneralTab::updateState() {
    consensusModeWidget->updateState();

    copyButton->setEnabled(!msa->getUI()->getSequenceArea()->getSelection().isEmpty());
}

}   // namespace
