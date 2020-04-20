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

#include <U2Core/DocumentUtils.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Gui/U2FileDialog.h>

#include "DotPlotFilesDialog.h"

namespace U2 {

DotPlotFilesDialog::DotPlotFilesDialog(QWidget *parent)
: QDialog(parent)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24748821");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Next"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    connect(openFirstButton, SIGNAL(clicked()), SLOT(sl_openFirstFile()));
    connect(openSecondButton, SIGNAL(clicked()), SLOT(sl_openSecondFile()));
    connect(oneSequenceCheckBox, SIGNAL(clicked()), SLOT(sl_oneSequence()));

    connect(mergeFirstCheckBox, SIGNAL(clicked()), SLOT(sl_mergeFirst()));
    connect(mergeSecondCheckBox, SIGNAL(clicked()), SLOT(sl_mergeSecond()));

    filter = DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, true).append("\n").append(
        DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::SEQUENCE, false));

}

void DotPlotFilesDialog::sl_oneSequence() {

    secondFileEdit->setDisabled(oneSequenceCheckBox->isChecked());
    openSecondButton->setDisabled(oneSequenceCheckBox->isChecked());

    mergeSecondCheckBox->setDisabled(oneSequenceCheckBox->isChecked());
    gapSecond->setDisabled(oneSequenceCheckBox->isChecked() || !mergeSecondCheckBox->isChecked());
}

void DotPlotFilesDialog::sl_mergeFirst() {
    gapFirst->setDisabled(!mergeFirstCheckBox->isChecked());
}

void DotPlotFilesDialog::sl_mergeSecond() {
    gapSecond->setDisabled(!mergeSecondCheckBox->isChecked());
}

// open first file button clicked
void DotPlotFilesDialog::sl_openFirstFile() {

    LastUsedDirHelper lod("DotPlot first file");
    lod.url = U2FileDialog::getOpenFileName(NULL, tr("Open first file"), lod.dir, filter);

    SAFE_POINT(firstFileEdit, "firstFileEdit is NULL", );
    if (!lod.url.isEmpty()) {
        firstFileEdit->setText(lod.url);
        FormatDetectionConfig conf;
        conf.useImporters = true;
        conf.bestMatchesOnly = true;
        QList<FormatDetectionResult> results = DocumentUtils::detectFormat(lod.url, conf);
        if (results.isEmpty()){
            firstFileEdit->setText("");
            lod.url = "";
            return;
        }
        FormatDetectionResult format = results.at(0); //get moslty matched format
        bool multySeq = format.rawDataCheckResult.properties.value(RawDataCheckResult_MultipleSequences).toBool();
        if(multySeq){
            mergeFirstCheckBox->setChecked(true);
            sl_mergeFirst();
        }
    }
}

// open second file button clicked
void DotPlotFilesDialog::sl_openSecondFile() {

    LastUsedDirHelper lod("DotPlot second file");
    if (lod.dir.isEmpty()) {
        LastUsedDirHelper lodFirst("DotPlot first file");

        lod.dir = lodFirst.dir;
    }
    lod.url = U2FileDialog::getOpenFileName(NULL, tr("Open second file"), lod.dir, filter);

    SAFE_POINT(secondFileEdit, "secondFileEdit is NULL", );
    if (!lod.url.isEmpty()) {
        secondFileEdit->setText(lod.url);
        FormatDetectionConfig conf;
        conf.useImporters = true;
        conf.bestMatchesOnly = true;
        QList<FormatDetectionResult> results = DocumentUtils::detectFormat(lod.url, conf);
        if (results.isEmpty()){
            secondFileEdit->setText("");
            lod.url = "";
            return;
        }
        FormatDetectionResult format = results.at(0); //get moslty matched format
        bool multySeq = format.rawDataCheckResult.properties.value(RawDataCheckResult_MultipleSequences).toBool();
        if(multySeq){
            mergeSecondCheckBox->setChecked(true);
            sl_mergeSecond();
        }
    }
}

// ok button clicked
void DotPlotFilesDialog::accept() {

    SAFE_POINT(firstFileEdit, "firstFileEdit is NULL", );
    SAFE_POINT(secondFileEdit, "secondFileEdit is NULL", );

    firstFileName = firstFileEdit->text();
    secondFileName = secondFileEdit->text();

    if (oneSequenceCheckBox->isChecked()) {
        secondFileName = firstFileName;
    }

    if (firstFileName.isEmpty() || secondFileName.isEmpty()) {
        QString error = oneSequenceCheckBox->isChecked() ?
                    tr("Select a file with a sequence to build dotplot!")
                  : firstFileName.isEmpty()?
                        tr("Select first file with a sequence to build dotplot!")
                      : tr("Input the second sequence or check the 'Compare sequence against itself' option.");
        QObjectScopedPointer<QMessageBox> mb = new QMessageBox(QMessageBox::Critical, tr("Select files"), error);
        mb->exec();
        return;
    }

    FormatDetectionConfig conf;
    QList<FormatDetectionResult> results = DocumentUtils::detectFormat(firstFileName, conf);
    if (results.isEmpty()){
        QObjectScopedPointer<QMessageBox> mb = new QMessageBox(QMessageBox::Critical, tr("Select files"), tr("Unable to detect file format %1.").arg(firstFileName));
        mb->exec();
        return;
    }

    if (firstFileName != secondFileName) {
        results = DocumentUtils::detectFormat(secondFileName, conf);
        if (results.isEmpty()){
            QObjectScopedPointer<QMessageBox> mb = new QMessageBox(QMessageBox::Critical, tr("Select files"), tr("Unable to detect file format %1.").arg(secondFileName));
            mb->exec();
            return;
        }
    }

    QDialog::accept();
}

int DotPlotFilesDialog::getFirstGap() const {

    if (mergeFirstCheckBox->isChecked()) {
        return gapFirst->value();
    }
    else {
        return -1;
    }
}

int DotPlotFilesDialog::getSecondGap() const {

    if (mergeSecondCheckBox->isChecked()) {
        return gapSecond->value();
    }
    else {
        return -1;
    }
}

}//namespace
