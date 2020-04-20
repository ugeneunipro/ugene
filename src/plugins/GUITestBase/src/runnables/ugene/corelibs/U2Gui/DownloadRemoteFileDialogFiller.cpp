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

#include <QDir>
#include <QFileInfo>

#include "DownloadRemoteFileDialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTListWidget.h>
#include <primitives/GTWidget.h>
#include <U2Core/U2SafePoints.h>

#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QPushButton>

namespace U2 {

#define GT_CLASS_NAME "DownloadRemoteFileDialogFiller"
DownloadRemoteFileDialogFiller::DownloadRemoteFileDialogFiller(HI::GUITestOpStatus &os, const QList<DownloadRemoteFileDialogFiller::Action> &actions) :
    Filler(os, "DownloadRemoteFileDialog"),
    actions(actions),
    dialog(NULL)
{
}

DownloadRemoteFileDialogFiller::DownloadRemoteFileDialogFiller(HI::GUITestOpStatus &os, CustomScenario *c)
    : Filler(os, "DownloadRemoteFileDialog", c),
      dialog(NULL)
{

}

#define GT_METHOD_NAME "commonScenario"
void DownloadRemoteFileDialogFiller::commonScenario() {
    dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    foreach (const Action &action, actions) {
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
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setResourceIds"
void DownloadRemoteFileDialogFiller::setResourceIds(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QStringList>(), "Can't get IDs list from the action data");
    GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "idLineEdit", dialog), actionData.toStringList().join(" "));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setDatabase"
void DownloadRemoteFileDialogFiller::setDatabase(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QString>(), "Can't get database name from the action data");
    GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "databasesBox", dialog), actionData.toString());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "enterSaveToDirectoryPath"
void DownloadRemoteFileDialogFiller::enterSaveToDirectoryPath(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QString>(), "Can't get folder path from the action data");
    GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "saveFilenameLineEdit", dialog), actionData.toString());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectSaveToDirectoryPath"
void DownloadRemoteFileDialogFiller::selectSaveToDirectoryPath(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QString>(), "Can't get folder path from the action data");
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, QFileInfo(actionData.toString()).absoluteDir().absolutePath(), "", GTFileDialogUtils::Choose));
    GTWidget::click(os, GTWidget::findWidget(os, "saveFilenameToolButton", dialog));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setOutputFormat"
void DownloadRemoteFileDialogFiller::setOutputFormat(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QString>(), "Can't get format name from the action data");
    QComboBox *formatBox = GTWidget::findExactWidget<QComboBox *>(os, "formatBox", dialog);
    GT_CHECK(NULL != formatBox, "Format combobox was not found");
    GT_CHECK(formatBox->isVisible(), "Format combobox is invisible");
    GTComboBox::setIndexWithText(os, formatBox, actionData.toString());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setForceSequenceDownload"
void DownloadRemoteFileDialogFiller::setForceSequenceDownload(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<bool>(), "Can't get checkbox state from the action data");
    QCheckBox *chbForceDownloadSequence = GTWidget::findExactWidget<QCheckBox *>(os, "chbForceDownloadSequence", dialog);
    GT_CHECK(NULL != chbForceDownloadSequence, "Force download sequence checkbox was not found");
    GT_CHECK(chbForceDownloadSequence->isVisible(), "Force download sequence checkbox is invisible");
    GTCheckBox::setChecked(os, chbForceDownloadSequence, actionData.toBool());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkDatabase"
