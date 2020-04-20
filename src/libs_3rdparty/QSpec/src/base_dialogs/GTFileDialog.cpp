/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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

#include "base_dialogs/GTFileDialog.h"
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include "GTGlobals.h"
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include "primitives/GTMenu.h"
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>

#include <QApplication>
#include <QComboBox>
#include <QFileSystemModel>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QTreeView>

#define FILE_NAME_LINE_EDIT "fileNameEdit"
#define CURRENT_FODLER_COMBO_BOX "lookInCombo"

namespace HI {
#define GT_CLASS_NAME "GTFileDialogUtils"

GTFileDialogUtils::GTFileDialogUtils(GUITestOpStatus &_os, const QString &_path, const QString &_fileName,
                                     Button _button, GTGlobals::UseMethod _method, TextInput textInput) :
    Filler(_os, "QFileDialog"),
    fileName(_fileName),
    button(_button),
    method(_method),
    textInput(textInput)
{
    init(_path + "/" + fileName);
}

GTFileDialogUtils::GTFileDialogUtils(GUITestOpStatus &os, const QString &filePath, GTGlobals::UseMethod method, Button b, TextInput textInput) :
    Filler(os, "QFileDialog"),
    button(b),
    method(method),
    textInput(textInput)
{
    init(filePath);
}

GTFileDialogUtils::GTFileDialogUtils(GUITestOpStatus &os, CustomScenario *customScenario)
    : Filler(os, "QFileDialog", customScenario),
      fileDialog(NULL),
      button(Open),
      method(GTGlobals::UseMouse),
      textInput(Typing)
{

}

#define GT_METHOD_NAME "commonScenario"
void GTFileDialogUtils::commonScenario()
{
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog != NULL && QString(dialog->metaObject()->className()) == "QFileDialog",
                   "file dialog not found");

    fileDialog = dialog;
    GTGlobals::sleep(300);
    const bool dirWasChanged = setPath();
    GTGlobals::sleep(300);
    if(button == Choose){
        clickButton(button);
        return;
    }

    if (dirWasChanged) {
        clickButton(Open);
        GTGlobals::sleep(300);
    }

    if(button == Save){//saving file
        setName();
        clickButton(button);
        return;
    }

