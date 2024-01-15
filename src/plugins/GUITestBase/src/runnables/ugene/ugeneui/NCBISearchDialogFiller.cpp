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

#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTextEdit.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QTreeWidget>

#include <U2Core/U2SafePoints.h>

#include "GTUtilsTaskTreeView.h"
#include "NCBISearchDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/DownloadRemoteFileDialogFiller.h"

namespace U2 {

#define GT_CLASS_NAME "NcbiSearchDialogFiller"

NcbiSearchDialogFiller::NcbiSearchDialogFiller(const QList<Action>& actions)
    : Filler("SearchGenbankSequenceDialog"),
      actions(actions) {
}

void NcbiSearchDialogFiller::commonScenario() {
    dialog = GTWidget::getActiveModalWidget();

    for (const Action& action : qAsConst(actions)) {
        switch (action.first) {
            case SetField:
                setField(action.second);
                break;
            case SetTerm:
                setTerm(action.second);
                break;
            case AddTerm:
                addTerm();
                break;
            case RemoveTerm:
                removeTerm(action.second);
                break;
            case SetDatabase:
                setDatabase(action.second);
                break;
            case CheckQuery:
                checkQuery(action.second);
                break;
            case ClickResultByNum:
                clickResultByNum(action.second);
                break;
            case ClickResultById:
                clickResultById(action.second);
                break;
            case ClickResultByDesc:
                clickResultByDesc(action.second);
                break;
            case ClickResultBySize:
                clickResultBySize(action.second);
                break;
            case SelectResultsByNumbers:
                selectResultsByNumbers(action.second);
                break;
            case SelectResultsByIds:
                selectResultsByIds(action.second);
                break;
            case SelectResultsByDescs:
                selectResultsByDescs(action.second);
                break;
            case SelectResultsBySizes:
                selectResultsBySizes(action.second);
                break;
            case SetResultLimit:
                setResultLimit(action.second);
                break;
            case ClickSearch:
                clickSearch();
                break;
            case ClickDownload:
                clickDownload(action.second);
                break;
            case ClickClose:
                clickClose();
                break;
            case WaitTasksFinish:
                waitTasksFinish();
                break;
        }
    }
}

void NcbiSearchDialogFiller::setField(const QVariant& actionData) {
    const bool canConvert = actionData.canConvert<QPair<int, QString>>();
    GT_CHECK(canConvert, "Can't get the block number and the field name from the action data");
    const QPair<int, QString> value = actionData.value<QPair<int, QString>>();
    auto blockWidget = GTWidget::findWidget("query_block_widget_" + QString::number(value.first), dialog);
    GTComboBox::selectItemByText(GTWidget::findComboBox("term_box", blockWidget), value.second);
}

void NcbiSearchDialogFiller::setTerm(const QVariant& actionData) {
    const bool canConvert = actionData.canConvert<QPair<int, QString>>();
    GT_CHECK(canConvert, "Can't get the block number and the query term from the action data");
    const QPair<int, QString> value = actionData.value<QPair<int, QString>>();
    auto blockWidget = GTWidget::findWidget("query_block_widget_" + QString::number(value.first), dialog);
    GTLineEdit::setText("queryEditLineEdit", value.second, blockWidget);
}

void NcbiSearchDialogFiller::addTerm() {
    GTWidget::click(GTWidget::findWidget("add_block_button", dialog));
}

void NcbiSearchDialogFiller::removeTerm(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<int>(), "Can't get the block number from the action data");
    QWidget* blockWidget = GTWidget::findWidget("query_block_widget_" + QString::number(actionData.toInt()), dialog);
    GT_CHECK(blockWidget != nullptr, "Block widget is NULL");
    GTWidget::click(GTWidget::findWidget("remove_block_button", blockWidget));
}

void NcbiSearchDialogFiller::setDatabase(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get the database name from the action data");
    GTComboBox::selectItemByText(GTWidget::findComboBox("databaseBox", dialog), actionData.toString());
}

void NcbiSearchDialogFiller::checkQuery(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get the expected query text from the action data");
    auto queryEdit = GTWidget::findTextEdit("queryEdit", dialog);
    GT_CHECK(actionData.toString() == queryEdit->toPlainText(), QString("Query text is unexpected: expect '%1', got '%2'").arg(actionData.toString()).arg(queryEdit->toPlainText()));
}

void NcbiSearchDialogFiller::clickResultByNum(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<int>(), "Can't get the result number from the action data");
    auto treeWidget = GTWidget::findTreeWidget("treeWidget", dialog);
    int itemIndex = actionData.toInt();
    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(treeWidget);
    GT_CHECK(itemIndex >= 0 && itemIndex < treeItems.size(), "Result number is out of range");
    GTTreeWidget::click(treeItems[itemIndex]);
}

void NcbiSearchDialogFiller::clickResultById(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get the result ID from the action data");
    auto treeWidget = GTWidget::findTreeWidget("treeWidget", dialog);
    GTTreeWidget::click(GTTreeWidget::findItem(treeWidget, actionData.toString(), nullptr, 0));
}

