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

#include "CreateElementWithCommandLineToolFiller.h"
#include <primitives/GTRadioButton.h>
#include <primitives/GTTextEdit.h>

#include <QRadioButton>

#include <U2Core/U2SafePoints.h>

#include "GTUtilsWizard.h"
#include "base_dialogs/MessageBoxFiller.h"

namespace U2 {

#define GT_CLASS_NAME "CreateElementWithCommandLineFiller"

CreateElementWithCommandLineToolFiller::CreateElementWithCommandLineToolFiller(
    const ElementWithCommandLineSettings& settings)
    : Filler("CreateExternalProcessWorkerDialog"),
      settings(settings) {
}

CreateElementWithCommandLineToolFiller::CreateElementWithCommandLineToolFiller(CustomScenario* scenario)
    : Filler("CreateExternalProcessWorkerDialog", scenario) {
}

#define GT_METHOD_NAME "run"
void CreateElementWithCommandLineToolFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    processFirstPage(dialog);

    processSecondPage(dialog);

    processThirdPage(dialog);

    processFourthPage(dialog);

    processFifthPage(dialog);

    processSixthPage(dialog);

    processSeventhPage(dialog);
}
#undef GT_METHOD_NAME

QString CreateElementWithCommandLineToolFiller::dataTypeToString(const InOutType& type) const {
    switch (type) {
        case Alignment:
            return "Alignment";
        case AnnotatedSequence:
            return "Annotated Sequence";
        case Annotations:
            return "Annotations";
        case Sequence:
            return "Sequence";
        case String:
            return "String";
        default:
            return QString();
    }
}

QString CreateElementWithCommandLineToolFiller::dataTypeToString(const ParameterType& type) const {
    switch (type) {
        case Boolean:
            return "Boolean";
        case Integer:
            return "Integer";
        case Double:
            return "Double";
        case ParameterString:
            return "String";
        case InputFileUrl:
            return "Input file URL";
        case InputFolderUrl:
            return "Input folder URL";
        case OutputFileUrl:
            return "Output file URL";
        case OutputFolderUrl:
            return "Output folder URL";
        default:
            return QString();
    }
}

QString CreateElementWithCommandLineToolFiller::formatToArgumentValue(const QString& format) const {
    QString result;
    if ("String data value" != format || "Output URL" != format) {
        result = QString("URL to %1 file with data").arg(format);
    } else {
        result = format;
    }

    return result;
}

void CreateElementWithCommandLineToolFiller::processStringType(QTableView* table, int row, const ColumnName columnName, const QString& value) {
    CHECK(!value.isEmpty(), );

    GTMouseDriver::moveTo(GTTableView::getCellPosition(table, static_cast<int>(columnName), row));
    GTMouseDriver::doubleClick();
    GTKeyboardDriver::keySequence(value);
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
}

void CreateElementWithCommandLineToolFiller::processDataType(QTableView* table, int row, const InOutDataType& type) {
    setType(table, row, type.first);
    {
        GTMouseDriver::moveTo(GTTableView::getCellPosition(table, static_cast<int>(ColumnName::Value), row));
        GTMouseDriver::doubleClick();

        auto box = qobject_cast<QComboBox*>(QApplication::focusWidget());
        QString fullValue = formatToArgumentValue(type.second);
        GTComboBox::selectItemByText(box, fullValue);
        if (isOsWindows()) {
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
        }
    }
}

void CreateElementWithCommandLineToolFiller::processDataType(QTableView* table, int row, const ParameterDataType& type) {
    setType(table, row, type.first);
    processStringType(table, row, ColumnName::Value, type.second);
}

