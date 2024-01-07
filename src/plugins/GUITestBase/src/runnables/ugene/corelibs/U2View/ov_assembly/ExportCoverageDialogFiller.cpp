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

#include "ExportCoverageDialogFiller.h"
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>

#include <U2Core/U2SafePoints.h>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::ExportCoverageDialogFiller"

ExportCoverageDialogFiller::ExportCoverageDialogFiller(const QList<Action>& actions)
    : Filler("ExportCoverageDialog"),
      dialog(nullptr),
      actions(actions) {
}

void ExportCoverageDialogFiller::commonScenario() {
    dialog = GTWidget::getActiveModalWidget();
    QPointer<QWidget> dialogPtr(dialog);
    for (const Action& action : qAsConst(actions)) {
        GT_CHECK(dialogPtr != nullptr, "Dialog was closed before action: " + QString::number(action.first));
        switch (action.first) {
            case EnterFilePath:
                enterFilePath(action.second);
                break;
            case SelectFile:
                selectFile(action.second);
                break;
            case SetFormat:
                setFormat(action.second);
                break;
            case SetCompress:
                setCompress(action.second);
                break;
            case SetExportCoverage:
                setExportCoverage(action.second);
                break;
            case SetExportBasesQuantity:
                setExportBasesQuantity(action.second);
                break;
            case SetThreshold:
                setThreshold(action.second);
                break;
            case CheckFilePath:
                checkFilePath(action.second);
                break;
            case CheckFormat:
                checkFormat(action.second);
                break;
            case CheckCompress:
                checkCompress(action.second);
                break;
            case CheckOptionsVisibility:
                checkOptionsVisibility(action.second);
                break;
            case CheckExportCoverage:
                checkExportCoverage(action.second);
                break;
            case CheckExportBasesQuantity:
                checkExportBasesQuantity(action.second);
                break;
            case CheckThreshold:
                checkThreshold(action.second);
                break;
            case CheckThresholdBounds:
                checkThresholdBounds(action.second);
                break;
            case ExpectMessageBox:
                expectMessageBox();
                break;
            case ClickOk:
                clickOk();
                break;
            case ClickCancel: {
                clickCancel();
                break;
            }
            default:
                GT_FAIL("An unexpected dialog action", )
        }
    }
}

void ExportCoverageDialogFiller::enterFilePath(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't a get file path from the action data");
    GTLineEdit::setText("leFilePath", actionData.toString(), dialog);
}

void ExportCoverageDialogFiller::selectFile(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't a get file path from the action data");
    const QString dirPath = QFileInfo(actionData.toString()).dir().path();
    QDir().mkpath(dirPath);
    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dirPath, QFileInfo(actionData.toString()).fileName(), GTFileDialogUtils::Save));
    GTWidget::click(GTWidget::findWidget("tbFilePath", dialog));
    GTGlobals::sleep(500);
}

void ExportCoverageDialogFiller::setFormat(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get a format name from the action data");
    GTComboBox::selectItemByText(GTWidget::findComboBox("cbFormat", dialog), actionData.toString());
}

void ExportCoverageDialogFiller::setCompress(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<bool>(), "Can't get a checkbox state from the action data");
    GTCheckBox::setChecked(GTWidget::findCheckBox("chbCompress", dialog), actionData.toBool());
}

void ExportCoverageDialogFiller::setExportCoverage(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<bool>(), "Can't get a checkbox state from the action data");
    GTCheckBox::setChecked(GTWidget::findCheckBox("chbExportCoverage", dialog), actionData.toBool());
}

void ExportCoverageDialogFiller::setExportBasesQuantity(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<bool>(), "Can't get a checkbox state from the action data");
    GTCheckBox::setChecked(GTWidget::findCheckBox("chbExportBasesQuantity", dialog), actionData.toBool());
}

void ExportCoverageDialogFiller::setThreshold(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<int>(), "Can't get a threshold value from the action data");
    GTSpinBox::setValue(GTWidget::findSpinBox("sbThreshold", dialog), actionData.toInt());
}

void ExportCoverageDialogFiller::checkFilePath(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get an expected file path from the action data");
    auto leFilePath = GTWidget::findLineEdit("leFilePath", dialog);
    GT_CHECK(leFilePath->text() == actionData.toString(), QString("An unexpected file path: expected '%1', got '%2'").arg(actionData.toString()).arg(leFilePath->text()));
}

void ExportCoverageDialogFiller::checkFormat(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get an expected format name from the action data");
    auto cbFormat = GTWidget::findComboBox("cbFormat", dialog);
    GT_CHECK(cbFormat->currentText() == actionData.toString(), QString("An unexpected format is set: expect '%1' got '%2'").arg(actionData.toString()).arg(cbFormat->currentText()));
}

void ExportCoverageDialogFiller::checkCompress(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<bool>(), "Can't get an expected checkbox state from the action data");
    auto chbCompress = GTWidget::findCheckBox("chbCompress", dialog);
    GT_CHECK(chbCompress->isChecked() == actionData.toBool(), "An unexpected checkbox state");
}

void ExportCoverageDialogFiller::checkOptionsVisibility(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<bool>(), "Can't get an expected groupbox visibility state from the action data");
    auto gbAdditionalOptions = GTWidget::findWidget("gbAdditionalOptions", dialog);
    GT_CHECK(gbAdditionalOptions->isVisible() == actionData.toBool(), "An unexpected additional options groupbox visibility state");
}

void ExportCoverageDialogFiller::checkExportCoverage(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<bool>(), "Can't get an expected checkbox state from the action data");
    auto chbExportCoverage = GTWidget::findCheckBox("chbExportCoverage", dialog);
    GT_CHECK(chbExportCoverage->isChecked() == actionData.toBool(), "An unexpected checkbox state");
}

void ExportCoverageDialogFiller::checkExportBasesQuantity(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<bool>(), "Can't get an expected checkbox state from the action data");
    auto chbExportBasesQuantity = GTWidget::findCheckBox("chbExportBasesQuantity", dialog);
    GT_CHECK(chbExportBasesQuantity->isChecked() == actionData.toBool(), "An unexpected checkbox state");
}

void ExportCoverageDialogFiller::checkThreshold(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<int>(), "Can't get an expected spinbox value from the action data");
    auto sbThreshold = GTWidget::findSpinBox("sbThreshold", dialog);
    GT_CHECK(sbThreshold->value() == actionData.toInt(), "An unexpected spinbox value");
}

void ExportCoverageDialogFiller::checkThresholdBounds(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QPoint>(), "Can't get a QPoint with expected spinbox bounds values from the action data");
    auto sbThreshold = GTWidget::findSpinBox("sbThreshold", dialog);
    const QPoint spinboxBounds = actionData.value<QPoint>();
    GTSpinBox::checkLimits(sbThreshold, spinboxBounds.x(), spinboxBounds.y());
}

void ExportCoverageDialogFiller::expectMessageBox() {
    GTUtilsDialog::waitForDialog(new MessageBoxDialogFiller(QMessageBox::Ok));
}

void ExportCoverageDialogFiller::clickOk() {
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}

void ExportCoverageDialogFiller::clickCancel() {
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
}

#undef GT_CLASS_NAME

}  // namespace U2