void DownloadRemoteFileDialogFiller::checkDatabase(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QString>(), "Can't get an expected database name from the action data");
    QComboBox *databasesBox = GTWidget::findExactWidget<QComboBox *>(os, "databasesBox", dialog);
    GT_CHECK(NULL != databasesBox, "Databases combobox wasn't found");
    GT_CHECK(actionData.toString() == databasesBox->currentText(), QString("An unexpected database: expect '%1', got '%2'").arg(actionData.toString()).arg(databasesBox->currentText()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkDatabasesCount"
void DownloadRemoteFileDialogFiller::checkDatabasesCount(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<int>(), "Can't get an expected databases count from the action data");
    QComboBox *databasesBox = GTWidget::findExactWidget<QComboBox *>(os, "databasesBox", dialog);
    GT_CHECK(NULL != databasesBox, "Databases combobox wasn't found");
    GT_CHECK(actionData.toInt() == databasesBox->count(), QString("An unexpected databases count: expect '%1', got '%2'").arg(actionData.toInt()).arg(databasesBox->count()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkOutputFormatVisibility"
void DownloadRemoteFileDialogFiller::checkOutputFormatVisibility(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<bool>(), "Can't get an expected format combobox visibility state from the action data");
    QComboBox *formatBox = GTWidget::findExactWidget<QComboBox *>(os, "formatBox", dialog);
    GT_CHECK(NULL != formatBox, "Format combobox was not found");
    GT_CHECK(actionData.toBool() == formatBox->isVisible(), "Format combobox has an unexpected visibility state");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkOutputFormat"
void DownloadRemoteFileDialogFiller::checkOutputFormat(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QString>(), "Can't get an expected format name from the action data");
    QComboBox *formatBox = GTWidget::findExactWidget<QComboBox *>(os, "formatBox", dialog);
    GT_CHECK(NULL != formatBox, "Format combobox was not found");
    GT_CHECK(formatBox->isVisible(), "Format combobox is invisible");
    GT_CHECK(actionData.toString() == formatBox->currentText(), QString("An unexpected format: expect '%1', got '%2'").arg(actionData.toString()).arg(formatBox->currentText()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkForceSequenceDownloadVisibility"
void DownloadRemoteFileDialogFiller::checkForceSequenceDownloadVisibility(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<bool>(), "Can't get expected visibility state from the action data");
    QCheckBox *chbForceDownloadSequence = GTWidget::findExactWidget<QCheckBox *>(os, "chbForceDownloadSequence", dialog);
    GT_CHECK(NULL != chbForceDownloadSequence, "Force download sequence checkbox was not found");
    GT_CHECK(actionData.toBool() == chbForceDownloadSequence->isVisible(), "Force download sequence checkbox has incorrect invisibility state");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkForceSequenceDownload"
void DownloadRemoteFileDialogFiller::checkForceSequenceDownload(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<bool>(), "Can't get an expected checkbox state from the action data");
    QCheckBox *chbForceDownloadSequence = GTWidget::findExactWidget<QCheckBox *>(os, "chbForceDownloadSequence", dialog);
    GT_CHECK(NULL != chbForceDownloadSequence, "Force download sequence checkbox was not found");
    GT_CHECK(actionData.toBool() == chbForceDownloadSequence->isChecked(), "Force download sequence checkbox has incorrect state");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickOk"
void DownloadRemoteFileDialogFiller::clickOk() {
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickCancel"
void DownloadRemoteFileDialogFiller::clickCancel() {
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::RemoteDBDialogFillerDeprecated"
#define GT_METHOD_NAME "commonScenario"
void RemoteDBDialogFillerDeprecated::commonScenario() {

    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    if (!resID.isEmpty()){
        QLineEdit *idLineEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "idLineEdit", dialog));
        GTLineEdit::setText(os, idLineEdit, resID);
    }

    QLineEdit *saveFilenameLineEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "saveFilenameLineEdit", dialog));
    if(!saveDirPath.isEmpty()){
        GTLineEdit::setText(os, saveFilenameLineEdit, saveDirPath);
    }

    QComboBox *databasesBox = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "databasesBox", dialog));
    GTComboBox::setCurrentIndex(os, databasesBox, DBItemNum, true, useMethod);

    GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox *>(os, "chbForceDownloadSequence", dialog), forceGetSequence);

    if (outFormatVal!=-1){
        QComboBox* formatBox = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "formatBox"));
        GTComboBox::setCurrentIndex(os, formatBox, outFormatVal, true, useMethod);
    }
    if (!addToProject){
        QCheckBox *addToProjectButton = dialog->findChild<QCheckBox*>(QString::fromUtf8("chbAddToProjectCheck"));
        GT_CHECK(addToProjectButton != NULL, "Check box not found");
        GTCheckBox::setChecked(os, addToProjectButton, false);
    }
    if (pressCancel) {
        QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", dialog));
        GT_CHECK(box != NULL, "buttonBox is NULL");
        QPushButton* button = box->button(QDialogButtonBox::Cancel);
        GT_CHECK(button !=NULL, "cancel button is NULL");
        GTWidget::click(os, button);
    }
    else {
        QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", dialog));
        GT_CHECK(box != NULL, "buttonBox is NULL");
        QPushButton* button = box->button(QDialogButtonBox::Ok);
        GT_CHECK(button !=NULL, "cancel button is NULL");
        GTWidget::click(os, button);
    }
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

}   // namespace U2
