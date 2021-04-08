/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

BuildDotPlotFiller::BuildDotPlotFiller(HI::GUITestOpStatus &_os,
                                       const QString &_firstFileName,
                                       const QString &_secondFileName,
                                       bool _mergeFirstBoxChecked,
                                       bool _oneSequenceBoxChecked,
                                       bool _mergeSecondBoxChecked,
                                       int _gapFirstValue,
                                       int _gapSecondValue,
                                       bool cancel)
    : Filler(_os, "DotPlotFilesDialog"), mergeFirstBoxChecked(_mergeFirstBoxChecked),
      oneSequenceBoxChecked(_oneSequenceBoxChecked), mergeSecondBoxChecked(_mergeSecondBoxChecked),
      firstFileName(_firstFileName), secondFileName(_secondFileName), firstGapSize(_gapFirstValue),
      secondGapSize(_gapSecondValue), cancel(cancel) {
}

#define GT_CLASS_NAME "GTUtilsDialog::DotPlotFiller"
#define GT_METHOD_NAME "commonScenario"
void BuildDotPlotFiller::commonScenario() {
    QWidget *dialog = GTWidget::getActiveModalWidget(os);

    GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, "oneSequenceCheckBox", dialog), oneSequenceBoxChecked);
    GTLineEdit::setText(os, GTWidget::findLineEdit(os, "firstFileEdit", dialog), firstFileName);

    GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, "mergeFirstCheckBox", dialog), mergeFirstBoxChecked);
    if (mergeFirstBoxChecked) {
        GTSpinBox::setValue(os, GTWidget::findExactWidget<QSpinBox *>(os, "gapFirst", dialog), firstGapSize);
    }

    if (!oneSequenceBoxChecked) {
        GTLineEdit::setText(os, GTWidget::findLineEdit(os, "secondFileEdit", dialog), secondFileName);
        GTCheckBox::setChecked(os, GTWidget::findCheckBox(os, "mergeSecondCheckBox", dialog), mergeSecondBoxChecked);
        if (mergeSecondBoxChecked) {
            GTSpinBox::setValue(os, GTWidget::findExactWidget<QSpinBox *>(os, "gapSecond", dialog), secondGapSize);
        }
    }
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
    if (cancel) {
        dialog = GTWidget::getActiveModalWidget(os);
        GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
    }
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}    // namespace U2
