/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

CreateElementWithCommandLineToolFiller::CreateElementWithCommandLineToolFiller(HI::GUITestOpStatus& os,
                                                                               const ElementWithCommandLineSettings& settings)
    : Filler(os, "CreateExternalProcessWorkerDialog"),
      settings(settings) {
}

CreateElementWithCommandLineToolFiller::CreateElementWithCommandLineToolFiller(HI::GUITestOpStatus& os, CustomScenario* scenario)
    : Filler(os, "CreateExternalProcessWorkerDialog", scenario) {
}

#define GT_METHOD_NAME "run"
void CreateElementWithCommandLineToolFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);

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

    GTMouseDriver::moveTo(GTTableView::getCellPosition(os, table, static_cast<int>(columnName), row));
    GTMouseDriver::doubleClick();
    GTKeyboardDriver::keySequence(value);
    GTKeyboardDriver::keyClick(Qt::Key_Enter);
}

void CreateElementWithCommandLineToolFiller::processDataType(QTableView* table, int row, const InOutDataType& type) {
    setType(table, row, type.first);
    {
        GTMouseDriver::moveTo(GTTableView::getCellPosition(os, table, static_cast<int>(ColumnName::Value), row));
        GTMouseDriver::doubleClick();

        QComboBox* box = qobject_cast<QComboBox*>(QApplication::focusWidget());
        QString fullValue = formatToArgumentValue(type.second);
        GTComboBox::selectItemByText(os, box, fullValue);
#ifdef Q_OS_WIN
        GTKeyboardDriver::keyClick(Qt::Key_Enter);
#endif
    }
}

void CreateElementWithCommandLineToolFiller::processDataType(QTableView* table, int row, const ParameterDataType& type) {
    setType(table, row, type.first);
    processStringType(table, row, ColumnName::Value, type.second);
}

void CreateElementWithCommandLineToolFiller::processFirstPage(QWidget* dialog) {
    if (!settings.elementName.isEmpty()) {
        GTLineEdit::setText(os, "leName", settings.elementName, dialog);
    }

    switch (settings.tooltype) {
        case CommandLineToolType::ExecutablePath: {
            auto rbCustomTool = GTWidget::findRadioButton(os, "rbCustomTool", dialog);

            GTRadioButton::click(os, rbCustomTool);
            GTLineEdit::setText(os, "leToolPath", settings.tool, dialog);
            break;
        }
        case CommandLineToolType::IntegratedExternalTool: {
            auto rbIntegratedTool = GTWidget::findRadioButton(os, "rbIntegratedTool", dialog);

            GTRadioButton::click(os, rbIntegratedTool);
            if (!settings.tool.isEmpty()) {
                auto cbIntegratedTools = GTWidget::findComboBox(os, "cbIntegratedTools", dialog);

                if (cbIntegratedTools->findText(settings.tool) == -1) {
                    GTComboBox::selectItemByText(os, cbIntegratedTools, "Show all tools");
                    GTKeyboardDriver::keyClick(Qt::Key_Escape);
                }
                GTComboBox::selectItemByText(os, cbIntegratedTools, settings.tool, HI::GTGlobals::UseKeyBoard);
            }
            break;
        }
        default:
            FAIL("Unexpected tool type",);
    }

    // GTGlobals::sleep();
    GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

}

void CreateElementWithCommandLineToolFiller::processSecondPage(QWidget* dialog) {
    auto pbAddInput = GTWidget::findWidget(os, "pbAddInput", dialog);

    auto tvInput = GTWidget::findTableView(os, "tvInput");

    fillTheTable(tvInput, pbAddInput, settings.input);

    // GTGlobals::sleep();
    GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

}

void CreateElementWithCommandLineToolFiller::processThirdPage(QWidget* dialog) {
    auto pbAdd = GTWidget::findWidget(os, "pbAdd", dialog);

    auto tvAttributes = GTWidget::findTableView(os, "tvAttributes");

    fillTheTable(tvAttributes, pbAdd, settings.parameters);

    GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
}

void CreateElementWithCommandLineToolFiller::processFourthPage(QWidget* dialog) {
    auto pbAddOutput = GTWidget::findWidget(os, "pbAddOutput", dialog);

    auto tvOutput = GTWidget::findTableView(os, "tvOutput");

    fillTheTable(tvOutput, pbAddOutput, settings.output);

    // GTGlobals::sleep();
    GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

}

void CreateElementWithCommandLineToolFiller::processFifthPage(QWidget* dialog) {
    auto teCommand = GTWidget::findTextEdit(os, "teCommand", dialog);

    GTTextEdit::setText(os, teCommand, settings.command);

    MessageBoxDialogFiller* msbxFiller = new MessageBoxDialogFiller(os, settings.commandDialogButtonTitle, "You don't use listed parameters in template string");
    GTUtilsDialog::waitForDialog(os, msbxFiller);
    // GTGlobals::sleep();
    GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
    GTGlobals::sleep(1000);
    GTUtilsDialog::removeRunnable(msbxFiller);
}

void CreateElementWithCommandLineToolFiller::processSixthPage(QWidget* dialog) {
    auto teDescription = GTWidget::findTextEdit(os, "teDescription", dialog);

    if (teDescription->toPlainText().isEmpty()) {
        GTTextEdit::setText(os, teDescription, settings.description);
    }

    auto tePrompter = GTWidget::findTextEdit(os, "tePrompter", dialog);

    if (tePrompter->toPlainText().isEmpty()) {
        GTTextEdit::setText(os, tePrompter, settings.prompter);
    }

    GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
}

void CreateElementWithCommandLineToolFiller::processSeventhPage(QWidget* /*dialog*/) {
    MessageBoxDialogFiller* msbxFiller = new MessageBoxDialogFiller(os, settings.summaryDialogButton, "You have changed the structure of the element");
    GTUtilsDialog::waitForDialog(os, msbxFiller);
    GTUtilsWizard::clickButton(os, GTUtilsWizard::Finish);
    GTGlobals::sleep(1000);
    GTUtilsDialog::removeRunnable(msbxFiller);
}

#undef GT_CLASS_NAME

}  // namespace U2