void NcbiSearchDialogFiller::clickResultByDesc(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QString>(), "Can't get the result description from the action data");
    auto treeWidget = GTWidget::findTreeWidget("treeWidget", dialog);
    GTTreeWidget::click(GTTreeWidget::findItem(treeWidget, actionData.toString(), nullptr, 1));
}

void NcbiSearchDialogFiller::clickResultBySize(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<int>(), "Can't get the result sequence size from the action data");
    auto treeWidget = GTWidget::findTreeWidget("treeWidget", dialog);
    GTTreeWidget::click(GTTreeWidget::findItem(treeWidget, actionData.toString(), nullptr, 2));
}

void NcbiSearchDialogFiller::selectResultsByNumbers(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QList<int>>(), "Can't get the list of result numbers from the action data");
    QList<int> numbers = actionData.value<QList<int>>();
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    for (int number : qAsConst(numbers)) {
        clickResultByNum(number);
    }
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
}

void NcbiSearchDialogFiller::selectResultsByIds(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QStringList>(), "Can't get the list of result IDs from the action data");
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    foreach (const QString& id, actionData.toStringList()) {
        clickResultById(id);
    }
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
}

void NcbiSearchDialogFiller::selectResultsByDescs(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QStringList>(), "Can't get the list of result descriptions from the action data");
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    foreach (const QString& desc, actionData.toStringList()) {
        clickResultByDesc(desc);
    }
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
}

void NcbiSearchDialogFiller::selectResultsBySizes(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QList<int>>(), "Can't get the list of result sizes from the action data");
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    const QList<int> sizes = actionData.value<QList<int>>();
    foreach (int size, sizes) {
        clickResultBySize(size);
    }
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
}

void NcbiSearchDialogFiller::setResultLimit(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<int>(), "Can't get the results limit from the action data");
    GTSpinBox::setValue(GTWidget::findSpinBox("resultLimitBox", dialog), actionData.toInt());
}

void NcbiSearchDialogFiller::clickSearch() {
    GTWidget::click(GTWidget::findWidget("searchButton", dialog));
}

void NcbiSearchDialogFiller::clickDownload(const QVariant& actionData) {
    GT_CHECK(actionData.canConvert<QList<DownloadRemoteFileDialogFiller::Action>>(), "Can't get actions for the DownloadRemoteFileDialogFiller from the action data");
    GTUtilsDialog::waitForDialog(new DownloadRemoteFileDialogFiller(actionData.value<QList<U2::DownloadRemoteFileDialogFiller::Action>>()));
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}

void NcbiSearchDialogFiller::clickClose() {
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
}

void NcbiSearchDialogFiller::waitTasksFinish() {
    GTUtilsTaskTreeView::waitTaskFinished();
}

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::NCBISearchDialogSimpleFiller"

NCBISearchDialogSimpleFiller::NCBISearchDialogSimpleFiller(
    const QString& _query,
    bool _doubleEnter,
    int _resultLimit,
    const QString& _term,
    const QString& _resultToLoad)
    : Filler("SearchGenbankSequenceDialog"),
      query(_query),
      doubleEnter(_doubleEnter),
      resultLimit(_resultLimit),
      term(_term), resultToLoad(_resultToLoad) {
}

void NCBISearchDialogSimpleFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    GTLineEdit::setText("queryEditLineEdit", query, dialog);

    if (term != "") {
        GTComboBox::selectItemByText(GTWidget::findComboBox("term_box", dialog), term);
    }
    if (resultLimit != -1) {
        GTSpinBox::setValue(GTWidget::findSpinBox("resultLimitBox", dialog), resultLimit, GTGlobals::UseKeyBoard);
    }

    // Run search
    GTWidget::click(GTWidget::findWidget("searchButton", dialog));
    GTUtilsTaskTreeView::waitTaskFinished();

    if (doubleEnter) {
        GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
        return;
    }

    if (resultLimit != -1) {
        int resultCount = getResultNumber();
        GT_CHECK(resultCount == resultLimit, QString("unexpected number of results. Expected: %1, found: %2").arg(resultLimit).arg(resultCount))
    }

    auto resultsTreeWidget = GTWidget::findTreeWidget("treeWidget", dialog);
    GTUtilsDialog::waitForDialog(new RemoteDBDialogFillerDeprecated("", 0, true, false, false, "", GTGlobals::UseMouse, 1));
    if (resultToLoad.isEmpty()) {
        // Click the first result. Original behavior.
        GTWidget::click(resultsTreeWidget, Qt::LeftButton, QPoint(10, 35));
    } else {
        auto item = GTTreeWidget::findItem(resultsTreeWidget, resultToLoad, nullptr, 0, GTGlobals::FindOptions(true, Qt::MatchContains));
        GTTreeWidget::click(item);
    }

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Cancel);
}

int NCBISearchDialogSimpleFiller::getResultNumber() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    auto w = GTWidget::findTreeWidget("treeWidget", dialog);
    return w->topLevelItemCount();
}

#undef GT_CLASS_NAME

}  // namespace U2
