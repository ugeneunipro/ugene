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

#include "EditAnnotationDialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QPushButton>
#include <QToolButton>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::EditAnnotationFiller"
#define GT_METHOD_NAME "run"
void EditAnnotationFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    GTLineEdit::setText(os, "leAnnotationName", annotationName, dialog);

    auto gbFormatLocation = GTWidget::findRadioButton(os, "rbGenbankFormat", dialog);
    GTRadioButton::click(os, gbFormatLocation);

    GTLineEdit::setText(os, "leLocation", location, dialog);

    if (complementStrand != false) {
        auto complementStrand = GTWidget::findToolButton(os, "tbDoComplement", dialog);
        GTWidget::click(os, complementStrand);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::EditAnnotationChecker"

#define GT_METHOD_NAME "commonScenario"
void EditAnnotationChecker::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    if (!annotationName.isEmpty()) {
        auto lineEdit = GTWidget::findLineEdit(os, "leAnnotationName", dialog);
        QString text = lineEdit->text();

        GT_CHECK(text == annotationName, QString("The name is incorrect: got [%1], expected [%2]").arg(text).arg(annotationName));
    }

    if (!location.isEmpty()) {
        auto gbFormatLocation = GTWidget::findRadioButton(os, "rbGenbankFormat", dialog);
        GTRadioButton::click(os, gbFormatLocation);

        auto lineEdit1 = GTWidget::findLineEdit(os, "leLocation", dialog);

        QString text = lineEdit1->text();

        GT_CHECK(text == location, QString("The location is incorrect: got [%1], expected [%2]").arg(text).arg(location));
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
