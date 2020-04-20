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

#include <primitives/GTCheckBox.h>
#include <primitives/GTRadioButton.h>

#include "EditSettingsDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::ExportChromatogramFiller"
EditSettingsDialogFiller::EditSettingsDialogFiller(HI::GUITestOpStatus &_os,
                                                   AnnotationPolicy _policy,
                                                   bool _recalculateQualifiers)
    : Filler(_os, "EditSettingDialogForm"),
      policy(_policy),
      recalculateQualifiers(_recalculateQualifiers)
{

}

#define GT_METHOD_NAME "commonScenario"
void EditSettingsDialogFiller::commonScenario() {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "dialog not found");

    QString radioButtonName;
    switch (policy) {
    case ExpandOrCropAffectedAnnotation:
        radioButtonName = "resizeRadioButton";
        break;
    case RemoveAffectedAnnotation:
        radioButtonName = "removeRadioButton";
        break;
    case SplitJoinAnnotationParts:
        radioButtonName = "splitRadioButton";
        break;
    case SplitSeparateAnnotationParts:
        radioButtonName = "split_separateRadioButton";
        break;
    default:
        GT_CHECK(false, "An unexpected AnnotationPolicy");
    }

    GTRadioButton::click(os, radioButtonName, dialog);

    GTCheckBox::setChecked(os, "recalculateQuals", recalculateQualifiers, dialog);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}   // namespace U2
