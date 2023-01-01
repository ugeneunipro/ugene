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

#include "ExportProjectDialogFiller.h"
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>

#include <QAbstractButton>
#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QtCore/QFileInfo>

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsDialog::ExportProjectDialogChecker"
#define GT_METHOD_NAME "commonScenario"
void ExportProjectDialogChecker::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    QString fullPath = GTLineEdit::getText(os, "projectFilePathEdit", dialog);
    QString actualName = projectName.contains('/') ? fullPath : QFileInfo(fullPath).fileName();
    GT_CHECK(actualName == projectName, QString("Expected project name: %1, got: %2").arg(projectName, actualName));

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::ExportProjectDialogFiller"
#define GT_METHOD_NAME "commonScenario"
void ExportProjectDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    if (!projectName.isEmpty()) {
        GTLineEdit::setText(os, "projectFilePathEdit", projectName, dialog);
    }
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
