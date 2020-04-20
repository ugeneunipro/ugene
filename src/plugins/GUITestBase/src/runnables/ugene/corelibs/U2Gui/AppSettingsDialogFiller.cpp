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
#include "AppSettingsDialogFiller.h"

#include <base_dialogs/ColorDialogFiller.h>
#include <primitives/GTWidget.h>
#include <primitives/GTTreeWidget.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTListWidget.h>
#include <utils/GTThread.h>

#include <U2Core/Log.h>

#include <QAbstractButton>
#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QTextBrowser>
#include <QToolButton>
#include <QTreeWidget>

#include <QFile>
#include <QFileInfoList>

namespace U2{
using namespace HI;

#define GT_CLASS_NAME "AppSettingsDialogFiller"
QMap<AppSettingsDialogFiller::Tabs, QString> AppSettingsDialogFiller::initMap(){
        QMap<Tabs, QString> result;
        result.insert(General, "  General");
        result.insert(Resourses, "  Resources");
        result.insert(Network, "  Network");
        result.insert(FileFormat, "  File Format");
        result.insert(Directories, "  Directories");
        result.insert(Logging, "  Logging");
        result.insert(AlignmentColorScheme, "  Alignment Color Scheme");
        result.insert(GenomeAligner, "  Genome Aligner");
        result.insert(WorkflowDesigner, "  Workflow Designer");
        result.insert(ExternalTools, "  External Tools");
        result.insert(OpenCL, "  OpenCL");
        return result;
}

const QMap<AppSettingsDialogFiller::Tabs, QString> AppSettingsDialogFiller::tabMap = initMap();

AppSettingsDialogFiller::AppSettingsDialogFiller(HI::GUITestOpStatus &os, CustomScenario *customScenario) :
    Filler(os, "AppSettingsDialog", customScenario),
    itemStyle(none),
    r(-1),
    g(-1),
    b(-1)
{
}

#define GT_METHOD_NAME "commonScenario"
void AppSettingsDialogFiller::commonScenario(){

    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QTreeWidget* tree = qobject_cast<QTreeWidget*>(GTWidget::findWidget(os,"tree"));
    GT_CHECK(tree, "tree widger not found");

    QList<QTreeWidgetItem*> items = GTTreeWidget::getItems(tree->invisibleRootItem());
    foreach (QTreeWidgetItem* item,items){
        if(item->text(0)=="  Workflow Designer"){
            GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os,item));
            GTMouseDriver::click();
        }
    }
    if(itemStyle!=none){
        QComboBox *styleCombo = qobject_cast<QComboBox*>(GTWidget::findWidget(os,"styleCombo",dialog));
        GTComboBox::setCurrentIndex(os,styleCombo,itemStyle);
    }

    if(r!=-1){
        GTUtilsDialog::waitForDialog(os, new ColorDialogFiller(os,r,g,b));
        QWidget *colorWidget = GTWidget::findWidget(os,"colorWidget",dialog);
        GTWidget::click(os,colorWidget);
    }

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setExternalToolPath"
void AppSettingsDialogFiller::setExternalToolPath(HI::GUITestOpStatus &os, const QString &toolName, const QString &toolPath){
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    openTab(os, ExternalTools);

    QTreeWidget* treeWidget = GTWidget::findExactWidget<QTreeWidget*>(os, "twIntegratedTools", dialog);
    QList<QTreeWidgetItem*> listOfItems = treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);
    bool set = false;
    foreach (QTreeWidgetItem* item, listOfItems) {
        if (item->text(0) == toolName) {
            QWidget *itemWid = treeWidget->itemWidget(item, 1);
            QLineEdit *lineEdit = itemWid->findChild<QLineEdit*>("PathLineEdit");
            treeWidget->scrollToItem(item);
            GTThread::waitForMainThread();
            GTLineEdit::setText(os, lineEdit, toolPath);
            GTTreeWidget::click(os, item, 0);
            set = true;
        }
    }
    GT_CHECK(set, "tool " + toolName + " not found in tree view");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setExternalToolPath"
