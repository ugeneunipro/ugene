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

#include "CreateAnnotationWidgetFiller.h"
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

#include <QDialogButtonBox>
#include <QDir>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::CreateAnnotationDialogFiller"

CreateAnnotationWidgetFiller::CreateAnnotationWidgetFiller(
    bool newTableRB,
    const QString& groupName,
    const QString& annotationName,
    const QString& location,
    const QString& saveTo,
    const QString& description)
    : Filler("CreateAnnotationDialog"),
      groupName(groupName),
      annotationName(annotationName),
      location(location),
      newTableRB(newTableRB),
      saveTo(saveTo),
      description(description) {
}

CreateAnnotationWidgetFiller::CreateAnnotationWidgetFiller(CustomScenario* scenario)
    : Filler("CreateAnnotationDialog", scenario),
      newTableRB(false) {
}

#define GT_METHOD_NAME "commonScenario"

void CreateAnnotationWidgetFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    if (newTableRB) {
        GTRadioButton::click(GTWidget::findRadioButton("rbCreateNewTable", dialog));
        if (!saveTo.isEmpty()) {
            QDir().mkpath(QFileInfo(saveTo).dir().absolutePath());
            GTLineEdit::setText("leNewTablePath", saveTo, dialog);
        }
    } else if (!saveTo.isEmpty()) {
        GTRadioButton::click(GTWidget::findRadioButton("rbExistingTable", dialog));
        GTComboBox::selectItemByText(GTWidget::findComboBox("cbExistingTable", dialog), saveTo);
    }

    GTLineEdit::setText("leGroupName", groupName, dialog);
    GTLineEdit::setText("leAnnotationName", annotationName, dialog);
    if (!location.isEmpty()) {
        GTRadioButton::click(GTWidget::findRadioButton("rbGenbankFormat", dialog));
        GTLineEdit::setText("leLocation", location, dialog);
    }
    if (!description.isEmpty()) {
        GTLineEdit::setText("leDescription", description, dialog);
    }

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
