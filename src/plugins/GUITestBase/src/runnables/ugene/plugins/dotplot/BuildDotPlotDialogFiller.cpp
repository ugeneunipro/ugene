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

#include "BuildDotPlotDialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDialogButtonBox>

#include "GTUtilsTaskTreeView.h"

namespace U2 {

BuildDotPlotFiller::BuildDotPlotFiller(HI::GUITestOpStatus& _os,
                                       const QString& _firstFileName,
                                       const QString& _secondFileName,
                                       bool _mergeFirstBoxChecked,
                                       bool _oneSequenceBoxChecked,
                                       bool _mergeSecondBoxChecked,
                                       int _gapFirstValue,
                                       int _gapSecondValue,
                                       bool _cancel)
    : Filler(_os, "DotPlotFilesDialog"), mergeFirstBoxChecked(_mergeFirstBoxChecked),
      oneSequenceBoxChecked(_oneSequenceBoxChecked), mergeSecondBoxChecked(_mergeSecondBoxChecked),
      firstFileName(_firstFileName), secondFileName(_secondFileName), firstGapSize(_gapFirstValue),
      secondGapSize(_gapSecondValue), cancel(_cancel) {
}

#define GT_CLASS_NAME "GTUtilsDialog::DotPlotFiller"
#define GT_METHOD_NAME "commonScenario"
void BuildDotPlotFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    GTCheckBox::setChecked(os, "oneSequenceCheckBox", oneSequenceBoxChecked, dialog);
    GTLineEdit::setText(os, "firstFileEdit", firstFileName, dialog);

    GTCheckBox::setChecked(os, "mergeFirstCheckBox", mergeFirstBoxChecked, dialog);
    if (mergeFirstBoxChecked) {
        GTSpinBox::setValue(os, "gapFirst", firstGapSize, dialog);
    }

    if (!oneSequenceBoxChecked) {
        GTLineEdit::setText(os, "secondFileEdit", secondFileName, dialog);
        GTCheckBox::setChecked(os, "mergeSecondCheckBox", mergeSecondBoxChecked, dialog);
        if (mergeSecondBoxChecked) {
            GTSpinBox::setValue(os, "gapSecond", secondGapSize, dialog);
        }
    }
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
    if (cancel) {
        GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
    }
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