void AppSettingsDialogFiller::setExternalToolPath(HI::GUITestOpStatus &os, const QString& toolName, const QString& path, const QString& name) {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    openTab(os, ExternalTools);

    QTreeWidget* treeWidget = GTWidget::findExactWidget<QTreeWidget*>(os, "twIntegratedTools", dialog);
    QList<QTreeWidgetItem*> listOfItems = treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);
    bool set = false;
    foreach(QTreeWidgetItem* item, listOfItems) {
        if (item->text(0) == toolName) {
            treeWidget->scrollToItem(item);
            GTThread::waitForMainThread();
            GTFileDialogUtils *ob = new GTFileDialogUtils(os, path, name, (GTFileDialogUtils::Button)GTFileDialog::Open, GTGlobals::UseMouse);
            GTUtilsDialog::waitForDialog(os, ob);

            QWidget *itemWid = treeWidget->itemWidget(item, 1);
            GT_CHECK(itemWid, "itemWid is NULL");

            QLineEdit *lineEdit = itemWid->findChild<QLineEdit*>("PathLineEdit");
            GT_CHECK(lineEdit, "lineEdit is NULL");

            QToolButton* clearToolPathButton = lineEdit->parentWidget()->findChild<QToolButton*>("ResetExternalTool");
            GT_CHECK(clearToolPathButton, "clearToolPathButton is NULL");

            GTWidget::click(os, clearToolPathButton);
            set = true;
        }
    }
    GT_CHECK(set, "tool " + toolName + " not found in tree view");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExternalToolPath"
QString AppSettingsDialogFiller::getExternalToolPath(HI::GUITestOpStatus &os, const QString &toolName){
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK_RESULT(dialog, "activeModalWidget is NULL", "");

    openTab(os, ExternalTools);

    QTreeWidget* treeWidget = GTWidget::findExactWidget<QTreeWidget*>(os, "twIntegratedTools", dialog);
    QList<QTreeWidgetItem*> listOfItems = treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);

    foreach (QTreeWidgetItem* item, listOfItems){
        if(item->text(0) == toolName){
            QWidget* itemWid = treeWidget->itemWidget(item, 1);
            QLineEdit* lineEdit = itemWid->findChild<QLineEdit*>("PathLineEdit");
            return lineEdit->text();
        }
    }
    return "";
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isExternalToolValid"
bool AppSettingsDialogFiller::isExternalToolValid(HI::GUITestOpStatus &os, const QString &toolName){
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK_RESULT(dialog, "activeModalWidget is NULL", false);

    openTab(os, ExternalTools);

    QTreeWidget* treeWidget = GTWidget::findExactWidget<QTreeWidget*>(os, "twIntegratedTools", dialog);
    QList<QTreeWidgetItem*> listOfItems = treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);
    foreach (QTreeWidgetItem* item, listOfItems){
        if(item->text(0) == toolName){
            GTTreeWidget::click(os, item);
            GTMouseDriver::doubleClick();
            QTextBrowser *descriptionTextBrowser = GTWidget::findExactWidget<QTextBrowser*>(os, "descriptionTextBrowser", dialog);
            return descriptionTextBrowser->toPlainText().contains("Version:");
        }
    }
    GT_CHECK_RESULT(false, "external tool " + toolName + " not found in tree view", false);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clearToolPath"
