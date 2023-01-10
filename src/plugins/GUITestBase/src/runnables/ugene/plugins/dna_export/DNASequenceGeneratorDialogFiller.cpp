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

#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>

#include "DNASequenceGeneratorDialogFiller.h"

namespace U2 {

DNASequenceGeneratorDialogFillerModel::DNASequenceGeneratorDialogFillerModel(const QString& _url)
    : url(_url) {
}

#define GT_CLASS_NAME "GTUtilsDialog::DNASequenceGeneratorDialogFiller"

DNASequenceGeneratorDialogFiller::DNASequenceGeneratorDialogFiller(GUITestOpStatus& os,
                                                                   const DNASequenceGeneratorDialogFillerModel& _model)
    : Filler(os, "DNASequenceGeneratorDialog"), model(_model) {
}

#define GT_METHOD_NAME "commonScenario"
void DNASequenceGeneratorDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

    GTSpinBox::setValue(os, "lengthSpin", model.length, dialog);
    GTSpinBox::setValue(os, "windowSpinBox", model.window, dialog);
    if (model.numberOfSequences > 1) {
        GTSpinBox::setValue(os, "seqNumSpin", model.numberOfSequences, dialog);
    }

    if (model.referenceUrl.isEmpty()) {
        GTRadioButton::click(os, "baseContentRadioButton", dialog);
        GTSpinBox::setValue(os, "percentASpin", model.percentA, dialog);
        GTSpinBox::setValue(os, "percentCSpin", model.percentC, dialog);
        GTSpinBox::setValue(os, "percentGSpin", model.percentG, dialog);
        GTSpinBox::setValue(os, "percentTSpin", model.percentT, dialog);
    } else {
        GTLineEdit::setText(os, "inputEdit", model.referenceUrl, dialog);
    }
    if (model.seed >= 0) {
        GTCheckBox::setChecked(os, "seedCheckBox", true, dialog);
        GTSpinBox::setValue(os, "seedSpinBox", model.seed, dialog);
    }
    GTLineEdit::setText(os, "outputEdit", model.url, dialog);

    if (!model.formatId.isEmpty()) {
        DocumentFormat* format = AppContext::getDocumentFormatRegistry()->getFormatById(model.formatId);
        CHECK_SET_ERR(format != nullptr, "Format not found: " + model.formatId);
        GTComboBox::selectItemByText(os, GTWidget::findComboBox(os, "formatCombo"), format->getFormatName());
    }

    GTWidget::click(os, GTWidget::findButtonByText(os, "Generate", dialog));
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
