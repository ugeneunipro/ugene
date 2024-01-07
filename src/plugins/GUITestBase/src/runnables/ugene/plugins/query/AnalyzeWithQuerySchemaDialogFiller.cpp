/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "AnalyzeWithQuerySchemaDialogFiller.h"
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>

#include "GTUtilsTaskTreeView.h"
namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsDialog::AnalyzeWithQuerySchemaDialogFiller"

AnalyzeWithQuerySchemaDialogFiller::AnalyzeWithQuerySchemaDialogFiller(const QString& _fileWithQuery, bool _expectBadSchema)
    : Filler("QDDialog"), fileWithQuery(_fileWithQuery), expectBadSchema(_expectBadSchema) {
}

void AnalyzeWithQuerySchemaDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();
    GTLineEdit::setText("queryFileEdit", fileWithQuery, dialog);
    if (expectBadSchema) {
        GTUtilsTaskTreeView::waitTaskFinished();
        auto okButton = GTUtilsDialog::buttonBox(dialog)->button(QDialogButtonBox::Ok);
        CHECK_SET_ERR(okButton != nullptr, "Search button is not found");
        CHECK_SET_ERR(!okButton->isEnabled(), "Search button must be disabled");
        GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
    } else {
        GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
    }
}

#undef GT_CLASS_NAME

}  // namespace U2
