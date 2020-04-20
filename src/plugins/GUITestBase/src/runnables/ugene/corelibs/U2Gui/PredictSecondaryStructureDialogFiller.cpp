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


#include "PredictSecondaryStructureDialogFiller.h"

#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include "runnables/ugene/corelibs/U2Gui/CreateAnnotationWidgetFiller.h"

#include "GTUtilsTaskTreeView.h"

#include <QApplication>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::PredictSecondaryStructureDialogFiller"

PredictSecondaryStructureDialogFiller::PredictSecondaryStructureDialogFiller(HI::GUITestOpStatus &os, int startPos, int endPos, bool onlyPressOk) :
    Filler(os, "SecStructDialog"),
    startPos(startPos),
    endPos(endPos),
    onlyPressOk(onlyPressOk)
{

}

PredictSecondaryStructureDialogFiller::PredictSecondaryStructureDialogFiller(HI::GUITestOpStatus &os, CustomScenario *scenario) :
    Filler(os, "SecStructDialog", scenario),
    startPos(0),
    endPos(0),
    onlyPressOk(0)
{

}

#define GT_METHOD_NAME "commonScenario"
void PredictSecondaryStructureDialogFiller::commonScenario() {
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QLineEdit * startLineEdit = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "start_edit_line", dialog));
    GT_CHECK(startLineEdit != NULL, "Start lineEdit is NULL");
    GTLineEdit::setText(os, startLineEdit, QString::number(startPos));

    QLineEdit * endLineEdit = qobject_cast<QLineEdit *>(GTWidget::findWidget(os, "end_edit_line", dialog));
    GT_CHECK(endLineEdit != NULL, "Start lineEdit is NULL");
    GTLineEdit::setText(os, endLineEdit, QString::number(endPos));

    QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", dialog));

    QPushButton* button = box->button(QDialogButtonBox::Ok);
    GT_CHECK(button != NULL, "ok button is NULL");
    GTWidget::click(os, button);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    if (onlyPressOk) {
        GTWidget::click(os, box->button(QDialogButtonBox::Cancel));
        return;
    }

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new CreateAnnotationWidgetFiller(os, new Scenario));

    QPushButton* saveButton = box->button(QDialogButtonBox::Save);
    GT_CHECK(saveButton != NULL, "save button is NULL");
    GTWidget::click(os, saveButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);

}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

} // namespace