    //opening file or getting size
    GTGlobals::sleep(300);
    setViewMode(Detail);
    GTGlobals::sleep(300);
    selectFile();
    GTGlobals::sleep(300);
    if(method == GTGlobals::UseKey){
        GTKeyboardDriver::keyClick( Qt::Key_Enter);

    }else{
        clickButton(button);
    }
    GTGlobals::sleep(500);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "init"
void GTFileDialogUtils::init(const QString &filePath) {
    const QFileInfo fileInfo(filePath);
    path = fileInfo.absoluteDir().absolutePath();
    fileName = fileInfo.fileName();
    if (!path.endsWith('/')) {
        path += '/';
    }
}
#undef GT_METHOD_NAME

GTFileDialogUtils_list::GTFileDialogUtils_list(GUITestOpStatus &_os, const QString &_path, const QStringList &fileNames) :
    GTFileDialogUtils(_os,_path, "", Open, GTGlobals::UseMouse),
    fileNamesList(fileNames)
{
//    path = QDir::cleanPath(QDir::currentPath() + "/" + _path);
//    if (path.at(path.count() - 1) != '/') {
//        path += '/';
//    }
    foreach (const QString &name, fileNames) {
        filePaths << _path + "/" + name;
    }
}

GTFileDialogUtils_list::GTFileDialogUtils_list(GUITestOpStatus &os, const QStringList &filePaths) :
    GTFileDialogUtils(os, "", "", Open, GTGlobals::UseMouse),
    filePaths(filePaths)
{

}

#define GT_METHOD_NAME "commonScenario"
void GTFileDialogUtils_list::commonScenario() {
    QWidget *dialog = QApplication::activeModalWidget();
    GT_CHECK(NULL != dialog && QString(dialog->metaObject()->className()) == "QFileDialog", "file dialog not found");

    setNameList(os, filePaths, dialog);
    GTGlobals::sleep(200);

    GTKeyboardDriver::keyClick(Qt::Key_Enter);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setNameList"
void GTFileDialogUtils_list::setNameList(GUITestOpStatus &os, const QStringList &nameList, QWidget *parent) {
    QString str;
    foreach (QString name, nameList){
        if (QFileInfo(name).isRelative()) {
            name = QApplication::applicationDirPath() + "/" + name;
        }
        str.append('\"' + name + "\" ");
    }

    QLineEdit* fileEdit = GTWidget::findExactWidget<QLineEdit *>(os, FILE_NAME_LINE_EDIT, parent);
    GTLineEdit::setText(os, fileEdit, str, false, true);
}
#undef GT_METHOD_NAME

void GTFileDialogUtils_list::selectFile(){
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    foreach(QString name, fileNamesList){
        GTFileDialogUtils::fileName = name;
        GTFileDialogUtils::selectFile();
    }
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
}

void GTFileDialogUtils::openFileDialog() {
    switch(method) {
    case GTGlobals::UseMouse:
        GTMenu::clickMainMenuItem(os, QStringList() << "File" << "Open...");
        break;
    case GTGlobals::UseKey:
        GTKeyboardDriver::keyClick( 'O', Qt::ControlModifier);
        break;
    default:
        break;
    }
    GTGlobals::sleep(500);
}

#define GT_METHOD_NAME "setPath"
bool GTFileDialogUtils::setPath()
{
    QComboBox* comboBox = fileDialog->findChild<QComboBox*>(CURRENT_FODLER_COMBO_BOX);
    if (NULL != comboBox && QDir::toNativeSeparators(comboBox->currentText()) + QDir::separator() == QDir::toNativeSeparators(path)) {
        // already there
        return false;
    }

    QLineEdit* lineEdit = fileDialog->findChild<QLineEdit*>(FILE_NAME_LINE_EDIT);
    GT_CHECK_RESULT(lineEdit != 0, QString("line edit \"%1\" not found").arg(FILE_NAME_LINE_EDIT), false);
    lineEdit->setCompleter(NULL);
    GTLineEdit::setText(os, lineEdit, path, false, textInput == CopyPaste);

    GT_CHECK_RESULT(lineEdit->text() == path, "Can't open file \"" + lineEdit->text() + "\"", false);
    return true;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setName"
void GTFileDialogUtils::setName()
{
    QLineEdit* lineEdit = fileDialog->findChild<QLineEdit*>(FILE_NAME_LINE_EDIT);
    GT_CHECK(lineEdit != 0, QString("line edit \"%1\" not found").arg(FILE_NAME_LINE_EDIT));
    lineEdit->setCompleter(NULL);

    GTLineEdit::setText(os, lineEdit, fileName, false, textInput == CopyPaste);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "selectFile"
void GTFileDialogUtils::selectFile()
{
    QTreeView *w = fileDialog->findChild<QTreeView*>("treeView");
    GT_CHECK(w != NULL, "widget, which contains list of file, not found");

    QFileSystemModel *model = qobject_cast<QFileSystemModel*>(w->model());
    QModelIndex index = model->index(path + fileName);
    GT_CHECK(index.isValid(), "File <" + path + fileName + "> not found");

    QPoint indexCenter;

    switch(method) {
    case GTGlobals::UseKey:{
        QLineEdit* lineEdit = fileDialog->findChild<QLineEdit*>(FILE_NAME_LINE_EDIT);
        GT_CHECK(lineEdit != 0, QString("line edit \"1\" not found").arg(FILE_NAME_LINE_EDIT));
        GTLineEdit::setText(os,lineEdit,fileName, false, textInput == CopyPaste);

        GTWidget::click(os,lineEdit);
        break;
    }

    case GTGlobals::UseMouse:
        w->scrollTo(index);
        indexCenter = w->visualRect(index).center();
        indexCenter.setY(indexCenter.y() + w->header()->rect().height());
        indexCenter.setX(indexCenter.x()+1);
        GTMouseDriver::moveTo(w->mapToGlobal(indexCenter));
        GTMouseDriver::click();
        break;
    default:
        break;
    }

    GTGlobals::sleep(100);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickButton"
void GTFileDialogUtils::clickButton(Button btn)
{
    QMap<Button, QString> button;
    button[Open] = "Open";
    button[Cancel] = "Cancel";
    button[Save] = "Save";
    button[Choose] = "Choose";

    QAbstractButton *button_to_click = GTWidget::findButtonByText(os, button[btn],fileDialog);
    GT_CHECK(button_to_click != NULL, "button not found");

    while (! button_to_click->isEnabled()) {
        GTGlobals::sleep(100);
    }

    GTGlobals::sleep(500);

    switch(method) {
    case GTGlobals::UseKey:
        while (! button_to_click->hasFocus()) {
            GTKeyboardDriver::keyClick( Qt::Key_Tab);
            GTGlobals::sleep(100);
        }
        GTKeyboardDriver::keyClick( Qt::Key_Enter);
        break;

    case GTGlobals::UseMouse:
        GTGlobals::sleep(100);
        GTWidget::click(os, button_to_click);
        break;
    default:
        break;
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setViewMode"
void GTFileDialogUtils::setViewMode(ViewMode v)
{
    QMap<ViewMode, QString> button;
    button[List] = "listModeButton";
    button[Detail] = "detailModeButton";
    QAbstractButton *w = qobject_cast<QAbstractButton *>(fileDialog->findChild<QWidget*>(button[v]));

    GT_CHECK(w != NULL, "view mode button not found");
    GT_CHECK(!w->isChecked(), );

    switch(method) {
    case GTGlobals::UseMouse:
        GTWidget::click(os, w);
        break;

    case GTGlobals::UseKey:
        while (! w->hasFocus()) {
            GTKeyboardDriver::keyClick( Qt::Key_Tab);
            GTGlobals::sleep(100);
        }
        GTKeyboardDriver::keyClick( Qt::Key_Space);
        break;

    default:
        break;
    }

    GTGlobals::sleep(100);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openFile"
void GTFileDialog::openFile(GUITestOpStatus &os, const QString &path, const QString &fileName,
                            Button button, GTGlobals::UseMethod m)
{
    GTFileDialogUtils *ob = new GTFileDialogUtils(os, path, fileName, (GTFileDialogUtils::Button)button, m);
    GTUtilsDialog::waitForDialog(os, ob);

    ob->openFileDialog();
    GTThread::waitForMainThread();
    GTGlobals::sleep(100);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openFile"
void GTFileDialog::openFile(GUITestOpStatus &os, const QString &filePath, Button button, GTGlobals::UseMethod m){
    int num = filePath.lastIndexOf('/');
    if (num == -1){
        num = filePath.lastIndexOf('\\');
        GT_CHECK(num != -1, QString("String %1 does not look like file path").arg(filePath));
    }
    QString path = filePath.left(num);
    QString name = filePath.right(filePath.length() - num - 1);

    openFile(os, path, name, button, m);
}
#undef GT_METHOD_NAME

void GTFileDialog::openFileList(GUITestOpStatus &os, const QString &path, const QStringList &fileNameList)
{
    GTFileDialogUtils_list *ob = new GTFileDialogUtils_list(os, path, fileNameList);
    GTUtilsDialog::waitForDialog(os, ob);

    ob->openFileDialog();

    GTGlobals::sleep();
}

void GTFileDialog::openFileList(GUITestOpStatus &os, const QStringList &filePaths) {
    GTFileDialogUtils_list *openFileDialogFiller = new GTFileDialogUtils_list(os, filePaths);
    GTUtilsDialog::waitForDialog(os, openFileDialogFiller);
    openFileDialogFiller->openFileDialog();
}

#undef GT_CLASS_NAME

} // namespace
