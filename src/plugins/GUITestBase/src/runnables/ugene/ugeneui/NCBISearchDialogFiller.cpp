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

#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QTreeWidget>

#include <primitives/GTComboBox.h>
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTLineEdit.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTextEdit.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include "GTUtilsTaskTreeView.h"
#include "NCBISearchDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/DownloadRemoteFileDialogFiller.h"
#include <U2Core/U2SafePoints.h>

namespace U2 {

#define GT_CLASS_NAME "NcbiSearchDialogFiller"

NcbiSearchDialogFiller::NcbiSearchDialogFiller(HI::GUITestOpStatus &os, const QList<Action> &actions) :
    Filler(os, "SearchGenbankSequenceDialog"),
    dialog(NULL),
    actions(actions)
{
}

#define GT_METHOD_NAME "commonScenario"
void NcbiSearchDialogFiller::commonScenario() {
    dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    foreach (const Action &action, actions) {
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
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setField"
void NcbiSearchDialogFiller::setField(const QVariant &actionData) {
    CHECK_OP(os, );
    const bool canConvert = actionData.canConvert<QPair<int, QString> >();
    GT_CHECK(canConvert, "Can't get the block number and the field name from the action data");
    const QPair<int, QString> value = actionData.value<QPair<int, QString> >();
    QWidget *blockWidget = GTWidget::findWidget(os, "query_block_widget_" + QString::number(value.first), dialog);
    GT_CHECK(NULL != blockWidget, "Block widget is NULL");
    GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "term_box", blockWidget), value.second);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setTerm"
void NcbiSearchDialogFiller::setTerm(const QVariant &actionData) {
    CHECK_OP(os, );
    const bool canConvert = actionData.canConvert<QPair<int, QString> >();
    GT_CHECK(canConvert, "Can't get the block number and the query term from the action data");
    const QPair<int, QString> value = actionData.value<QPair<int, QString> >();
    QWidget *blockWidget = GTWidget::findWidget(os, "query_block_widget_" + QString::number(value.first), dialog);
    GT_CHECK(NULL != blockWidget, "Block widget is NULL");
    GTLineEdit::setText(os, GTWidget::findExactWidget<QLineEdit *>(os, "queryEditLineEdit", blockWidget), value.second);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "addTerm"
void NcbiSearchDialogFiller::addTerm() {
    CHECK_OP(os, );
    GTWidget::click(os, GTWidget::findWidget(os, "add_block_button", dialog));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "removeTerm"
void NcbiSearchDialogFiller::removeTerm(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<int>(), "Can't get the block number from the action data");
    QWidget *blockWidget = GTWidget::findWidget(os, "query_block_widget_" + QString::number(actionData.toInt()), dialog);
    GT_CHECK(NULL != blockWidget, "Block widget is NULL");
    GTWidget::click(os, GTWidget::findWidget(os, "remove_block_button", blockWidget));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setDatabase"
void NcbiSearchDialogFiller::setDatabase(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QString>(), "Can't get the database name from the action data");
    GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "databaseBox", dialog), actionData.toString());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkQuery"
void NcbiSearchDialogFiller::checkQuery(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QString>(), "Can't get the expected query text from the action data");
    QTextEdit *queryEdit = GTWidget::findExactWidget<QTextEdit *>(os, "queryEdit", dialog);
    GT_CHECK(NULL != queryEdit, "Query edit is NULL");
    GT_CHECK(actionData.toString() == queryEdit->toPlainText(), QString("Query text is unexpected: expect '%1', got '%2'").arg(actionData.toString()).arg(queryEdit->toPlainText()));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickResultByNum"
void NcbiSearchDialogFiller::clickResultByNum(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<int>(), "Can't get the result number from the action data");
    QTreeWidget *treeWidget = GTWidget::findExactWidget<QTreeWidget *>(os, "treeWidget", dialog);
    GT_CHECK(NULL != treeWidget, "Tree widget is NULL");
    QList<QTreeWidgetItem*> resultItems = GTTreeWidget::getItems(os, treeWidget);
    GT_CHECK(actionData.toInt() < resultItems.size(), "Result number is out of range");
    GTTreeWidget::click(os, resultItems[actionData.toInt()]);

    GTGlobals::sleep();
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickResultById"
void NcbiSearchDialogFiller::clickResultById(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QString>(), "Can't get the result ID from the action data");
    QTreeWidget *treeWidget = GTWidget::findExactWidget<QTreeWidget *>(os, "treeWidget", dialog);
    GT_CHECK(NULL != treeWidget, "Tree widget is NULL");
    GTTreeWidget::click(os, GTTreeWidget::findItem(os, treeWidget, actionData.toString(), NULL, 0));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickResultByDesc"
void NcbiSearchDialogFiller::clickResultByDesc(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QString>(), "Can't get the result description from the action data");
    QTreeWidget *treeWidget = GTWidget::findExactWidget<QTreeWidget *>(os, "treeWidget", dialog);
    GT_CHECK(NULL != treeWidget, "Tree widget is NULL");
    GTTreeWidget::click(os, GTTreeWidget::findItem(os, treeWidget, actionData.toString(), NULL, 1));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickResultBySize"
void NcbiSearchDialogFiller::clickResultBySize(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<int>(), "Can't get the result sequence size from the action data");
    QTreeWidget *treeWidget = GTWidget::findExactWidget<QTreeWidget *>(os, "treeWidget", dialog);
    GT_CHECK(NULL != treeWidget, "Tree widget is NULL");
    GTTreeWidget::click(os, GTTreeWidget::findItem(os, treeWidget, actionData.toString(), NULL, 2));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectResultsByNum"
void NcbiSearchDialogFiller::selectResultsByNumbers(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QList<int> >(), "Can't get the list of result numbers from the action data");
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    const QList<int> numbers = actionData.value<QList<int> >();
    foreach (int number, numbers) {
        clickResultByNum(number);
    }
    GTKeyboardDriver::keyRelease( Qt::Key_Control);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectResultsByIds"
void NcbiSearchDialogFiller::selectResultsByIds(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QStringList>(), "Can't get the list of result IDs from the action data");
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    foreach (const QString &id, actionData.toStringList()) {
        clickResultById(id);
    }
    GTKeyboardDriver::keyRelease( Qt::Key_Control);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectResultsByDescs"
void NcbiSearchDialogFiller::selectResultsByDescs(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QStringList>(), "Can't get the list of result descriptions from the action data");
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    foreach (const QString &desc, actionData.toStringList()) {
        clickResultByDesc(desc);
    }
    GTKeyboardDriver::keyRelease( Qt::Key_Control);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectResultsBySizes"
void NcbiSearchDialogFiller::selectResultsBySizes(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QList<int> >(), "Can't get the list of result sizes from the action data");
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    const QList<int> sizes = actionData.value<QList<int> >();
    foreach (int size, sizes) {
        clickResultBySize(size);
    }
    GTKeyboardDriver::keyRelease( Qt::Key_Control);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setResultLimit"
void NcbiSearchDialogFiller::setResultLimit(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<int>(), "Can't get the results limit from the action data");
    GTSpinBox::setValue(os, GTWidget::findExactWidget<QSpinBox *>(os, "resultLimitBox", dialog), actionData.toInt());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickSearch"
void NcbiSearchDialogFiller::clickSearch() {
    CHECK_OP(os, );
    GTWidget::click(os, GTWidget::findWidget(os, "searchButton", dialog));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickDownload"
void NcbiSearchDialogFiller::clickDownload(const QVariant &actionData) {
    CHECK_OP(os, );
    GT_CHECK(actionData.canConvert<QList<DownloadRemoteFileDialogFiller::Action> >(), "Can't get actions for the DownloadRemoteFileDialogFiller from the action data");
    GTUtilsDialog::waitForDialog(os, new DownloadRemoteFileDialogFiller(os, actionData.value<QList<U2::DownloadRemoteFileDialogFiller::Action> >()));
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickClose"
void NcbiSearchDialogFiller::clickClose() {
    CHECK_OP(os, );
    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "waitTasksFinish"
void NcbiSearchDialogFiller::waitTasksFinish() {
    GTUtilsTaskTreeView::waitTaskFinished(os);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsDialog::NCBISearchDialogFillerDeprecated"
#define GT_METHOD_NAME "commonScenario"
void NCBISearchDialogFillerDeprecated::commonScenario(){

    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    GTGlobals::sleep(500);

    QLineEdit* queryEditLE = qobject_cast<QLineEdit*>(GTWidget::findWidget(os,"queryEditLineEdit", dialog));
    GT_CHECK(queryEditLE != NULL,"queryEdit line not found");
    GTLineEdit::setText(os, queryEditLE, query);

    GTWidget::click(os, GTWidget::findWidget(os, "searchButton", dialog));
    GTGlobals::sleep(5000);


    if (term != ""){
        QComboBox* term_box = GTWidget::findExactWidget<QComboBox*>(os, "term_box", dialog);
        GTComboBox::setIndexWithText(os, term_box, term);
    }
    if (resultLimit!=-1){
        QSpinBox* resultLimitBox = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "resultLimitBox", dialog));
        GTSpinBox::setValue(os, resultLimitBox, resultLimit, GTGlobals::UseKeyBoard);
        GTWidget::click(os, GTWidget::findWidget(os, "searchButton"));
        GTGlobals::sleep(5000);
        int i = getResultNumber();
        GT_CHECK(i==resultLimit,QString("unexpected number of results. Expected: %1, found: %2").arg(resultLimit).arg(i))
    }

    if (doubleEnter) {
        GTWidget::click(os, GTWidget::findWidget(os, "searchButton"));
        GTGlobals::sleep(5000);

        QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", dialog));
        GT_CHECK(box != NULL, "buttonBox is NULL");
        QPushButton* button = box->button(QDialogButtonBox::Cancel);
        GT_CHECK(button !=NULL, "cancel button is NULL");
        GTWidget::click(os, button);

        return;
    }

    GTUtilsDialog::waitForDialog(os, new RemoteDBDialogFillerDeprecated(os, "", 0, true, false, false, "", GTGlobals::UseMouse, 1));
    QTreeWidget* w = dialog->findChild<QTreeWidget*>("treeWidget");
    GT_CHECK(w, "treeWidget not found");

    GTWidget::click(os, w, Qt::LeftButton, QPoint(10,35));//fast fix, clicking first result

    QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", dialog));
    GT_CHECK(box != NULL, "buttonBox is NULL");
    QPushButton* button = box->button(QDialogButtonBox::Ok);
    GT_CHECK(button !=NULL, "ok button is NULL");
    GTWidget::click(os, button);

    button = box->button(QDialogButtonBox::Cancel);
    GT_CHECK(button !=NULL, "cancel button is NULL");
    GTWidget::click(os, button);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "shownCorrect"
bool NCBISearchDialogFillerDeprecated::shownCorrect(){
    return true;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "NCBISearchDialogFiller::getResultNumber"
int NCBISearchDialogFillerDeprecated::getResultNumber(){
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK_RESULT(dialog, "activeModalWidget is NULL",-1);

    QTreeWidget* w = dialog->findChild<QTreeWidget*>("treeWidget");
    GT_CHECK_RESULT(w, "treeWidget not found",-1);
    return w->topLevelItemCount();
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}
