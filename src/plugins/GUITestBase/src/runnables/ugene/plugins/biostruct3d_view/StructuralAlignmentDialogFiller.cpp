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

#include "StructuralAlignmentDialogFiller.h"
#include <primitives/GTComboBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>

namespace U2 {

#define GT_CLASS_NAME "StructuralAlignmentDialogFiller"
StructuralAlignmentDialogFiller::StructuralAlignmentDialogFiller(const QStringList& chainIndexes)
    : Filler("StructuralAlignmentDialog"), chainIndexes(chainIndexes) {
}

void StructuralAlignmentDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    if (!chainIndexes.isEmpty()) {
        auto refEditor = GTWidget::findWidget("ref_editor", dialog);
        auto combo = GTWidget::findComboBox("chainCombo", refEditor);
        for (const QString& curString : qAsConst(chainIndexes)) {
            int index = combo->findText(curString, Qt::MatchContains);
            GT_CHECK(index != -1, "Index '" + curString + "' was not found");
        }
    }

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}

#undef GT_CLASS_NAME

}  // namespace U2