void AppSettingsDialogFiller::clearToolPath(HI::GUITestOpStatus &os, const QString &toolName) {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    openTab(os, ExternalTools);

    QTreeWidget* treeWidget = GTWidget::findExactWidget<QTreeWidget*>(os, "twIntegratedTools", dialog);
    QList<QTreeWidgetItem*> listOfItems = treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);
    foreach (QTreeWidgetItem* item, listOfItems){
        if(item->text(0) == toolName){
            QWidget* itemWid = treeWidget->itemWidget(item, 1);
            QToolButton* clearPathButton = itemWid->findChild<QToolButton*>("ClearToolPathButton");
            CHECK_SET_ERR(clearPathButton != NULL, "Clear path button not found");
            treeWidget->scrollToItem(item);
            GTThread::waitForMainThread();
            if (clearPathButton->isEnabled()) {
                GTWidget::click(os, clearPathButton);
            }
        }
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isToolDescriptionContainsString"
bool AppSettingsDialogFiller::isToolDescriptionContainsString(HI::GUITestOpStatus &os, const QString& toolName, const QString& checkIfContains) {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK_RESULT(dialog, "activeModalWidget is NULL", false);

    clickOnTool(os, toolName);

    QTextBrowser* textBrowser = GTWidget::findExactWidget<QTextBrowser*>(os, "descriptionTextBrowser", dialog);
    GT_CHECK_RESULT(textBrowser, "textBrowser is NULL", false);

    QString plainText = textBrowser->toPlainText();
    return plainText.contains(checkIfContains);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setTemporaryDirPath"
void AppSettingsDialogFiller::setTemporaryDirPath(GUITestOpStatus &os, const QString &path) {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(NULL != dialog, "activeModalWidget is NULL");

    openTab(os, Directories);

    GTLineEdit::setText(os, "tmpDirPathEdit", path, dialog);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setDocumentsDirPath"
void AppSettingsDialogFiller::setDocumentsDirPath(GUITestOpStatus &os, const QString &path) {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(NULL != dialog, "activeModalWidget is NULL");

    openTab(os, Directories);

    GTLineEdit::setText(os, "documentsDirectoryEdit", path, dialog);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setWorkflowOutputDirPath"
void AppSettingsDialogFiller::setWorkflowOutputDirPath(GUITestOpStatus &os, const QString &path) {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(nullptr != dialog, "activeModalWidget is nullptr");

    openTab(os, WorkflowDesigner);

    GTLineEdit::setText(os, "workflowOutputEdit", path, dialog);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "OpenTab"
void AppSettingsDialogFiller::openTab(HI::GUITestOpStatus &os, Tabs tab){
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QString itemText = tabMap.value(tab);
    GT_CHECK(!itemText.isEmpty(), "tree element for item not found");

    QTreeWidget* mainTree = GTWidget::findExactWidget<QTreeWidget *>(os, "tree");

    if(mainTree->selectedItems().first()->text(0) != itemText){
        GTTreeWidget::click(os, GTTreeWidget::findItem(os, mainTree, itemText));
    }
    GTGlobals::sleep(300);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickOnTool"
void AppSettingsDialogFiller::clickOnTool(HI::GUITestOpStatus &os, const QString& toolName) {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK_RESULT(dialog, "activeModalWidget is NULL", );

    openTab(os, ExternalTools);

    QTreeWidget* treeWidget = GTWidget::findExactWidget<QTreeWidget*>(os, "twIntegratedTools", dialog);
    QList<QTreeWidgetItem*> listOfItems = treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);
    foreach(QTreeWidgetItem* item, listOfItems){
        if (item->text(0) == toolName){
            GTTreeWidget::click(os, item);
            return;
        }
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setExternalToolsDir"
void AppSettingsDialogFiller::setExternalToolsDir(HI::GUITestOpStatus &os, const QString& dirPath) {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    openTab(os, ExternalTools);

    QWidget* selectExToolsDirButton = GTWidget::findWidget(os, "selectToolPackButton", dialog);
    GT_CHECK(selectExToolsDirButton, "selectToolPackButton not found");
    while (!selectExToolsDirButton->isEnabled()) {
        uiLog.trace("selectToolPackButton is disabled");
        GTGlobals::sleep(100);
    }
    
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, dirPath, "", GTFileDialogUtils::Choose));
    GTWidget::click(os, selectExToolsDirButton);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

NewColorSchemeCreator::NewColorSchemeCreator(HI::GUITestOpStatus &_os, QString _schemeName, alphabet _al, Action _act, bool cancel)
    : Filler(_os, "AppSettingsDialog"), schemeName(_schemeName), al(_al), act(_act), cancel(cancel)
{

}

NewColorSchemeCreator::NewColorSchemeCreator(HI::GUITestOpStatus &os, CustomScenario *c)
    : Filler(os, "AppSettingsDialog", c),
      al(nucl),
      act(Create),
      cancel(true)
{

}

#define GT_CLASS_NAME "NewColorSchemeCreator"
#define GT_METHOD_NAME "commonScenario"
void NewColorSchemeCreator::commonScenario() {
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QTreeWidget* tree = qobject_cast<QTreeWidget*>(GTWidget::findWidget(os,"tree"));
    GT_CHECK(tree, "tree widger not found");

    QList<QTreeWidgetItem*> items = GTTreeWidget::getItems(tree->invisibleRootItem());
    foreach (QTreeWidgetItem* item,items){
        if(item->text(0)=="  Alignment Color Scheme"){
            GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(os,item));
            GTMouseDriver::click();
        }
    }

    switch (act) {
    case Delete: {
        QListWidget* colorSchemas = qobject_cast<QListWidget*>(GTWidget::findWidget(os, "colorSchemas", dialog));
        GT_CHECK(colorSchemas != NULL, "colorSchemas list widget not found");
        GTListWidget::click(os, colorSchemas, schemeName);
        GTGlobals::sleep(500);

        QWidget* deleteSchemaButton = GTWidget::findWidget(os, "deleteSchemaButton",dialog);
        GT_CHECK (deleteSchemaButton, "deleteSchemaButton not found");
        while(!deleteSchemaButton->isEnabled()){
            uiLog.trace("deleteSchemaButton is disabled");
            GTGlobals::sleep(100);
        }
        GTWidget::click(os, deleteSchemaButton);
        break;
    }
    case Create: {
        QWidget* addSchemaButton = GTWidget::findWidget(os, "addSchemaButton");
        GT_CHECK (addSchemaButton, "addSchemaButton not found");

        GTUtilsDialog::waitForDialog(os, new CreateAlignmentColorSchemeDialogFiller(os, schemeName, al));
        GTWidget::click(os, addSchemaButton);
        break;
    }
    case Change: {
        GTListWidget::click(os, GTWidget::findExactWidget<QListWidget *>(os, "colorSchemas", dialog), schemeName);

        class Scenario : public CustomScenario {
        public:
            void run(HI::GUITestOpStatus &os) {
                QWidget *dialog = QApplication::activeModalWidget();
                GT_CHECK(NULL != dialog, "Active modal widget is NULL");
                GTUtilsDialog::waitForDialog(os, new ColorDialogFiller(os, 255, 0, 0));
                GTWidget::click(os, GTWidget::findWidget(os, "alphabetColorsFrame", dialog), Qt::LeftButton, QPoint(5, 5));

                GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
            }
        };

        GTUtilsDialog::waitForDialog(os, new ColorSchemeDialogFiller(os, new Scenario));
        GTWidget::click(os, GTWidget::findWidget(os, "changeSchemaButton", dialog));
    }
    }

    GTUtilsDialog::clickButtonBox(os, dialog, cancel ? QDialogButtonBox::Cancel : QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "CreateAlignmentColorSchemeDialogFiller"
#define GT_METHOD_NAME "commonScenario"
void CreateAlignmentColorSchemeDialogFiller::commonScenario(){
    GTGlobals::sleep( 500 );
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QWidget* w = GTWidget::findWidget(os, "schemeName",dialog);
    QLineEdit* schemeNameLine = qobject_cast<QLineEdit*>(w);
    GT_CHECK(schemeNameLine, "schemeName lineEdit not found ");

    GTLineEdit::setText(os,schemeNameLine, schemeName);

    QComboBox *alphabetComboBox = qobject_cast<QComboBox*>(GTWidget::findWidget(os,"alphabetComboBox",dialog));
    GT_CHECK(alphabetComboBox, "alphabetComboBox lineEdit not found ");

    GTComboBox::setCurrentIndex(os, alphabetComboBox, al);
    GTGlobals::sleep( 500 );

    GTUtilsDialog::waitForDialog(os, new ColorSchemeDialogFiller(os));

    QDialogButtonBox* box = qobject_cast<QDialogButtonBox*>(GTWidget::findWidget(os, "buttonBox", dialog));
    GT_CHECK(box != NULL, "buttonBox is NULL");
    QPushButton* button = box->button(QDialogButtonBox::Ok);
    GT_CHECK(button !=NULL, "ok button is NULL");
    GTWidget::click(os, button);
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "ColorSchemeDialogFiller"
#define GT_METHOD_NAME "commonScenario"
void ColorSchemeDialogFiller::commonScenario(){
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    QList<QAbstractButton* > list = dialog->findChildren<QAbstractButton*>();
    foreach(QAbstractButton* b, list){
        if (b->text().contains("ok",Qt::CaseInsensitive)){
            GTWidget::click(os, b);
            return;
        }
    }
    GTKeyboardDriver::keyClick( Qt::Key_Enter);//if ok button not found
}
#undef GT_METHOD_NAME
#undef GT_CLASS_NAME
//QDialogButtonBox *buttonBox;
}
