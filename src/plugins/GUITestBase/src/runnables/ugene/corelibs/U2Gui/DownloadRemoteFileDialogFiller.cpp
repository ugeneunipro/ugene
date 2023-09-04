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

#include "DownloadRemoteFileDialogFiller.h"
#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTListWidget.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileInfo>
#include <QListWidget>
#include <QPushButton>

#include <U2Core/U2SafePoints.h>

namespace U2 {

#define GT_CLASS_NAME "DownloadRemoteFileDialogFiller"
DownloadRemoteFileDialogFiller::DownloadRemoteFileDialogFiller(const QList<DownloadRemoteFileDialogFiller::Action>& actions)
    : Filler("DownloadRemoteFileDialog"),
      actions(actions) {
}

DownloadRemoteFileDialogFiller::DownloadRemoteFileDialogFiller(CustomScenario* c)
    : Filler("DownloadRemoteFileDialog", c) {
}

void DownloadRemoteFileDialogFiller::commonScenario() {
    dialog = GTWidget::getActiveModalWidget();

    for (const Action& action : qAsConst(actions)) {
        switch (action.first) {
            case SetResourceIds:
                setResourceIds(action.second);
                break;
            case SetDatabase:
                setDatabase(action.second);
                break;
            case EnterSaveToDirectoryPath:
                enterSaveToDirectoryPath(action.second);
                break;
            case SelectSaveToDirectoryPath:
                selectSaveToDirectoryPath(action.second);
                break;
            case SetOutputFormat:
                setOutputFormat(action.second);
                break;
            case SetForceSequenceDownload:
                setForceSequenceDownload(action.second);
                break;
            case CheckDatabase:
                checkDatabase(action.second);
                break;
            case CheckDatabasesCount:
                checkDatabasesCount(action.second);
                break;
            case CheckOutputFormatVisibility:
                checkOutputFormatVisibility(action.second);
                break;
            case CheckOutputFormat:
                checkOutputFormat(action.second);
                break;
            case CheckForceSequenceDownloadVisibility:
                checkForceSequenceDownloadVisibility(action.second);
                break;
            case CheckForceSequenceDownload:
                checkForceSequenceDownload(action.second);
                break;
            case ClickOk:
                clickOk();
                break;
            case ClickCancel:
                clickCancel();
                break;
        }
    }
}

void DownloadRemoteFileDialogFiller::setResourceIds(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QStringList>(), "Can't get IDs list from the action data");
    GTLineEdit::setText("idLineEdit", actionData.toStringList().join(" "), dialog);
}

void DownloadRemoteFileDialogFiller::setDatabase(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get database name from the action data");
    GTComboBox::selectItemByText(GTWidget::findComboBox("databasesBox", dialog), actionData.toString());
}

void DownloadRemoteFileDialogFiller::enterSaveToDirectoryPath(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get folder path from the action data");
    GTLineEdit::setText("saveFilenameLineEdit", actionData.toString(), dialog);
}

void DownloadRemoteFileDialogFiller::selectSaveToDirectoryPath(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get folder path from the action data");
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(QFileInfo(actionData.toString()).absoluteDir().absolutePath(), "", GTFileDialogUtils::Choose));
    GTWidget::click(GTWidget::findWidget("saveFilenameToolButton", dialog));
}

void DownloadRemoteFileDialogFiller::setOutputFormat(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get format name from the action data");
    auto formatBox = GTWidget::findComboBox("formatBox", dialog);
    GT_CHECK(formatBox != nullptr, "Format combobox was not found");
    GT_CHECK(formatBox->isVisible(), "Format combobox is invisible");
    GTComboBox::selectItemByText(formatBox, actionData.toString());
}

void DownloadRemoteFileDialogFiller::setForceSequenceDownload(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<bool>(), "Can't get checkbox state from the action data");
    auto chbForceDownloadSequence = GTWidget::findCheckBox("chbForceDownloadSequence", dialog);
    GT_CHECK(chbForceDownloadSequence != nullptr, "Force download sequence checkbox was not found");
    GT_CHECK(chbForceDownloadSequence->isVisible(), "Force download sequence checkbox is invisible");
    GTCheckBox::setChecked(chbForceDownloadSequence, actionData.toBool());
}

