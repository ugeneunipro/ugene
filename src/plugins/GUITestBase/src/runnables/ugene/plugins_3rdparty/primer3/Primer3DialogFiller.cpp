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

#include <drivers/GTKeyboardDriver.h>
#include "Primer3DialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTWidget.h>

#include <base_dialogs/MessageBoxFiller.h>

#include <QApplication>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QCheckBox>

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsDialog::KalignDialogFiller"
Primer3DialogFiller::Primer3DialogFiller(HI::GUITestOpStatus &os, const Primer3Settings &settings)
    : Filler(os, "Primer3Dialog"),
      settings(settings)
{}

#define GT_METHOD_NAME "run"
void Primer3DialogFiller::commonScenario() {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");

    if(settings.resultsCount != -1){
        QSpinBox* resultsCountSpinBox = dialog->findChild<QSpinBox*>("edit_PRIMER_NUM_RETURN");
        GTSpinBox::setValue(os, resultsCountSpinBox, settings.resultsCount);
    }

    QTabWidget *tabWidget = dialog->findChild<QTabWidget*>("tabWidget");
    if (!settings.primersGroupName.isEmpty() || !settings.primersName.isEmpty()) {
        GTTabWidget::setCurrentIndex(os, tabWidget, 6);

        if (!settings.primersGroupName.isEmpty()) {
            QLineEdit *groupEdit = dialog->findChild<QLineEdit*>("leGroupName");
            GT_CHECK(groupEdit != NULL, "groupNameEdit is NULL");
            GTLineEdit::setText(os, groupEdit, settings.primersGroupName);
        }

        if (!settings.primersName.isEmpty()) {
            QLineEdit *primerEdit = dialog->findChild<QLineEdit*>("leAnnotationName");
            GT_CHECK(primerEdit != NULL, "annotationNameEdit is NULL");
            GTLineEdit::setText(os, primerEdit, settings.primersName);
        }
    }

    if (settings.start != -1) {
        QLineEdit* start = dialog->findChild<QLineEdit*>("start_edit_line");
        GT_CHECK(start != NULL, "start_edit_line is NULL");
        GTLineEdit::setText(os, start, QString::number(settings.start));
    }

    if (settings.end != -1) {
        QLineEdit* end = dialog->findChild<QLineEdit*>("end_edit_line");
        GT_CHECK(end != NULL, "start_edit_line is NULL");
        GTLineEdit::setText(os, end, QString::number(settings.end));
    }
    QCheckBox* leftCheckbox = dialog->findChild<QCheckBox*>("checkbox_PICK_LEFT");
    GT_CHECK(leftCheckbox != NULL, "checkbox_PICK_LEFT is NULL");
    GTCheckBox::setChecked(os, leftCheckbox, settings.pickLeft);

    QCheckBox* rightCheckbox = dialog->findChild<QCheckBox*>("checkbox_PICK_RIGHT");
    GT_CHECK(rightCheckbox != NULL, "checkbox_PICK_RIGHT is NULL");
    GTCheckBox::setChecked(os, rightCheckbox, settings.pickRight);

    if (settings.rtPcrDesign) {
        GTTabWidget::setCurrentIndex(os, tabWidget, 4);

        QGroupBox* groupBox = dialog->findChild<QGroupBox*>("spanIntronExonBox");
        GT_CHECK(groupBox != NULL, "spanIntronExonBox is NULL");
        GTGroupBox::setChecked(os, groupBox);
    }

    if(!settings.shortRegion) {
        QPushButton* button = dialog->findChild<QPushButton*>("pickPrimersButton");
        GT_CHECK(button !=NULL, "PickPrimers button is NULL");
        GTWidget::click(os, button);
    } else {
        GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
        QPushButton* button = dialog->findChild<QPushButton*>("pickPrimersButton");
        GT_CHECK(button != NULL, "PickPrimers button is NULL");
        GTWidget::click(os, button);
#ifdef Q_OS_MAC
        dialog->close();
#else
        GTKeyboardDriver::keyClick( Qt::Key_Escape);
#endif
    }
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

} // namespace
