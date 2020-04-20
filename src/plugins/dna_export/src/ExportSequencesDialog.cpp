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

#include <QPushButton>
#include <QMessageBox>
#include <QTreeWidget>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/L10n.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/GUrlUtils.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/SaveDocumentController.h>

#include "ExportSequencesDialog.h"

#define SETTINGS_ROOT QString("dna_export/")

namespace U2 {

ExportSequencesDialog::ExportSequencesDialog( bool m, bool allowComplement, bool allowTranslation,
    bool allowBackTranslation, const QString& defaultFileName, const QString &sourceFileBaseName,
    const DocumentFormatId& defaultFormatId, QWidget* p )
    : QDialog(p),
      sequenceName(sourceFileBaseName),
      saveController(NULL),
      defaultFileName(defaultFileName) {
    setupUi(this);
    new HelpButton(this, buttonBox, "24748719");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    initSaveController(defaultFormatId);

    multiMode = m;
    strand = TriState_Yes;
    translate = false;
    translateAllFrames = false;
    addToProject = false;

    sequenceNameEdit->setText( sequenceName );
    withAnnotationsBox->setEnabled(false);

    if (!allowComplement) {
        directStrandButton->setEnabled(false);
        complementStrandButton->setEnabled(false);
        bothStrandsButton->setEnabled(false);

        directStrandButton->setHidden(true);
        complementStrandButton->setHidden(true);
        bothStrandsButton->setHidden(true);

    }
    if (!allowTranslation) {
        translateButton->setEnabled(false);
        translateButton->setHidden(true);
        allTFramesButton->setHidden(true);
    }

    if (!allowTranslation && !allowComplement) {
         translationBox->setHidden(true);
    }

    if (!multiMode) {
        saveModeBox->setEnabled(false);
    }

    if (!allowBackTranslation) {
        backTranslationBox->setHidden(true);
    }

    if (allowTranslation) {
        const DNAAlphabet* al = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
        DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
        QList<DNATranslation*> aminoTs = tr->lookupTranslation(al, DNATranslationType_NUCL_2_AMINO);
        if (!aminoTs.empty()) {
            foreach(DNATranslation* t, aminoTs) {
                translationTableCombo->addItem(t->getTranslationName());
                tableID.append(t->getTranslationId());
            }
        }
        translationTable = tableID[translationTableCombo->currentIndex()];
    }

    if (allowBackTranslation) {
        const DNAAlphabet* al = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::AMINO_DEFAULT());
        DNATranslationRegistry* treg = AppContext::getDNATranslationRegistry();
        QList<DNATranslation*> nucleicTs = treg->lookupTranslation(al, DNATranslationType_AMINO_2_NUCL);
        QTreeWidget *tree = new QTreeWidget();
        tree->setHeaderHidden(true);
        organismCombo->setModel(tree->model());
        organismCombo->setView (tree);

        if (!nucleicTs.empty()) {
            tree->setSortingEnabled(false);
            foreach(DNATranslation* t, nucleicTs) {
                QStringList current = t->getTranslationId().split("/");
                QString type = current[1];
                QString text = t->getTranslationName();
                int i, n = tree->topLevelItemCount();
                for (i = 0; i < n; i++) {
                    QTreeWidgetItem* gi = tree->topLevelItem(i);
                    if (gi->text(0) == type) {
                        QTreeWidgetItem* curr = new QTreeWidgetItem(gi);
                        curr->setText(0, text);
                        curr->setText(1, t->getTranslationId());
                        gi->addChild(curr);
                        break;
                    }
                }
                if (i == n) {
                    QTreeWidgetItem* gi = new QTreeWidgetItem(tree);
                    gi->setFlags(gi->flags() & ~Qt::ItemIsSelectable);
                    gi->setText(0, type);
                    tree->addTopLevelItem(gi);
                    QTreeWidgetItem* curr = new QTreeWidgetItem(gi);
                    curr->setText(0, text);
                    curr->setText(1, t->getTranslationId());
                    gi->addChild(curr);
                    gi->setExpanded(true);
                }
            }
            tree->sortItems(0, Qt::AscendingOrder);
            QTreeWidgetItem* def = new QTreeWidgetItem(tree);
            def->setText(0, tr("Select organism"));
            def->setFlags(def->flags() & ~Qt::ItemIsSelectable);
            tree->insertTopLevelItem(0, def);
            tree->setCurrentItem(def);
            organismCombo->setCurrentIndex(organismCombo->count()-1);
        }
    }

    formatId = defaultFormatId;
    QPushButton* exportButton = buttonBox->button(QDialogButtonBox::Ok);
    connect(exportButton, SIGNAL(clicked()), SLOT(sl_exportClicked()));
    connect(translateButton, SIGNAL(clicked()), SLOT(sl_translationTableEnabler()));
    connect(translationTableButton, SIGNAL(clicked()), SLOT(sl_translationTableEnabler()));

