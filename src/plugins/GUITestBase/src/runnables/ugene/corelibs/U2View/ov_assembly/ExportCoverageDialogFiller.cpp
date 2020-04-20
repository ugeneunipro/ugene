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

#include <QApplication>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>

#include "ExportCoverageDialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <base_dialogs/GTFileDialog.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::BranchSettingsDialogFiller"

ExportCoverageDialogFiller::ExportCoverageDialogFiller(HI::GUITestOpStatus &os, const QList<Action> &actions) :
    Filler(os, "ExportCoverageDialog"),
    dialog(NULL),
    actions(actions)
{
}

#define GT_METHOD_NAME "commonScenario"
void ExportCoverageDialogFiller::commonScenario() {
    dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL, "Active modal widget not found");

    foreach (const Action &action, actions) {
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
        os.setError("An unexpected dialog action");
        clickCancel();
        break;
    }
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "enterFilePath"
void ExportCoverageDialogFiller::enterFilePath(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QString>(), "Can't a get file path from the action data");
    GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "leFilePath", dialog), actionData.toString());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectFile"
void ExportCoverageDialogFiller::selectFile(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QString>(), "Can't a get file path from the action data");
    const QString dirPath = QFileInfo(actionData.toString()).dir().path();
    QDir().mkpath(dirPath);
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os,
                                                           dirPath,
                                                           QFileInfo(actionData.toString()).fileName(),
                                                           GTFileDialogUtils::Save));
    GTWidget::click(os, GTWidget::findWidget(os, "tbFilePath", dialog));
    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setFormat"
void ExportCoverageDialogFiller::setFormat(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QString>(), "Can't get a format name from the action data");
    GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "cbFormat", dialog), actionData.toString());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setCompress"
void ExportCoverageDialogFiller::setCompress(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<bool>(), "Can't get a checkbox state from the action data");
    GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox *>(os, "chbCompress", dialog), actionData.toBool());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setExportCoverage"
void ExportCoverageDialogFiller::setExportCoverage(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<bool>(), "Can't get a checkbox state from the action data");
    GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox *>(os, "chbExportCoverage", dialog), actionData.toBool());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setExportBasesQuantity"
void ExportCoverageDialogFiller::setExportBasesQuantity(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<bool>(), "Can't get a checkbox state from the action data");
    GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox *>(os, "chbExportBasesQuantity", dialog), actionData.toBool());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setThreshold"
void ExportCoverageDialogFiller::setThreshold(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<int>(), "Can't get a threshold value from the action data");
    GTSpinBox::setValue(os, GTWidget::findExactWidget<QSpinBox *>(os, "sbThreshold", dialog), actionData.toInt());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkFilePath"
void ExportCoverageDialogFiller::checkFilePath(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QString>(), "Can't get an expected file path from the action data");
    QLineEdit *leFilePath = GTWidget::findExactWidget<QLineEdit *>(os, "leFilePath", dialog);
    GT_CHECK(NULL != leFilePath, "A file path line edit wasn't found");
    GT_CHECK(leFilePath->text() == actionData.toString(), QString("An unexpected file path: expected '%1', got '%2'").arg(actionData.toString()).arg(leFilePath->text()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkFormat"
void ExportCoverageDialogFiller::checkFormat(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QString>(), "Can't get an expected format name from the action data");
    QComboBox *cbFormat = GTWidget::findExactWidget<QComboBox *>(os, "cbFormat", dialog);
    GT_CHECK(NULL != cbFormat, "A format combobox wasn't found");
    GT_CHECK(cbFormat->currentText() == actionData.toString(), QString("An unexpected format is set: expect '%1' got '%2'").arg(actionData.toString()).arg(cbFormat->currentText()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkCompress"
void ExportCoverageDialogFiller::checkCompress(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<bool>(), "Can't get an expected checkbox state from the action data");
    QCheckBox *chbCompress = GTWidget::findExactWidget<QCheckBox *>(os, "chbCompress", dialog);
    GT_CHECK(NULL != chbCompress, "A compress checkbox wasn't found");
    GT_CHECK(chbCompress->isChecked() == actionData.toBool(), "An unexpected checkbox state");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkOptionsVisibility"
void ExportCoverageDialogFiller::checkOptionsVisibility(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<bool>(), "Can't get an expected groupbox visibility state from the action data");
    QWidget *gbAdditionalOptions = GTWidget::findWidget(os, "gbAdditionalOptions", dialog);
    GT_CHECK(NULL != gbAdditionalOptions, "An additional options widget wasn't found");
    GT_CHECK(gbAdditionalOptions->isVisible() == actionData.toBool(), "An unexpected additional options groupbox visibility state");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkExportCoverage"
void ExportCoverageDialogFiller::checkExportCoverage(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<bool>(), "Can't get an expected checkbox state from the action data");
    QCheckBox *chbExportCoverage = GTWidget::findExactWidget<QCheckBox *>(os, "chbExportCoverage", dialog);
    GT_CHECK(NULL != chbExportCoverage, "An export coverage checkbox wasn't found");
    GT_CHECK(chbExportCoverage->isChecked() == actionData.toBool(), "An unexpected checkbox state");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkExportBasesQuantity"
void ExportCoverageDialogFiller::checkExportBasesQuantity(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<bool>(), "Can't get an expected checkbox state from the action data");
    QCheckBox *chbExportBasesQuantity = GTWidget::findExactWidget<QCheckBox *>(os, "chbExportBasesQuantity", dialog);
    GT_CHECK(NULL != chbExportBasesQuantity, "An export bases count checkbox wasn't found");
    GT_CHECK(chbExportBasesQuantity->isChecked() == actionData.toBool(), "An unexpected checkbox state");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkThreshold"
void ExportCoverageDialogFiller::checkThreshold(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<int>(), "Can't get an expected spinbox value from the action data");
    QSpinBox *sbThreshold = GTWidget::findExactWidget<QSpinBox *>(os, "sbThreshold", dialog);
    GT_CHECK(NULL != sbThreshold, "A threshold spinbox wasn't found");
    GT_CHECK(sbThreshold->value() == actionData.toInt(), "An unexpected spinbox value");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkThresholdBounds"
void ExportCoverageDialogFiller::checkThresholdBounds(const QVariant &actionData) {
    GT_CHECK(actionData.canConvert<QPoint>(), "Can't get a QPoint with expected spinbox bounds values from the action data");
    QSpinBox *sbThreshold = GTWidget::findExactWidget<QSpinBox *>(os, "sbThreshold", dialog);
    GT_CHECK(NULL != sbThreshold, "A threshold spinbox wasn't found");
    const QPoint spinboxBounds = actionData.value<QPoint>();
    GTSpinBox::checkLimits(os, sbThreshold, spinboxBounds.x(), spinboxBounds.y());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "expectMessageBox"
void ExportCoverageDialogFiller::expectMessageBox() {
    CHECK_OP(os, );
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickOk"
void ExportCoverageDialogFiller::clickOk() {
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickCancel"
void ExportCoverageDialogFiller::clickCancel() {
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}   // namespace U2
