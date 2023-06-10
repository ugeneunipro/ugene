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

#include "FindRepeatsDialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::FindRepeatsDialogFiller"
#define GT_METHOD_NAME "run"

FindRepeatsDialogFiller::FindRepeatsDialogFiller(
    const QString& _resultFilesPath,
    bool _searchInverted,
    int _minRepeatLength,
    int _repeatsIdentity,
    int _minDistance)
    : Filler("FindRepeatsDialog"), button(Start), resultAnnotationFilesPath(_resultFilesPath),
      searchInverted(_searchInverted), minRepeatLength(_minRepeatLength), repeatsIdentity(_repeatsIdentity), minDistance(_minDistance) {
}

FindRepeatsDialogFiller::FindRepeatsDialogFiller(CustomScenario* scenario)
    : Filler("FindRepeatsDialog", scenario),
      button(Start),
      searchInverted(false),
      minRepeatLength(0),
      repeatsIdentity(0),
      minDistance(0) {
}

void FindRepeatsDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();
    if (button == Cancel) {
        GTWidget::click(GTWidget::findWidget("cancelButton", dialog));
        return;
    }

    auto tabWidget = GTWidget::findTabWidget("tabWidget", dialog);
    GTTabWidget::setCurrentIndex(tabWidget, 0);

    if (minRepeatLength != -1) {
        GTSpinBox::setValue("minLenBox", minRepeatLength, dialog);
    }

    if (repeatsIdentity != -1) {
        GTSpinBox::setValue("identityBox", repeatsIdentity, dialog);
    }

    if (minDistance != -1) {
        GTSpinBox::setValue("minDistBox", minDistance, dialog);
    }

    GTLineEdit::setText("leNewTablePath", resultAnnotationFilesPath, dialog);

    GTTabWidget::setCurrentIndex(tabWidget, 1);

    GTCheckBox::setChecked("invertCheck", searchInverted, dialog);

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
