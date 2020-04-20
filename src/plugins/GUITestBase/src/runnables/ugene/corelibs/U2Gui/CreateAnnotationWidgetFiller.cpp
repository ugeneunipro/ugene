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

#include <QAbstractButton>
#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QPushButton>
#include <QRadioButton>

#include "CreateAnnotationWidgetFiller.h"
#include <primitives/GTComboBox.h>
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTWidget.h>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::CreateAnnotationDialogFiller"

CreateAnnotationWidgetFiller::CreateAnnotationWidgetFiller(HI::GUITestOpStatus &os,
                                                           bool newTableRB,
                                                           const QString &groupName,
                                                           const QString &annotationName,
                                                           const QString &location,
                                                           const QString &saveTo,
                                                           const QString &description) :
    Filler(os, "CreateAnnotationDialog"),
    groupName(groupName),
    annotationName(annotationName),
    location(location),
    newTableRB(newTableRB),
    saveTo(saveTo),
    description(description)
{

}

CreateAnnotationWidgetFiller::CreateAnnotationWidgetFiller(HI::GUITestOpStatus &os, CustomScenario *scenario) :
    Filler(os, "CreateAnnotationDialog", scenario),
    newTableRB(false)
{

}

#define GT_METHOD_NAME "commonScenario"

void CreateAnnotationWidgetFiller::commonScenario() {
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    if (newTableRB) {
        GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton *>(os, "rbCreateNewTable", dialog));

        if (!saveTo.isEmpty()) {
            QDir().mkpath(QFileInfo(saveTo).dir().absolutePath());
            GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leNewTablePath", dialog), saveTo);
        }
    } else {
        GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton *>(os, "rbExistingTable", dialog));

        if (!saveTo.isEmpty()) {
            GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "cbExistingTable", dialog), saveTo);
        }
    }

    GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leGroupName", dialog), groupName);
    GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leAnnotationName", dialog), annotationName);
    GTRadioButton::click(os, GTWidget::findExactWidget<QRadioButton *>(os, "rbGenbankFormat", dialog));
    GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leLocation", dialog), location);
    if (!description.isEmpty()) {
        GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leDescription", dialog), description);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}
