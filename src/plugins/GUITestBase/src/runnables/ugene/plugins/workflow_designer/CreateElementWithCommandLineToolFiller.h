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

#ifndef _U2_GT_CREATE_ELEMENT_WITH_COMMAND_LINE_TOOL_FILLER_H_
#define _U2_GT_CREATE_ELEMENT_WITH_COMMAND_LINE_TOOL_FILLER_H_

#include <QApplication>
#include <QMessageBox>
#include <QTableView>

#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTTableView.h>
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>
#include <utils/GTUtilsDialog.h>

namespace U2 {
using namespace HI;

class CreateElementWithCommandLineToolFiller : public Filler {
public:
    template <typename DataType>
    class Data {
    public:
        Data(QString _displayName, DataType type, QString desc = QString(), QString _argumentName = QString())
            : displayName(_displayName),
              argumentName(_argumentName),
              type(type),
              description(desc) {}
        QString displayName;
        QString argumentName;
        DataType type;
        QString description;
    };

    enum class CommandLineToolType {
        ExecutablePath,
        IntegratedExternalTool
    };

    enum class ColumnName {
        DisplayName = 0,
        ArgumentName,
        Type,
        Value,
        Description
    };

    enum InOutType {
        Alignment,
        AnnotatedSequence,
        Annotations,
        Sequence,
        String
    };
    typedef QPair<InOutType, QString> InOutDataType;        // type / argument value
    typedef Data<InOutDataType> InOutData;

    enum ParameterType {
        Boolean,
        Integer,
        Double,
        ParameterString,
        InputFileUrl,
        InputFolderUrl,
        OutputFileUrl,
        OutputFolderUrl
    };
    typedef QPair<ParameterType, QString> ParameterDataType;        // type / default value
    typedef Data<ParameterDataType> ParameterData;

    struct ElementWithCommandLineSettings {
        ElementWithCommandLineSettings() : tooltype(CommandLineToolType::IntegratedExternalTool),
                                           commandDialogButtonTitle("Continue"),
                                           summaryDialogButton(QMessageBox::Yes) {}

        //1th page
        QString                         elementName;
        CommandLineToolType             tooltype;
        QString                         tool;

        //2th page
        QList<InOutData>                input;

        //3th page
        QList<ParameterData>            parameters;

        //4th page
        QList<InOutData>                output;

        //5th page
        QString                         command;
        QString                         commandDialogButtonTitle;

        //6th page
        QString                         description;
        QString                         prompter;

        //7th page
        QMessageBox::Button             summaryDialogButton;
    };

public:
    CreateElementWithCommandLineToolFiller(HI::GUITestOpStatus& os,
                                           const ElementWithCommandLineSettings& settings);
    CreateElementWithCommandLineToolFiller(HI::GUITestOpStatus &os, CustomScenario *scenario);
    void commonScenario();
private:
    QString dataTypeToString(const InOutType &type) const;
    QString dataTypeToString(const ParameterType &type) const;
    QString formatToArgumentValue(const QString& format) const;

    void processStringType(QTableView* table, int row, const ColumnName columnName, const QString& value);
    void processDataType(QTableView *table, int row, const InOutDataType &type);
    void processDataType(QTableView *table, int row, const ParameterDataType &type);

    bool processFirstPage(QWidget* dialog, QString& errorMessage);
    bool processSecondPage(QWidget* dialog, QString& errorMessage);
    bool processThirdPage(QWidget* dialog, QString& errorMessage);
    bool processFourthPage(QWidget* dialog, QString& errorMessage);
    bool processFifthPage(QWidget* dialog, QString& errorMessage);
    bool processSixthPage(QWidget* dialog, QString& errorMessage);
    bool processSeventhPage(QWidget* dialog, QString& errorMessage);

    template <typename DataType>
    void setType(QTableView *table, int row, const DataType &type) {
        GTMouseDriver::moveTo(GTTableView::getCellPosition(os, table, static_cast<int>(ColumnName::Type), row));
        GTMouseDriver::doubleClick();
        GTThread::waitForMainThread();

        QComboBox* box = qobject_cast<QComboBox*>(QApplication::focusWidget());
        QString dataType = dataTypeToString(type);
        GTComboBox::setIndexWithText(os, box, dataType);
#ifdef Q_OS_WIN
        GTKeyboardDriver::keyClick( Qt::Key_Enter);
#endif
    }

    template <typename DataType>
    void fillTheTable(QTableView* table,
                      QWidget* addRowButton,
                      QList< Data<DataType> >& rowItems) {
        QAbstractItemModel* model = table->model();
        int row = model->rowCount();

        foreach (const Data<DataType> &rowData, rowItems) {
            GTWidget::click(os, addRowButton);
            table->scrollTo(model->index(row, static_cast<int>(ColumnName::DisplayName)));

            processStringType(table, row, ColumnName::DisplayName, rowData.displayName);

            if (!rowData.argumentName.isEmpty()) {
                processStringType(table, row, ColumnName::ArgumentName, rowData.argumentName);
            }

            processDataType(table, row, rowData.type);

            if (!rowData.description.isEmpty()) {
                processStringType(table, row, ColumnName::Description, rowData.description);
            }
            row++;
        }
    }

    ElementWithCommandLineSettings  settings;
};

} // namespace

#endif // _U2_GT_CREATE_ELEMENT_WITH_COMMAND_LINE_TOOL_FILLER_H_