    int height = layout()->minimumSize().height();
    setMaximumHeight(height);
}

void ExportSequencesDialog::sl_formatChanged(const QString &newFormatId) {
    DocumentFormatRegistry *dfr = AppContext::getDocumentFormatRegistry();
    SAFE_POINT(NULL != dfr, "Invalid document format registry", );
    if (dfr->getFormatById(newFormatId)->getSupportedObjectTypes().contains(GObjectTypes::ANNOTATION_TABLE)) {
        withAnnotationsBox->setEnabled(true);
        withAnnotationsBox->setChecked(true);
    } else {
        withAnnotationsBox->setEnabled(false);
        withAnnotationsBox->setChecked(false);
    }
}

void ExportSequencesDialog::initSaveController(const DocumentFormatId &defaultFormatId) {
    SaveDocumentControllerConfig config;
    config.defaultFileName = defaultFileName;
    config.defaultFormatId = defaultFormatId;
    config.fileDialogButton = fileButton;
    config.fileNameEdit = fileNameEdit;
    config.formatCombo = formatCombo;
    config.parentWidget = this;
    config.saveTitle = tr("Export Sequences");

    DocumentFormatConstraints formatConstraints;
    formatConstraints.supportedObjectTypes << GObjectTypes::SEQUENCE;
    formatConstraints.addFlagToExclude(DocumentFormatFlag_SingleObjectFormat);
    formatConstraints.addFlagToSupport(DocumentFormatFlag_SupportWriting);

    saveController = new SaveDocumentController(config, formatConstraints, this);

    connect(saveController, SIGNAL(si_formatChanged(const QString &)), SLOT(sl_formatChanged(const QString &)));
}

void ExportSequencesDialog::updateModel() {
    strand = directStrandButton->isChecked() ? TriState_Yes : complementStrandButton->isChecked() ? TriState_No : TriState_Unknown;
    translate = translateButton->isChecked();
    translateAllFrames = allTFramesButton->isVisible() && allTFramesButton->isChecked();
    addToProject = addToProjectBox->isChecked();

    merge = mergeButton->isChecked();
    mergeGap = merge ? mergeSpinBox->value() : 0;

    file = saveController->getSaveFileName();
    QFileInfo fi(file);
    if( fi.isRelative() ) {
        // save it in root sequence folder
        file = QFileInfo(defaultFileName).absoluteDir().absolutePath() + "/" + file;
    }
    sequenceName = ( customSeqNameBox->isChecked( ) ) ? sequenceNameEdit->text( ) : QString( );

    formatId = saveController->getFormatIdToSave();
    useSpecificTable = translationTableButton->isChecked();
    if (translate) {
        translationTable = tableID[translationTableCombo->currentIndex()];
    }
    backTranslate = backTranslateButton->isChecked();
    if (backTranslate) {
        QTreeWidget *tree = (QTreeWidget *)organismCombo->view();
        QTreeWidgetItem *current = tree->currentItem();
        translationTable = current->text(1);
    }
    mostProbable = mostFrequentlyButton->isChecked();

    withAnnotations = withAnnotationsBox->isChecked() && withAnnotationsBox->isEnabled();
}

void ExportSequencesDialog::sl_exportClicked() {
    QString fileName = saveController->getSaveFileName();
    if (fileName.isEmpty()) {
        QMessageBox::warning(this, L10N::warningTitle(), tr("File is empty"));
        fileNameEdit->setFocus();
        return;
    }
    QFileInfo fi(fileName);
    if (fi.fileName().length() > MAX_OS_FILE_NAME_LENGTH) {
        QMessageBox::warning(this, L10N::warningTitle(), tr("File name is too long!"));
        fileNameEdit->setFocus();
        return;
    }
    if (backTranslateButton->isChecked() && organismCombo->currentText().isEmpty()){
        QMessageBox::warning(this, L10N::warningTitle(), tr("Organism for back translation not specified"));
        organismCombo->setFocus();
        return;
    }

    updateModel();
    accept();
}

void ExportSequencesDialog::sl_translationTableEnabler() {
    if (translateButton->isChecked() && translationTableButton->isChecked()) {
        emit translationTableCombo->setEnabled(true);
    } else {
        emit translationTableCombo->setEnabled(false);
    }
}

void ExportSequencesDialog::disableAllFramesOption(bool v) {
    allTFramesButton->setHidden(v);
}

void ExportSequencesDialog::disableStrandOption(bool v) {
    directStrandButton->setDisabled(v);
    complementStrandButton->setDisabled(v);
    bothStrandsButton->setDisabled(v);
}

void ExportSequencesDialog::disableAnnotationsOption(bool v) {
    withAnnotationsBox->setDisabled(v);
}


}//namespace
