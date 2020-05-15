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

#include "ExportProjectDialogFiller.h"
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>

#include <QAbstractButton>
#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsDialog::ExportProjectDialogChecker"
#define GT_METHOD_NAME "commonScenario"
void ExportProjectDialogChecker::commonScenario() {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QLineEdit *projectFileLineEdit = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "projectFilePathEdit", dialog));
    GT_CHECK(projectFileLineEdit != NULL, "LineEdit is NULL");
    GT_CHECK(projectFileLineEdit->text() == projectName, "Project name is not " + projectName);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::ExportProjectDialogFiller"
#define GT_METHOD_NAME "commonScenario"
void ExportProjectDialogFiller::commonScenario() {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");
    
    QLineEdit *projectFileLineEdit = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "projectFilePathEdit", dialog));
    GT_CHECK(projectFileLineEdit != NULL, "LineEdit is NULL");
    if (!projectName.isEmpty()) {
        GTLineEdit::setText(os, projectFileLineEdit, projectName);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}    // namespace U2
