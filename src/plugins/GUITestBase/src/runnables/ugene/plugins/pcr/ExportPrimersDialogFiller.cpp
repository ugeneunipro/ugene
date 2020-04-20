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
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLineEdit>

#include "ExportPrimersDialogFiller.h"
#include "GTUtilsTaskTreeView.h"
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTWidget.h>
#include "runnables/ugene/corelibs/U2Gui/SharedConnectionsDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::ExportPrimersDialogFiller"

ExportPrimersDialogFiller::ExportPrimersDialogFiller(HI::GUITestOpStatus &os, CustomScenario *scenario) :
    Filler(os, "ExportPrimersDialog", scenario)
{
}

#define GT_METHOD_NAME "commonScenario"
void ExportPrimersDialogFiller::commonScenario() {
    GTUtilsDialog::clickButtonBox(os, getDialog(os), QDialogButtonBox::Cancel);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDialog"
QWidget *ExportPrimersDialogFiller::getDialog(HI::GUITestOpStatus &os) {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK_RESULT(NULL != dialog, "Active modal dialog is NULL", NULL);
    return dialog;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setExportTarget"
void ExportPrimersDialogFiller::setExportTarget(HI::GUITestOpStatus &os, ExportPrimersDialogFiller::ExportTarget exportTarget) {
    switch (exportTarget) {
    case LocalFile:
        GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "cbExport", getDialog(os)), "Local file");
        break;
    case SharedDb:
        GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "cbExport", getDialog(os)), "Shared database");
        break;
    default:
        os.setError("Unexpected export target");
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setFormat"
void ExportPrimersDialogFiller::setFormat(HI::GUITestOpStatus &os, const QString &format) {
    GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "cbFormat", getDialog(os)), format);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setFilePath"
void ExportPrimersDialogFiller::setFilePath(HI::GUITestOpStatus &os, const QString &filePath) {
    GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leFilePath", getDialog(os)), filePath);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setDatabase"
void ExportPrimersDialogFiller::setDatabase(HI::GUITestOpStatus &os, const QString &database) {
    QComboBox *cbDatabase = GTWidget::findExactWidget<QComboBox *>(os, "cbDatabase", getDialog(os));
    GT_CHECK(NULL != cbDatabase, "Database combobox is NULL");
    if (-1 == cbDatabase->findText(database)) {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, database);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CONNECT);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
        GTWidget::click(os, GTWidget::findWidget(os, "tbConnect", getDialog(os)));
        GTUtilsTaskTreeView::waitTaskFinished(os);
    }
    GTComboBox::setIndexWithText(os, cbDatabase, database);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setFolder"
void ExportPrimersDialogFiller::setFolder(HI::GUITestOpStatus &os, const QString &folder) {
    GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leFolder", getDialog(os)), folder);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}   // namespace U2
