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

#include "SendSelectionDialog.h"

#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

namespace U2 {

#define SETTINGS_ROOT QString("remote_blast_plugin/")
#define SHORT_SETTINGS QString("short")
#define EXPECT_SETTINGS QString("espect_value")
#define HITS_SETTINGS QString("max_hits")
#define HITS_PAIRS_SETTINGS QString("max_hits_pairs")
#define LOW_COMPLEX_SETTINGS QString("low_complexity_filter")
#define REPEATS_SETTINGS QString("human_repeats_filter")
#define LOOKUP_SETTINGS QString("lookup_mask")
#define LOWCASE_SETTINGS QString("lowcase_mask")
#define RETRY_SETTINGS QString("retry")
#define FILTER_SETTINGS QString("filter")

void SendSelectionDialog::setUpSettings() {
    Settings* s = AppContext::getSettings();
    shortSequenceCheckBox->setChecked(s->getValue(SETTINGS_ROOT + SHORT_SETTINGS, false).toBool());
    evalueSpinBox->setValue(s->getValue(SETTINGS_ROOT + EXPECT_SETTINGS, 10).toDouble());
    if (annWgtController != nullptr) {
        quantitySpinBox->setValue(s->getValue(SETTINGS_ROOT + HITS_SETTINGS, 20).toInt());
    } else {
        quantitySpinBox->setValue(s->getValue(SETTINGS_ROOT + HITS_PAIRS_SETTINGS, 200).toInt());
    }
    lowComplexityFilterCheckBox->setChecked(s->getValue(SETTINGS_ROOT + LOW_COMPLEX_SETTINGS, true).toBool());
    repeatsCheckBox->setChecked(s->getValue(SETTINGS_ROOT + REPEATS_SETTINGS, false).toBool());
    lookupMaskCheckBox->setChecked(s->getValue(SETTINGS_ROOT + LOOKUP_SETTINGS, false).toBool());
    lowerCaseCheckBox->setChecked(s->getValue(SETTINGS_ROOT + LOWCASE_SETTINGS, false).toBool());
    retrySpinBox->setValue(s->getValue(SETTINGS_ROOT + RETRY_SETTINGS, 10).toInt());
    evalueRadioButton->setChecked(s->getValue(SETTINGS_ROOT + FILTER_SETTINGS, true).toBool());
    scoreRadioButton->setChecked(!s->getValue(SETTINGS_ROOT + FILTER_SETTINGS, true).toBool());
}

void SendSelectionDialog::saveSettings() {
    Settings* s = AppContext::getSettings();
    s->setValue(SETTINGS_ROOT + SHORT_SETTINGS, shortSequenceCheckBox->isChecked());
    s->setValue(SETTINGS_ROOT + EXPECT_SETTINGS, evalueSpinBox->value());
    if (annWgtController != nullptr) {
        s->setValue(SETTINGS_ROOT + HITS_SETTINGS, quantitySpinBox->value());
    } else {
        s->setValue(SETTINGS_ROOT + HITS_PAIRS_SETTINGS, quantitySpinBox->value());
    }
    s->setValue(SETTINGS_ROOT + LOW_COMPLEX_SETTINGS, lowComplexityFilterCheckBox->isChecked());
    s->setValue(SETTINGS_ROOT + REPEATS_SETTINGS, repeatsCheckBox->isChecked());
    s->setValue(SETTINGS_ROOT + LOOKUP_SETTINGS, lookupMaskCheckBox->isChecked());
    s->setValue(SETTINGS_ROOT + LOWCASE_SETTINGS, lowerCaseCheckBox->isChecked());
    s->setValue(SETTINGS_ROOT + RETRY_SETTINGS, retrySpinBox->value());
    s->setValue(SETTINGS_ROOT + FILTER_SETTINGS, evalueRadioButton->isChecked());
}

void SendSelectionDialog::alignComboBoxes() {
    int count = dataBase->count();
    dataBase->setEditable(true);
    dataBase->lineEdit()->setReadOnly(true);
    dataBase->lineEdit()->setAlignment(Qt::AlignRight);
    for (int i = 0; i < count; i++) {
        dataBase->setItemData(i, Qt::AlignRight, Qt::TextAlignmentRole);
    }

    count = wordSizeComboBox->count();
    wordSizeComboBox->setEditable(true);
    wordSizeComboBox->lineEdit()->setReadOnly(true);
    wordSizeComboBox->lineEdit()->setAlignment(Qt::AlignRight);
    for (int i = 0; i < count; i++) {
        wordSizeComboBox->setItemData(i, Qt::AlignRight, Qt::TextAlignmentRole);
    }

    count = scoresComboBox->count();
    scoresComboBox->setEditable(true);
    scoresComboBox->lineEdit()->setReadOnly(true);
    scoresComboBox->lineEdit()->setAlignment(Qt::AlignRight);
    for (int i = 0; i < count; i++) {
        scoresComboBox->setItemData(i, Qt::AlignRight, Qt::TextAlignmentRole);
    }

    count = costsComboBox->count();
    costsComboBox->setEditable(true);
    costsComboBox->lineEdit()->setReadOnly(true);
    costsComboBox->lineEdit()->setAlignment(Qt::AlignRight);
    for (int i = 0; i < count; i++) {
        costsComboBox->setItemData(i, Qt::AlignRight, Qt::TextAlignmentRole);
    }

    count = matrixComboBox->count();
    matrixComboBox->setEditable(true);
    matrixComboBox->lineEdit()->setReadOnly(true);
    matrixComboBox->lineEdit()->setAlignment(Qt::AlignRight);
    for (int i = 0; i < count; i++) {
        matrixComboBox->setItemData(i, Qt::AlignRight, Qt::TextAlignmentRole);
    }

    count = serviceComboBox->count();
    serviceComboBox->setEditable(true);
    serviceComboBox->lineEdit()->setReadOnly(true);
    serviceComboBox->lineEdit()->setAlignment(Qt::AlignRight);
    for (int i = 0; i < count; i++) {
        serviceComboBox->setItemData(i, Qt::AlignRight, Qt::TextAlignmentRole);
    }
}

SendSelectionDialog::SendSelectionDialog(ADVSequenceObjectContext* _seqCtx, bool _isAminoSeq, const QStringList& selectedPrimerPairNames, QWidget* parent)
    : QDialog(parent), isAminoSeq(_isAminoSeq), seqCtx(_seqCtx) {
    setupUi(this);

    if (selectedPrimerPairNames.isEmpty()) {
        U2SequenceObject* dnaso = seqCtx->getSequenceObject();
        CreateAnnotationModel createAnnotationModel;
        createAnnotationModel.hideAnnotationType = true;
        createAnnotationModel.hideAnnotationName = true;
        createAnnotationModel.hideLocation = true;
        createAnnotationModel.sequenceObjectRef = GObjectReference(dnaso);
        createAnnotationModel.sequenceLen = dnaso->getSequenceLength();
        annWgtController = new CreateAnnotationWidgetController(createAnnotationModel, this);

        optionsTab->setCurrentIndex(0);
        layoutAnnotations->addWidget(annWgtController->getWidget());
    } else {
        auto label = new QLabel(tr("The following primer pairs will be BLASTed:"), this);
        // This listwidget shows grou names of all fit primer pairs
        auto listWidget = new QListWidget(this);
        listWidget->addItems(selectedPrimerPairNames);
        layoutAnnotations->addWidget(label);
        layoutAnnotations->addWidget(listWidget);

        lblQuantity->setText(tr("One primer result limit:"));
        quantitySpinBox->setValue(200);
        QString tooltip = tr("The maximum number of results received for each primer");
        lblQuantity->setToolTip(tooltip);
        quantitySpinBox->setToolTip(tooltip);
    }

    new HelpButton(this, buttonBox, "65930710");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Search"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    matrixComboBox->addItems(ParametersLists::blastp_matrix);

    setupDataBaseList();
    setUpSettings();
    megablastCheckBox->setEnabled(false);
    alignComboBoxes();

    connect(dataBase, SIGNAL(currentIndexChanged(int)), SLOT(sl_scriptSelected(int)));
    QPushButton* okButton = buttonBox->button(QDialogButtonBox::Ok);
    QPushButton* cancelButton = buttonBox->button(QDialogButtonBox::Cancel);

    connect(okButton, SIGNAL(clicked()), SLOT(sl_OK()));
    connect(cancelButton, SIGNAL(clicked()), SLOT(sl_Cancel()));
    connect(megablastCheckBox, SIGNAL(stateChanged(int)), SLOT(sl_megablastChecked(int)));
    connect(serviceComboBox, SIGNAL(currentIndexChanged(int)), SLOT(sl_serviceChanged(int)));
    // I would like to check megablast as default in case of primer pair BLASTing,
    // because it works extremely faster than regular BLAST does.
    megablastCheckBox->setChecked(annWgtController == nullptr);
    sl_scriptSelected(0);
}

void SendSelectionDialog::sl_serviceChanged(int) {
    if (serviceComboBox->currentText() == "phi") {
        lblPhiPattern->show();
        phiPatternEdit->show();
    } else {
        lblPhiPattern->hide();
        phiPatternEdit->hide();
    }
}

void SendSelectionDialog::sl_megablastChecked(int state) {
    if (state == Qt::Checked) {
        wordSizeComboBox->clear();
        wordSizeComboBox->addItems(ParametersLists::megablast_wordSize);
        wordSizeComboBox->setCurrentIndex(3);
    } else {
        wordSizeComboBox->clear();
        wordSizeComboBox->addItems(ParametersLists::blastn_wordSize);
    }
}

QString SendSelectionDialog::getGroupName() const {
    return annWgtController->getModel().groupName;
}

const QString& SendSelectionDialog::getAnnotationDescription() const {
    return annWgtController->getModel().description;
}

const CreateAnnotationModel* SendSelectionDialog::getModel() const {
    return &(annWgtController->getModel());
}

AnnotationTableObject* SendSelectionDialog::getAnnotationObject() const {
    return annWgtController->getModel().getAnnotationObject();
}

QString SendSelectionDialog::getUrl() const {
    return annWgtController->getModel().newDocUrl;
}

void SendSelectionDialog::setupDataBaseList() {
    // cannot analyze amino sequences using nucleotide databases
    if (isAminoSeq) {
        dataBase->removeItem(0);
    }
}

void SendSelectionDialog::sl_scriptSelected(int index) {
    Q_UNUSED(index);
    QString descr = "";
    if (dataBase->currentText() == "cdd") {
        optionsTab->setTabEnabled(1, 0);
        descr.append(CDD_DESCRIPTION);
        shortSequenceCheckBox->setEnabled(false);
        megablastCheckBox->setEnabled(false);
        matrixComboBox->hide();
        lblMatrix->hide();
        dbComboBox->clear();
        dbComboBox->addItems(ParametersLists::cdd_dataBase);
    } else {
        optionsTab->setTabEnabled(1, 1);
        descr.append(BLAST_DESCRIPTION);
        shortSequenceCheckBox->setEnabled(true);
        megablastCheckBox->setEnabled(true);
        if (dataBase->currentText() == "blastn") {
            phiPatternEdit->hide();
            lblPhiPattern->hide();

            megablastCheckBox->setEnabled(true);

            wordSizeComboBox->clear();
            wordSizeComboBox->addItems(ParametersLists::blastn_wordSize);
            wordSizeComboBox->setCurrentIndex(1);

            costsComboBox->clear();
            costsComboBox->addItems(ParametersLists::blastn_gapCost);
            costsComboBox->setCurrentIndex(4);

            scoresComboBox->clear();
            scoresComboBox->addItems(ParametersLists::blastn_scores);
            scoresComboBox->setCurrentIndex(3);
            scoresComboBox->show();
            lblScores->show();

            dbComboBox->clear();
            dbComboBox->addItems(ParametersLists::blastn_dataBase);
            dbComboBox->setCurrentIndex(2);

            matrixComboBox->hide();
            lblMatrix->hide();

            serviceComboBox->hide();
            lblService->hide();
        } else {
            megablastCheckBox->setEnabled(false);

            wordSizeComboBox->clear();
            wordSizeComboBox->addItems(ParametersLists::blastp_wordSize);
            wordSizeComboBox->setCurrentIndex(2);  // The default value is "6".

            costsComboBox->clear();
            costsComboBox->addItems(ParametersLists::blastp_gapCost);
            costsComboBox->setCurrentIndex(4);

            dbComboBox->clear();
            dbComboBox->addItems(ParametersLists::blastp_dataBase);

            matrixComboBox->show();
            matrixComboBox->setCurrentIndex(3);
            lblMatrix->show();

            scoresComboBox->hide();
            lblScores->hide();

            serviceComboBox->show();
            lblService->show();
        }
    }
    teDbDescription->setPlainText(descr);
    alignComboBoxes();
}

void SendSelectionDialog::sl_OK() {
    if (annWgtController != nullptr) {
        QString error = annWgtController->validate();
        CHECK_EXT(error.isEmpty(), QMessageBox::critical(nullptr, tr("Error"), error), );
    }

    cfg.dbChoosen = dataBase->currentText();
    if (cfg.dbChoosen != "cdd") {
        cfg.params = "CMD=Put";
        addParametr(cfg.params, ReqParams::program, cfg.dbChoosen);

        double eValue = evalueSpinBox->value();
        if (shortSequenceCheckBox->isChecked())
            eValue = 1000;
        addParametr(cfg.params, ReqParams::expect, eValue);

        if (false == entrezQueryEdit->text().isEmpty())
            addParametr(cfg.params, ReqParams::entrezQuery, entrezQueryEdit->text());

        int maxHit = quantitySpinBox->value();
        addParametr(cfg.params, ReqParams::hits, maxHit);

        if (megablastCheckBox->isChecked()) {
            addParametr(cfg.params, ReqParams::megablast, "true");
        }

        addParametr(cfg.params, ReqParams::database, dbComboBox->currentText().split(" ").last());

        QString filter = "";
        if (lowComplexityFilterCheckBox->isChecked() && !shortSequenceCheckBox->isChecked()) {
            filter.append("L");
        }
        if (repeatsCheckBox->isChecked()) {
            filter.append("R");
        }
        if (lookupMaskCheckBox->isChecked()) {
            filter.append("m");
        }
        if (!filter.isEmpty()) {
            addParametr(cfg.params, ReqParams::filter, filter);
        }

        addParametr(cfg.params, ReqParams::gapCost, costsComboBox->currentText());
        if (cfg.dbChoosen == "blastn") {
            addParametr(cfg.params, ReqParams::matchScore, scoresComboBox->currentText().split(" ").first());
            addParametr(cfg.params, ReqParams::mismatchScore, scoresComboBox->currentText().split(" ").last());
        }

        if (shortSequenceCheckBox->isChecked()) {
            QString wordSize = wordSizeComboBox->currentText().toInt() > 7 ? "7" : wordSizeComboBox->currentText();
            addParametr(cfg.params, ReqParams::wordSize, wordSize);
        } else {
            addParametr(cfg.params, ReqParams::wordSize, wordSizeComboBox->currentText());
        }

        if (lowerCaseCheckBox->isChecked()) {
            addParametr(cfg.params, ReqParams::lowCaseMask, "yes");
        }

        if (cfg.dbChoosen == "blastp") {
            if (!isAminoSeq) {
                translateToAmino = true;
            }

            addParametr(cfg.params, ReqParams::matrix, matrixComboBox->currentText());
            addParametr(cfg.params, ReqParams::service, serviceComboBox->currentText());
            if (serviceComboBox->currentText() == "phi") {
                addParametr(cfg.params, ReqParams::phiPattern, phiPatternEdit->text());
            }
        }
    }

    else {  // CDD
        cfg.params = "CMD=Put";
        cfg.dbChoosen = "blastp";
        addParametr(cfg.params, ReqParams::program, cfg.dbChoosen);

        addParametr(cfg.params, ReqParams::expect, evalueSpinBox->value());

        addParametr(cfg.params, ReqParams::hits, quantitySpinBox->value());

        QString dbName = dbComboBox->currentText().split(" ").last();
        addParametr(cfg.params, ReqParams::database, dbName.toLower());
        addParametr(cfg.params, ReqParams::service, "rpsblast");
    }

    if (translateToAmino) {
        QObjectScopedPointer<QMessageBox> msg = new QMessageBox(this);
        msg->setText(tr("You chose to search nucleotide sequence in protein database. This sequence will be converted into 6 sequences(3 translations for both strands)."
                        "Therefore this search may take some time. Continue?"));
        msg->setWindowTitle(windowTitle());
        msg->setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msg->setDefaultButton(QMessageBox::Yes);
        msg->exec();
        CHECK(!msg.isNull(), );

        if (msg->result() == QMessageBox::Cancel) {
            return;
        }
    }

    int filterResults = 0;
    filterResults |= (int)accessionCheckBox->isChecked();
    filterResults |= (int)defCheckBox->isChecked() << 1;
    filterResults |= (int)idCheckBox->isChecked() << 2;
    cfg.filterResult = filterResults;
    cfg.useEval = evalueRadioButton->isChecked();
    cfg.retries = retrySpinBox->value();

    saveSettings();
    if (annWgtController != nullptr) {
        bool objectPrepared = annWgtController->prepareAnnotationObject();
        if (!objectPrepared) {
            QMessageBox::warning(this, tr("Error"), tr("Cannot create an annotation object. Please check settings"));
            return;
        }
        seqCtx->getAnnotatedDNAView()->tryAddObject(annWgtController->getModel().getAnnotationObject());
    }
    accept();
}

void SendSelectionDialog::sl_Cancel() {
    reject();
}

}  // namespace U2
