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

#include <QRadioButton>

#include "CreateElementWithCommandLineToolFiller.h"
#include "GTUtilsWizard.h"

#include <primitives/GTRadioButton.h>
#include <primitives/GTTextEdit.h>

#include <U2Core/U2SafePoints.h>

#include "base_dialogs/MessageBoxFiller.h"

namespace U2 {

#define GT_CLASS_NAME "CreateElementWithCommandLineFiller"

CreateElementWithCommandLineToolFiller::CreateElementWithCommandLineToolFiller(HI::GUITestOpStatus& os,
                                                                               const ElementWithCommandLineSettings& settings)
    : Filler(os, "CreateExternalProcessWorkerDialog"),
      settings(settings)
{}

CreateElementWithCommandLineToolFiller::CreateElementWithCommandLineToolFiller(HI::GUITestOpStatus &os, CustomScenario *scenario)
: Filler(os, "CreateExternalProcessWorkerDialog", scenario)
{

}

#define GT_METHOD_NAME "run"
void CreateElementWithCommandLineToolFiller::commonScenario() {
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QString errorMessage;
    bool firstPageResult = processFirstPage(dialog, errorMessage);
    GT_CHECK(firstPageResult, errorMessage);

    bool secondPageResult = processSecondPage(dialog, errorMessage);
    GT_CHECK(secondPageResult, errorMessage);

    bool thirdPageResult = processThirdPage(dialog, errorMessage);
    GT_CHECK(thirdPageResult, errorMessage);

    bool fourthPageResult = processFourthPage(dialog, errorMessage);
    GT_CHECK(fourthPageResult, errorMessage);

    bool fifthPageResult = processFifthPage(dialog, errorMessage);
    GT_CHECK(fifthPageResult, errorMessage);

    bool sixthPageResult = processSixthPage(dialog, errorMessage);
    GT_CHECK(sixthPageResult, errorMessage);

    bool seventhPageResult = processSeventhPage(dialog, errorMessage);
    GT_CHECK(seventhPageResult, errorMessage);

}
#undef GT_METHOD_NAME

QString CreateElementWithCommandLineToolFiller::dataTypeToString(const InOutType &type) const {
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

QString CreateElementWithCommandLineToolFiller::dataTypeToString(const ParameterType &type) const {
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
    }
    else {
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

void CreateElementWithCommandLineToolFiller::processDataType(QTableView *table, int row, const InOutDataType &type) {
    setType(table, row, type.first);
    {
        GTMouseDriver::moveTo(GTTableView::getCellPosition(os, table, static_cast<int>(ColumnName::Value), row));
        GTMouseDriver::doubleClick();

        QComboBox* box = qobject_cast<QComboBox*>(QApplication::focusWidget());
        QString fullValue = formatToArgumentValue(type.second);
        GTComboBox::setIndexWithText(os, box, fullValue);
#ifdef Q_OS_WIN
        GTKeyboardDriver::keyClick( Qt::Key_Enter);
#endif
    }
}

void CreateElementWithCommandLineToolFiller::processDataType(QTableView *table, int row, const ParameterDataType& type) {
    setType(table, row, type.first);
    processStringType(table, row, ColumnName::Value, type.second);
}

bool CreateElementWithCommandLineToolFiller::processFirstPage(QWidget* dialog, QString& errorMessage) {
    if (!settings.elementName.isEmpty()) {
        QLineEdit* nameEdit = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "leName", dialog));
        CHECK_EXT(nullptr != nameEdit, errorMessage = "leName not found", false);

        GTLineEdit::setText(os, nameEdit, settings.elementName);
    }

    switch (settings.tooltype) {
    case CommandLineToolType::ExecutablePath: {
        QRadioButton* rbCustomTool = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "rbCustomTool", dialog));
        CHECK_EXT(nullptr != rbCustomTool, errorMessage = "rbCustomTool not found", false);

        GTRadioButton::click(os, rbCustomTool);
        QLineEdit* leToolPath = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "leToolPath", dialog));
        CHECK_EXT(nullptr != leToolPath, errorMessage = "leName not found", false);

        GTLineEdit::setText(os, leToolPath, settings.tool);
        break;
    }
    case CommandLineToolType::IntegratedExternalTool: {
        QRadioButton* rbIntegratedTool = qobject_cast<QRadioButton*>(GTWidget::findWidget(os, "rbIntegratedTool", dialog));
        CHECK_EXT(nullptr != rbIntegratedTool, errorMessage = "rbIntegratedTool not found", false);

        GTRadioButton::click(os, rbIntegratedTool);
        if (!settings.tool.isEmpty()) {
            QComboBox* cbIntegratedTools = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "cbIntegratedTools", dialog));
            CHECK_EXT(nullptr != cbIntegratedTools, errorMessage = "cbIntegratedTools not found", false);

            if (cbIntegratedTools->findText(settings.tool) == -1) {
                GTComboBox::setIndexWithText(os, cbIntegratedTools, "Show all tools", false);
                GTKeyboardDriver::keyClick(Qt::Key_Escape);
            }
            GTComboBox::setIndexWithText(os, cbIntegratedTools, settings.tool, false, HI::GTGlobals::UseKeyBoard);
        }
        break;
    }
    default:
        CHECK_EXT(false, errorMessage = "Unexpected tool type", false);
        break;
    }

    //GTGlobals::sleep();
    GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

    return true;
}

