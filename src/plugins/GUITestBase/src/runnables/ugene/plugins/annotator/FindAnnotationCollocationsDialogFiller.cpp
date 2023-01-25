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

#include "FindAnnotationCollocationsDialogFiller.h"
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::FindAnnotationCollocationsDialogFiller"

#define GT_METHOD_NAME "commonScenario"
void FindAnnotationCollocationsDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getPlusButton"
QToolButton* FindAnnotationCollocationsDialogFiller::getPlusButton() const {
    QToolButton* result = nullptr;
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    QList<QToolButton*> toolList = dialog->findChildren<QToolButton*>();
    foreach (QToolButton* but, toolList) {
        if (but->text() == "+") {
            result = but;
        }
    }

    GT_CHECK_RESULT(result, "PlusButton not found", nullptr);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getMinusButtons"
QList<QToolButton*> FindAnnotationCollocationsDialogFiller::getMinusButtons() const {
    QList<QToolButton*> result;
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    QList<QToolButton*> toolList = dialog->findChildren<QToolButton*>();
    foreach (QToolButton* but, toolList) {
        if (but->text() == "-") {
            result << but;
        }
    }

    return result;
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