void CreateElementWithCommandLineToolFiller::processFirstPage(QWidget* dialog) {
    if (!settings.elementName.isEmpty()) {
        GTLineEdit::setText("leName", settings.elementName, dialog);
    }

    switch (settings.tooltype) {
        case CommandLineToolType::ExecutablePath: {
            auto rbCustomTool = GTWidget::findRadioButton("rbCustomTool", dialog);

            GTRadioButton::click(rbCustomTool);
            GTLineEdit::setText("leToolPath", settings.tool, dialog);
            break;
        }
        case CommandLineToolType::IntegratedExternalTool: {
            auto rbIntegratedTool = GTWidget::findRadioButton("rbIntegratedTool", dialog);

            GTRadioButton::click(rbIntegratedTool);
            if (!settings.tool.isEmpty()) {
                auto cbIntegratedTools = GTWidget::findComboBox("cbIntegratedTools", dialog);

                if (cbIntegratedTools->findText(settings.tool) == -1) {
                    GTComboBox::selectItemByText(cbIntegratedTools, "Show all tools");
                    GTKeyboardDriver::keyClick(Qt::Key_Escape);
                }
                GTComboBox::selectItemByText(cbIntegratedTools, settings.tool, HI::GTGlobals::UseKeyBoard);
            }
            break;
        }
        default:
            FAIL("Unexpected tool type", );
    }

    // GTGlobals::sleep();
    GTUtilsWizard::clickButton(GTUtilsWizard::Next);
}

void CreateElementWithCommandLineToolFiller::processSecondPage(QWidget* dialog) {
    auto pbAddInput = GTWidget::findWidget("pbAddInput", dialog);

    auto tvInput = GTWidget::findTableView("tvInput");

    fillTheTable(tvInput, pbAddInput, settings.input);

    // GTGlobals::sleep();
    GTUtilsWizard::clickButton(GTUtilsWizard::Next);
}

void CreateElementWithCommandLineToolFiller::processThirdPage(QWidget* dialog) {
    auto pbAdd = GTWidget::findWidget("pbAdd", dialog);

    auto tvAttributes = GTWidget::findTableView("tvAttributes");

    fillTheTable(tvAttributes, pbAdd, settings.parameters);

    GTUtilsWizard::clickButton(GTUtilsWizard::Next);
}

void CreateElementWithCommandLineToolFiller::processFourthPage(QWidget* dialog) {
    auto pbAddOutput = GTWidget::findWidget("pbAddOutput", dialog);

    auto tvOutput = GTWidget::findTableView("tvOutput");

    fillTheTable(tvOutput, pbAddOutput, settings.output);

    // GTGlobals::sleep();
    GTUtilsWizard::clickButton(GTUtilsWizard::Next);
}

void CreateElementWithCommandLineToolFiller::processFifthPage(QWidget* dialog) {
    auto teCommand = GTWidget::findTextEdit("teCommand", dialog);

    GTTextEdit::setText(teCommand, settings.command);

    MessageBoxDialogFiller* msbxFiller = new MessageBoxDialogFiller(settings.commandDialogButtonTitle, "You don't use listed parameters in template string");
    GTUtilsDialog::waitForDialog(msbxFiller);
    // GTGlobals::sleep();
    GTUtilsWizard::clickButton(GTUtilsWizard::Next);
    GTGlobals::sleep(1000);
    GTUtilsDialog::removeRunnable(msbxFiller);
}

void CreateElementWithCommandLineToolFiller::processSixthPage(QWidget* dialog) {
    auto teDescription = GTWidget::findTextEdit("teDescription", dialog);

    if (teDescription->toPlainText().isEmpty()) {
        GTTextEdit::setText(teDescription, settings.description);
    }

    auto tePrompter = GTWidget::findTextEdit("tePrompter", dialog);

    if (tePrompter->toPlainText().isEmpty()) {
        GTTextEdit::setText(tePrompter, settings.prompter);
    }

    GTUtilsWizard::clickButton(GTUtilsWizard::Next);
}

void CreateElementWithCommandLineToolFiller::processSeventhPage(QWidget* /*dialog*/) {
    MessageBoxDialogFiller* msbxFiller = new MessageBoxDialogFiller(settings.summaryDialogButton, "You have changed the structure of the element");
    GTUtilsDialog::waitForDialog(msbxFiller);
    GTUtilsWizard::clickButton(GTUtilsWizard::Finish);
    GTGlobals::sleep(1000);
    GTUtilsDialog::removeRunnable(msbxFiller);
}

#undef GT_CLASS_NAME

}  // namespace U2
