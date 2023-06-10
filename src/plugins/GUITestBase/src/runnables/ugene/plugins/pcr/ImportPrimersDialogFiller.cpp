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

#include "ImportPrimersDialogFiller.h"
#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>

#include <U2Core/U2ObjectDbi.h>

#include "runnables/ugene/corelibs/U2Gui/ProjectTreeItemSelectorDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::ImportPrimersDialogFiller"

ImportPrimersDialogFiller::ImportPrimersDialogFiller(
    const QStringList& _fileList,
    const QMap<QString, QStringList>& _objectNameList)
    : Filler("ImportPrimersDialog"),
      fileList(_fileList),
      objectNameList(_objectNameList) {
}

ImportPrimersDialogFiller::ImportPrimersDialogFiller(CustomScenario* scenario)
    : Filler("ImportPrimersDialog", scenario) {
}

#define GT_METHOD_NAME "commonScenario"
void ImportPrimersDialogFiller::commonScenario() {
    if (fileList.isEmpty() && objectNameList.isEmpty()) {
        GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
        return;
    }

    for (const QString& file : qAsConst(fileList)) {
        addFile(file);
    }

    addObjects(objectNameList);

    GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addFile"
void ImportPrimersDialogFiller::addFile(const QString& filePath) {
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(filePath));
    GTWidget::click(GTWidget::findWidget("pbAddFile", getDialog()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addObject"
void ImportPrimersDialogFiller::addObjects(const QMap<QString, QStringList>& databaseAndObjectNames) {
    GTUtilsDialog::add(new ProjectTreeItemSelectorDialogFiller(databaseAndObjectNames, QSet<GObjectType>() << GObjectTypes::SEQUENCE, ProjectTreeItemSelectorDialogFiller::Separate));
    GTWidget::click(GTWidget::findWidget("pbAddObject", getDialog()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDialog"
QWidget* ImportPrimersDialogFiller::getDialog() {
    QWidget* dialog = GTWidget::getActiveModalWidget();
    return dialog;
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
