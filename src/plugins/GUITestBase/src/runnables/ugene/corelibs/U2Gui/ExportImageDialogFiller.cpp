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

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QPushButton>
#include <QRadioButton>
#include <QTableWidget>

#include <drivers/GTMouseDriver.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include "ExportImageDialogFiller.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsDialog::ExportImage"

ExportImage::ExportImage(HI::GUITestOpStatus &os, const QString &filePath, const QString &comboValue, int spinValue) :
    Filler(os, "ImageExportForm"),
    filePath(QDir::toNativeSeparators(filePath)),
    comboValue(comboValue),
    spinValue(spinValue)
{

}

ExportImage::ExportImage(HI::GUITestOpStatus &os, CustomScenario *scenario) :
    Filler(os, "ImageExportForm", scenario),
    spinValue(0)
{

}

#define GT_METHOD_NAME "commonScenario"
void ExportImage::commonScenario() {
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QLineEdit* fileEdit = dialog->findChild<QLineEdit*>("fileNameEdit");
    GTLineEdit::setText(os,fileEdit, filePath);

    if(comboValue != ""){
        QComboBox* formatsBox = dialog->findChild<QComboBox*>("formatsBox");
        GTComboBox::setIndexWithText(os, formatsBox,comboValue);
    }

    if(spinValue){
        QSpinBox* spin = dialog->findChild<QSpinBox*>("qualitySpinBox");
        GTSpinBox::setValue(os, spin, spinValue, GTGlobals::UseKeyBoard);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::CircularViewExportImage"
#define GT_METHOD_NAME "commonScenario"
void CircularViewExportImage::commonScenario() {

    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QLineEdit* fileEdit = dialog->findChild<QLineEdit*>("fileNameEdit");
    GTLineEdit::setText(os,fileEdit, filePath);

    if(comboValue != ""){
        QComboBox* formatsBox = dialog->findChild<QComboBox*>("formatsBox");
        GTComboBox::setIndexWithText(os, formatsBox,comboValue);
    }

    if(spinValue){
        QSpinBox* spin = dialog->findChild<QSpinBox*>("qualitySpinBox");
        GTSpinBox::setValue(os, spin, spinValue, GTGlobals::UseKeyBoard);
    }

    if(!exportedSequenceName.isEmpty()) {
        QComboBox* seqsCombo = dialog->findChild<QComboBox*>("Exported_sequence_combo");
        GTComboBox::setIndexWithText(os, seqsCombo, exportedSequenceName);
    }

    QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", dialog));
    GT_CHECK(box != NULL, "buttonBox is NULL");
    QPushButton* button = box->button(QDialogButtonBox::Ok);
    GT_CHECK(button !=NULL, "Ok button is NULL");
    GTWidget::click(os, button);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::ExportMsaImage"
#define GT_METHOD_NAME "commonScenario"
void ExportMsaImage::commonScenario() {
    GT_CHECK( (exportWholeAlignment && exportCurrentSelection) != true, "Wrong filler parameters");

    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    if (!exportWholeAlignment) {
        if (!exportCurrentSelection) {
            GTUtilsDialog::waitForDialog(os, new SelectSubalignmentFiller(os, region));
        }
        QComboBox* exportType = dialog->findChild<QComboBox*>("comboBox");
        GTComboBox::setIndexWithText(os, exportType, "Custom region");
    }

    QCheckBox* namesCB = dialog->findChild<QCheckBox*>("exportSeqNames");
    GTCheckBox::setChecked(os, namesCB, settings.includeNames);

    QCheckBox* consensusCB = dialog->findChild<QCheckBox*>("exportConsensus");
    GTCheckBox::setChecked(os, consensusCB, settings.includeConsensus);

    QCheckBox* rulerCB= dialog->findChild<QCheckBox*>("exportRuler");
    GTCheckBox::setChecked(os, rulerCB, settings.includeRuler);

    QLineEdit* fileEdit = dialog->findChild<QLineEdit*>("fileNameEdit");
    GTLineEdit::setText(os,fileEdit, filePath);

    if(comboValue != ""){
        QComboBox* formatsBox = dialog->findChild<QComboBox*>("formatsBox");
        GTComboBox::setIndexWithText(os, formatsBox,comboValue);
    }

    if(spinValue){
        QSpinBox* spin = dialog->findChild<QSpinBox*>("qualitySpinBox");
        GTSpinBox::setValue(os, spin, spinValue, GTGlobals::UseKeyBoard);
    }

    QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", dialog));
    GT_CHECK(box != NULL, "buttonBox is NULL");
    QPushButton* button = box->button(QDialogButtonBox::Ok);
    GT_CHECK(button !=NULL, "ok button is NULL");
    GTWidget::click(os, button);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME


#define GT_CLASS_NAME "GTUtilsDialog::ExportSequenceImage"
#define GT_METHOD_NAME "commonScenario"
void ExportSequenceImage::commonScenario() {
    GTGlobals::sleep(500);
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QString radioButtonName;
    switch (settings.type) {
    case CurrentView:
        radioButtonName = "currentViewButton";
        break;
    case ZoomedView:
        radioButtonName = "zoomButton";
        break;
    case DetailsView:
        radioButtonName = "detailsButton";
        break;
    }

    QRadioButton* radioButton = dialog->findChild<QRadioButton*>(radioButtonName);
    GTRadioButton::click(os, radioButton);

    if (settings.type != CurrentView) {
        // set region
        QLineEdit* start = dialog->findChild<QLineEdit*>("start_edit_line");
        GTLineEdit::setText(os, start, QString::number(settings.region.startPos));

        QLineEdit* end = dialog->findChild<QLineEdit*>("end_edit_line");
        GTLineEdit::setText(os, end, QString::number(settings.region.endPos()));
    }

    QLineEdit* fileEdit = dialog->findChild<QLineEdit*>("fileNameEdit");
    GTLineEdit::setText(os, fileEdit, filePath);

    if(comboValue != ""){
        QComboBox* formatsBox = dialog->findChild<QComboBox*>("formatsBox");
        GTComboBox::setIndexWithText(os, formatsBox,comboValue);
    }

    if(spinValue){
        QSpinBox* spin = dialog->findChild<QSpinBox*>("qualitySpinBox");
        GTSpinBox::setValue(os, spin, spinValue, GTGlobals::UseKeyBoard);
    }

    QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", dialog));
    GT_CHECK(box != NULL, "buttonBox is NULL");
    QPushButton* button = box->button(QDialogButtonBox::Ok);
    GT_CHECK(button !=NULL, "ok button is NULL");
    GTWidget::click(os, button);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::SelectSubalignmentFiller"
#define GT_METHOD_NAME "commonScenario"
void SelectSubalignmentFiller::commonScenario() {
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QSpinBox* startLineEdit = dialog->findChild<QSpinBox*>("startLineEdit");
    GT_CHECK(startLineEdit != NULL, "startLineEdit is NULL");
    GTSpinBox::setValue(os, startLineEdit, msaRegion.region.startPos, GTGlobals::UseKeyBoard);

    QSpinBox* endLineEdit = dialog->findChild<QSpinBox*>("endLineEdit");
    GT_CHECK(endLineEdit != NULL, "endPoxBox is NULL");
    GTSpinBox::setValue(os, endLineEdit, msaRegion.region.endPos(), GTGlobals::UseKeyBoard);

    QWidget *noneButton = dialog->findChild<QWidget*>("noneButton");
    GT_CHECK(noneButton!=NULL, "noneButton is NULL");
    GTWidget::click(os, noneButton);

    QTableWidget *table = dialog->findChild<QTableWidget*>("sequencesTableWidget");
    GT_CHECK(table!=NULL, "tableWidget is NULL");

    QPoint p = table->geometry().topRight();
    p.setX(p.x() - 2);
    p.setY(p.y() + 2);
    p = dialog->mapToGlobal(p);

    GTMouseDriver::moveTo(p);
    GTMouseDriver::click();
    for(int i = 0; i < table->rowCount();i++){
        foreach (QString s, msaRegion.sequences){
            QCheckBox *box = qobject_cast<QCheckBox*>(table->cellWidget(i,0));
            if (s == box->text()){
                GT_CHECK(box->isEnabled(), QString("%1 box is disabled").arg(box->text()));
                box->setChecked(true);
            }
        }
    }

    QDialogButtonBox* box = dialog->findChild<QDialogButtonBox*>("buttonBox");
    GT_CHECK(box != NULL, "buttonBox is NULL");
    QPushButton* ok = box->button(QDialogButtonBox::Ok);
    GT_CHECK(ok !=NULL, "ok button is NULL");
    GTWidget::click(os, ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

ImageExportFormFiller::ImageExportFormFiller(HI::GUITestOpStatus &os, const Parameters &parameters)
: Filler(os, "ImageExportForm"), parameters(parameters)
{

}

#define GT_CLASS_NAME "GTUtilsDialog::ImageExportFormFiller"
#define GT_METHOD_NAME "commonScenario"

void ImageExportFormFiller::commonScenario() {
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QLineEdit* fileNameEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "fileNameEdit", dialog));
    GT_CHECK(fileNameEdit, "fileNameEdit is NULL");
    GTLineEdit::setText(os, fileNameEdit, QDir::toNativeSeparators(parameters.fileName));

    QComboBox* formatsBox = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "formatsBox", dialog));
    GT_CHECK(formatsBox, "formatsBox is NULL");
    GTComboBox::setIndexWithText(os, formatsBox, parameters.format);

    QDialogButtonBox* box = dialog->findChild<QDialogButtonBox*>("buttonBox");
    GT_CHECK(box != NULL, "buttonBox is NULL");
    QPushButton* ok = box->button(QDialogButtonBox::Ok);
    GT_CHECK(ok !=NULL, "ok button is NULL");
    GTWidget::click(os, ok);
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}
