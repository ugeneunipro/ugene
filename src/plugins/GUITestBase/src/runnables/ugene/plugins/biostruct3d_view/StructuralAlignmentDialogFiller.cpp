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

#include "StructuralAlignmentDialogFiller.h"
#include <primitives/GTWidget.h>
#include <primitives/GTComboBox.h>

namespace U2 {

#define GT_CLASS_NAME "StructuralAlignmentDialogFiller"
StructuralAlignmentDialogFiller::StructuralAlignmentDialogFiller(HI::GUITestOpStatus &os, const QStringList& chainIndexes) :
    Filler(os, "StructuralAlignmentDialog"), chainIndexes(chainIndexes)
{
}

#define GT_METHOD_NAME "commonScenario"
void StructuralAlignmentDialogFiller::commonScenario() {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");

    if (!chainIndexes.isEmpty()){
        QComboBox* combo = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "chainCombo", dialog));
        CHECK_SET_ERR(combo != NULL, "chainCombo not found!");
        foreach(const QString& curString, chainIndexes) {
            int index = combo->findText(curString, Qt::MatchContains);
            GT_CHECK(index != -1, "Index '" + curString + "' was not found");
        }
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}   // namespace U2
