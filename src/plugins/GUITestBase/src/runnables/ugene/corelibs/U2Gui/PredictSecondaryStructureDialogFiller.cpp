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

#include "PredictSecondaryStructureDialogFiller.h"
#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QPushButton>

#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::PredictSecondaryStructureDialogFiller"

PredictSecondaryStructureDialogFiller::PredictSecondaryStructureDialogFiller(int startPos, int endPos, bool onlyPressOk)
    : Filler("SecStructDialog"),
      startPos(startPos),
      endPos(endPos),
      onlyPressOk(onlyPressOk) {
}

PredictSecondaryStructureDialogFiller::PredictSecondaryStructureDialogFiller(CustomScenario* scenario)
    : Filler("SecStructDialog", scenario),
      startPos(0),
      endPos(0),
      onlyPressOk(0) {
}

#define GT_METHOD_NAME "commonScenario"
void PredictSecondaryStructureDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    GTLineEdit::setText("start_edit_line", QString::number(startPos), dialog);

    GTLineEdit::setText("end_edit_line", QString::number(endPos), dialog);

    auto box = GTWidget::findDialogButtonBox("buttonBox", dialog);

    QPushButton* button = box->button(QDialogButtonBox::Ok);
    GT_CHECK(button != nullptr, "ok button is NULL");
    GTWidget::click(button);
    GTUtilsTaskTreeView::waitTaskFinished();
    if (onlyPressOk) {
        GTWidget::click(box->button(QDialogButtonBox::Cancel));
        return;
    }

    class Scenario : public CustomScenario {
    public:
        void run() {
            QWidget* dialog = GTWidget::getActiveModalWidget();

            GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(new CreateAnnotationWidgetFiller(new Scenario));

    QPushButton* saveButton = box->button(QDialogButtonBox::Save);
    GT_CHECK(saveButton != nullptr, "save button is NULL");
    GTWidget::click(saveButton);
    GTUtilsTaskTreeView::waitTaskFinished();
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}  // namespace U2