bool CreateElementWithCommandLineToolFiller::processSecondPage(QWidget* dialog, QString& errorMessage) {
    QWidget* pbAddInput = GTWidget::findWidget(os, "pbAddInput", dialog);
    CHECK_EXT(nullptr != pbAddInput, errorMessage = "pbAddInput not found", false);

    QTableView* tvInput = qobject_cast<QTableView*>(GTWidget::findWidget(os, "tvInput"));
    CHECK_EXT(nullptr != tvInput, errorMessage = "tvInput not found", false);

    fillTheTable(tvInput, pbAddInput, settings.input);

    //GTGlobals::sleep();
    GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

    return true;
}

bool CreateElementWithCommandLineToolFiller::processThirdPage(QWidget* dialog, QString& errorMessage) {
    QWidget* pbAdd = GTWidget::findWidget(os, "pbAdd", dialog);
    CHECK_EXT(nullptr != pbAdd, errorMessage = "pbAdd not found", false);

    QTableView* tvAttributes = qobject_cast<QTableView*>(GTWidget::findWidget(os, "tvAttributes"));
    CHECK_EXT(nullptr != tvAttributes, errorMessage = "tvAttributes not found", false);

    fillTheTable(tvAttributes, pbAdd, settings.parameters);

    //GTGlobals::sleep();
    GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

    return true;
}

bool CreateElementWithCommandLineToolFiller::processFourthPage(QWidget* dialog, QString& errorMessage) {
    QWidget* pbAddOutput = GTWidget::findWidget(os, "pbAddOutput", dialog);
    CHECK_EXT(nullptr != pbAddOutput, errorMessage = "pbAddOutput not found", false);

    QTableView* tvOutput = qobject_cast<QTableView*>(GTWidget::findWidget(os, "tvOutput"));
    CHECK_EXT(nullptr != tvOutput, errorMessage = "tvOutput not found", false);

    fillTheTable(tvOutput, pbAddOutput, settings.output);

    //GTGlobals::sleep();
    GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

    return true;
}

bool CreateElementWithCommandLineToolFiller::processFifthPage(QWidget* dialog, QString& errorMessage) {
    QTextEdit* teCommand = qobject_cast<QTextEdit*>(GTWidget::findWidget(os, "teCommand", dialog));
    CHECK_EXT(nullptr != teCommand, errorMessage = "teCommand not found", false);

    GTTextEdit::setText(os, teCommand, settings.command);

    MessageBoxDialogFiller* msbxFiller = new MessageBoxDialogFiller(os, settings.commandDialogButtonTitle, "You don't use listed parameters in template string");
    GTUtilsDialog::waitForDialog(os, msbxFiller);
    //GTGlobals::sleep();
    GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);
    GTGlobals::sleep(1000);
    GTUtilsDialog::removeRunnable(msbxFiller);

    return true;
}

bool CreateElementWithCommandLineToolFiller::processSixthPage(QWidget* dialog, QString& errorMessage) {
    QTextEdit* teDescription = qobject_cast<QTextEdit*>(GTWidget::findWidget(os, "teDescription", dialog));
    CHECK_EXT(nullptr != teDescription, errorMessage = "teCommand not found", false);

    if (teDescription->toPlainText().isEmpty()) {
        GTTextEdit::setText(os, teDescription, settings.description);
    }

    QTextEdit* tePrompter = qobject_cast<QTextEdit*>(GTWidget::findWidget(os, "tePrompter", dialog));
    CHECK_EXT(nullptr != tePrompter, errorMessage = "teCommand not found", false);

    if (tePrompter->toPlainText().isEmpty()) {
        GTTextEdit::setText(os, tePrompter, settings.prompter);
    }

    //GTGlobals::sleep();
    GTUtilsWizard::clickButton(os, GTUtilsWizard::Next);

    return true;
}

bool CreateElementWithCommandLineToolFiller::processSeventhPage(QWidget* dialog, QString& errorMessage) {
    MessageBoxDialogFiller* msbxFiller = new MessageBoxDialogFiller(os, settings.summaryDialogButton, "You have changed the structure of the element");
    GTUtilsDialog::waitForDialog(os, msbxFiller);
    //GTGlobals::sleep();
    GTUtilsWizard::clickButton(os, GTUtilsWizard::Finish);
    GTGlobals::sleep(1000);
    GTUtilsDialog::removeRunnable(msbxFiller);

    return true;
}

#undef GT_CLASS_NAME

}
