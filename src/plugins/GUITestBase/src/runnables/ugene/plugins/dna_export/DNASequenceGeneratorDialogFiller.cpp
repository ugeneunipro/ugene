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

DNASequenceGeneratorDialogFiller::DNASequenceGeneratorDialogFiller(
    const DNASequenceGeneratorDialogFillerModel& _model)
    : Filler("DNASequenceGeneratorDialog"), model(_model) {
}

#define GT_METHOD_NAME "commonScenario"
void DNASequenceGeneratorDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    GTSpinBox::setValue("lengthSpin", model.length, dialog);
    GTSpinBox::setValue("windowSpinBox", model.window, dialog);
    if (model.numberOfSequences > 1) {
        GTSpinBox::setValue("seqNumSpin", model.numberOfSequences, dialog);
    }

    if (model.referenceUrl.isEmpty()) {
        GTRadioButton::click("baseContentRadioButton", dialog);
        GTSpinBox::setValue("percentASpin", model.percentA, dialog);
        GTSpinBox::setValue("percentCSpin", model.percentC, dialog);
        GTSpinBox::setValue("percentGSpin", model.percentG, dialog);
        GTSpinBox::setValue("percentTSpin", model.percentT, dialog);
    } else {
        GTLineEdit::setText("inputEdit", model.referenceUrl, dialog);
    }
    if (model.seed >= 0) {
        GTCheckBox::setChecked("seedCheckBox", true, dialog);
        GTSpinBox::setValue("seedSpinBox", model.seed, dialog);
    }
    GTLineEdit::setText("outputEdit", model.url, dialog);

    if (!model.formatId.isEmpty()) {
        DocumentFormat* format = AppContext::getDocumentFormatRegistry()->getFormatById(model.formatId);
        CHECK_SET_ERR(format != nullptr, "Format not found: " + model.formatId);
        GTComboBox::selectItemByText(GTWidget::findComboBox("formatCombo"), format->getFormatName());
    }

    GTWidget::click(GTWidget::findButtonByText("Generate", dialog));
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
