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

#include <QMessageBox>
#include <QPushButton>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/L10n.h>
#include <U2Core/Settings.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/SaveDocumentController.h>

#include "ExportMSA2MSADialog.h"

namespace U2 {

ExportMSA2MSADialog::ExportMSA2MSADialog(const QString& defaultFileName, const DocumentFormatId& defaultFormatId, bool wholeAlignmentOnly, QWidget* p)
    : QDialog(p),
      saveController(NULL) {
    setupUi(this);
    new HelpButton(this, buttonBox, "24748722");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    addToProjectFlag = true;

    initSaveController(defaultFileName, defaultFormatId);

    const DNAAlphabet* al = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
    QList<DNATranslation*> aminoTs = tr->lookupTranslation(al, DNATranslationType_NUCL_2_AMINO);
    assert(!aminoTs.empty());
    foreach(DNATranslation* t, aminoTs) {
        translationCombo->addItem(t->getTranslationName());
        tableID.append(t->getTranslationId());
    }
    translationTable = tableID[translationCombo->currentIndex()];

    QPushButton* exportButton = buttonBox->button(QDialogButtonBox::Ok);
    connect(exportButton, SIGNAL(clicked()), SLOT(sl_exportClicked()));

    rangeGroupBox->setDisabled(wholeAlignmentOnly);

    int height = layout()->minimumSize().height();
    setMaximumHeight(height);
}

void ExportMSA2MSADialog::updateModel(){
    formatId = saveController->getFormatIdToSave();
    file = saveController->getSaveFileName();
    translationTable = tableID[translationCombo->currentIndex()];
    addToProjectFlag = addDocumentButton->isChecked();
    exportWholeAlignment = wholeRangeButton->isChecked();
}


void ExportMSA2MSADialog::sl_exportClicked() {
    if (saveController->getSaveFileName().isEmpty()) {
        QMessageBox::warning(this, L10N::warningTitle(), tr("File is empty"));
        fileNameEdit->setFocus();
        return;
    }
    updateModel();
    accept();
}

void ExportMSA2MSADialog::initSaveController(const QString& defaultFileName, const DocumentFormatId& defaultFormatId) {
    SaveDocumentControllerConfig config;
    config.defaultFileName = defaultFileName;
    config.defaultFormatId = defaultFormatId;
    config.fileDialogButton = fileButton;
    config.fileNameEdit = fileNameEdit;
    config.formatCombo = formatCombo;
    config.parentWidget = this;
    config.saveTitle = tr("Export alignment");

    DocumentFormatConstraints formatConstraints;
    formatConstraints.supportedObjectTypes << GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    formatConstraints.addFlagToSupport(DocumentFormatFlag_SupportWriting);

    saveController = new SaveDocumentController(config, formatConstraints, this);
}

}   // namespace