void DownloadRemoteFileDialogFiller::checkDatabase(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get an expected database name from the action data");
    auto databasesBox = GTWidget::findComboBox("databasesBox", dialog);
    GT_CHECK(actionData.toString() == databasesBox->currentText(), QString("An unexpected database: expect '%1', got '%2'").arg(actionData.toString()).arg(databasesBox->currentText()));
}

void DownloadRemoteFileDialogFiller::checkDatabasesCount(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<int>(), "Can't get an expected databases count from the action data");
    auto databasesBox = GTWidget::findComboBox("databasesBox", dialog);
    GT_CHECK(actionData.toInt() == databasesBox->count(), QString("An unexpected databases count: expect '%1', got '%2'").arg(actionData.toInt()).arg(databasesBox->count()));
}

void DownloadRemoteFileDialogFiller::checkOutputFormatVisibility(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<bool>(), "Can't get an expected format combobox visibility state from the action data");
    auto formatBox = GTWidget::findComboBox("formatBox", dialog);
    GT_CHECK(actionData.toBool() == formatBox->isVisible(), "Format combobox has an unexpected visibility state");
}

void DownloadRemoteFileDialogFiller::checkOutputFormat(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get an expected format name from the action data");
    auto formatBox = GTWidget::findComboBox("formatBox", dialog);
    GT_CHECK(formatBox->isVisible(), "Format combobox is invisible");
    GT_CHECK(actionData.toString() == formatBox->currentText(), QString("An unexpected format: expect '%1', got '%2'").arg(actionData.toString()).arg(formatBox->currentText()));
}

void DownloadRemoteFileDialogFiller::checkForceSequenceDownloadVisibility(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<bool>(), "Can't get expected visibility state from the action data");
    auto chbForceDownloadSequence = GTWidget::findCheckBox("chbForceDownloadSequence", dialog);
    GT_CHECK(actionData.toBool() == chbForceDownloadSequence->isVisible(), "Force download sequence checkbox has incorrect invisibility state");
}

void DownloadRemoteFileDialogFiller::checkForceSequenceDownload(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<bool>(), "Can't get an expected checkbox state from the action data");
    auto chbForceDownloadSequence = GTWidget::findCheckBox("chbForceDownloadSequence", dialog);
    GT_CHECK(actionData.toBool() == chbForceDownloadSequence->isChecked(), "Force download sequence checkbox has incorrect state");
}

void DownloadRemoteFileDialogFiller::clickOk() {
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}

void DownloadRemoteFileDialogFiller::clickCancel() {
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
}

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::RemoteDBDialogFillerDeprecated"
void RemoteDBDialogFillerDeprecated::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    if (!resID.isEmpty()) {
        GTLineEdit::setText("idLineEdit", resID, dialog);
    }

    if (!saveDirPath.isEmpty()) {
        GTLineEdit::setText("saveFilenameLineEdit", saveDirPath, dialog);
    }

    auto databasesBox = GTWidget::findComboBox("databasesBox", dialog);
    GTComboBox::selectItemByIndex(databasesBox, DBItemNum, useMethod);

    GTCheckBox::setChecked(GTWidget::findCheckBox("chbForceDownloadSequence", dialog), forceGetSequence);

    if (outFormatVal != -1) {
        auto formatBox = GTWidget::findComboBox("formatBox");
        GTComboBox::selectItemByIndex(formatBox, outFormatVal, useMethod);
    }
    if (!addToProject) {
        auto addToProjectButton = GTWidget::findCheckBox("chbAddToProjectCheck", dialog);
        GTCheckBox::setChecked(addToProjectButton, false);
    }
    if (pressCancel) {
        GTUtilsDialog::clickButtonBox(QDialogButtonBox::Cancel);
    } else {
        GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
    }
}

#undef GT_CLASS_NAME

}  // namespace U2
