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

#include "GTFileDialog.h"
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTMenu.h>
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>

#include <QApplication>
#include <QFileSystemModel>
#include <QHeaderView>
#include <QPushButton>

#include <U2Core/AppContext.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Task.h>

#define FILE_NAME_LINE_EDIT "fileNameEdit"
#define CURRENT_FODLER_COMBO_BOX "lookInCombo"
#define FILE_TYPE_COMBO_BOX "fileTypeCombo"

namespace HI {
#define GT_CLASS_NAME "GTFileDialogUtils"

GTFileDialogUtils::GTFileDialogUtils(const QString& _path, const QString& _fileName, Button _button, GTGlobals::UseMethod _method, TextInput textInput, const QString& _filter)
    : Filler("QFileDialog"),
      fileName(_fileName),
      button(_button),
      method(_method),
      textInput(textInput),
      filter(_filter) {
    init(_path + "/" + fileName);
}

GTFileDialogUtils::GTFileDialogUtils(const QString& filePath, GTGlobals::UseMethod method, Button b, TextInput textInput, const QString& _filter)
    : Filler("QFileDialog"),
      button(b),
      method(method),
      textInput(textInput),
      filter(_filter) {
    init(filePath);
}

GTFileDialogUtils::GTFileDialogUtils(CustomScenario* customScenario)
    : Filler("QFileDialog", customScenario),
      button(Open),
      method(GTGlobals::UseMouse),
      textInput(Typing) {
}

void GTFileDialogUtils::commonScenario() {
    fileDialog = GTWidget::getActiveModalWidget();

    bool dirWasChanged = setPath();
    if (button == Choose) {
        clickButton(button);
        return;
    }

    if (dirWasChanged) {
        clickButton(Open);
    }

    if (button == Save) {  // saving file
        setName();
        clickButton(button);
        return;
    }

    // opening file or getting size
    setViewMode(Detail);
    applyFilter();
    selectFile();
    if (method == GTGlobals::UseKey) {
        GTKeyboardDriver::keyClick(Qt::Key_Enter);
    } else {
        clickButton(button);
    }
}

void GTFileDialogUtils::init(const QString& filePath) {
    const QFileInfo fileInfo(filePath);
    path = fileInfo.absoluteDir().absolutePath();
    fileName = fileInfo.fileName();
    if (!path.endsWith('/')) {
        path += '/';
    }
}

GTFileDialogUtils_list::GTFileDialogUtils_list(const QString& _path, const QStringList& _fileNamesList)
    : GTFileDialogUtils(_path, "", Open, GTGlobals::UseMouse),
      fileNamesList(_fileNamesList) {
    for (const QString& name : qAsConst(fileNamesList)) {
        filePaths << _path + "/" + name;
    }
}

GTFileDialogUtils_list::GTFileDialogUtils_list(const QStringList& _filePaths)
    : GTFileDialogUtils("", "", Open, GTGlobals::UseMouse),
      filePaths(_filePaths) {
}

void GTFileDialogUtils_list::commonScenario() {
    auto dialog = GTWidget::getActiveModalWidget();

    setNameList(filePaths, dialog);

    GTKeyboardDriver::keyClick(Qt::Key_Enter);
}

void GTFileDialogUtils_list::setNameList(const QStringList& nameList, QWidget* parent) {
    QString str;
    for (QString name : qAsConst(nameList)) {
        if (QFileInfo(name).isRelative()) {
            name = QApplication::applicationDirPath() + "/" + name;
        }
        str.append('\"' + name + "\" ");
    }
    GTLineEdit::setText(FILE_NAME_LINE_EDIT, str, parent, false, true);
}

void GTFileDialogUtils_list::selectFile() {
    GTKeyboardDriver::keyPress(Qt::Key_Control);
    for (const QString& name : qAsConst(fileNamesList)) {
        GTFileDialogUtils::fileName = name;
        GTFileDialogUtils::selectFile();
    }
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
}

void GTFileDialogUtils::openFileDialog() {
    switch (method) {
        case GTGlobals::UseMouse:
            GTMenu::clickMainMenuItem({"File", "Open..."});
            break;
        case GTGlobals::UseKey:
            GTKeyboardDriver::keyClick('O', Qt::ControlModifier);
            break;
        default:
            break;
    }
}

bool GTFileDialogUtils::setPath() {
    auto comboBox = GTWidget::findComboBox(CURRENT_FODLER_COMBO_BOX, fileDialog);
    if (QDir::toNativeSeparators(comboBox->currentText()) + QDir::separator() == QDir::toNativeSeparators(path)) {
        return false;  // Already there.
    }

    auto lineEdit = GTWidget::findLineEdit(FILE_NAME_LINE_EDIT, fileDialog);
    lineEdit->setCompleter(nullptr);
    GTLineEdit::setText(lineEdit, path, false, textInput == CopyPaste);

    GT_CHECK_RESULT(lineEdit->text() == path, "Can't open file \"" + lineEdit->text() + "\"", false);
    return true;
}

void GTFileDialogUtils::setName() {
    auto lineEdit = GTWidget::findLineEdit(FILE_NAME_LINE_EDIT, fileDialog);
    lineEdit->setCompleter(nullptr);
    GTLineEdit::setText(lineEdit, fileName, false, textInput == CopyPaste);
}

void GTFileDialogUtils::selectFile() {
    auto treeWidget = GTWidget::findTreeView("treeView", fileDialog);

    auto model = qobject_cast<QFileSystemModel*>(treeWidget->model());
    GT_CHECK(model != nullptr, "QFileSystemModel is null");

    QModelIndex index = model->index(path + fileName);
    GT_CHECK(index.isValid(), "File <" + path + fileName + "> not found");

    QPoint indexCenter;
    switch (method) {
        case GTGlobals::UseKey: {
            auto lineEdit = GTWidget::findLineEdit(FILE_NAME_LINE_EDIT, fileDialog);
            GTLineEdit::setText(lineEdit, fileName, false, textInput == CopyPaste);
            GTWidget::click(lineEdit);
            break;
        }

        case GTGlobals::UseMouse:
            GTWidget::scrollToIndex(treeWidget, index);
            indexCenter = treeWidget->visualRect(index).center();
            indexCenter.setY(indexCenter.y() + treeWidget->header()->rect().height());
            indexCenter.setX(indexCenter.x() + 1);
            GTMouseDriver::moveTo(treeWidget->mapToGlobal(indexCenter));
            GTMouseDriver::click();
            break;
        default:
            break;
    }
}

void GTFileDialogUtils::clickButton(const Button& buttonType) {
    static const QMap<Button, QString> BUTTON_NAME_MAP = {
        {Open, "Open"},
        {Cancel, "Cancel"},
        {Save, "Save"},
        {Choose, "Choose"},
    };
    auto targetButton = GTWidget::findButtonByText(BUTTON_NAME_MAP[buttonType], fileDialog);
    GTWidget::checkEnabled(targetButton);

    switch (method) {
        case GTGlobals::UseKey:
            while (!targetButton->hasFocus()) {
                GTKeyboardDriver::keyClick(Qt::Key_Tab);
            }
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
            break;
        case GTGlobals::UseMouse:
            GTWidget::click(targetButton);
            break;
        default:
            break;
    }
}

void GTFileDialogUtils::setViewMode(const ViewMode& viewMode) {
    static const QMap<ViewMode, QString> BUTTON_NAME_MAP = {
        {List, "listModeButton"},
        {Detail, "detailModeButton"},
    };
    auto targetButton = GTWidget::findToolButton(BUTTON_NAME_MAP[viewMode], fileDialog);
    GTWidget::checkEnabled(targetButton);

    switch (method) {
        case GTGlobals::UseMouse:
            GTWidget::click(targetButton);
            break;
        case GTGlobals::UseKey:
            while (!targetButton->hasFocus()) {
                GTKeyboardDriver::keyClick(Qt::Key_Tab);
            }
            GTKeyboardDriver::keyClick(Qt::Key_Space);
            break;
        default:
            break;
    }
}

void GTFileDialogUtils::applyFilter() {
    if (filter.isEmpty()) {
        return;
    }
    auto comboBox = GTWidget::findComboBox(FILE_TYPE_COMBO_BOX, fileDialog);
    GTComboBox::selectItemByText(comboBox, filter, GTGlobals::UseMouse);
}

void GTFileDialog::openFile(const QString& path, const QString& fileName, Button button, GTGlobals::UseMethod m) {
    bool isDirectApiMode = button == Open && qgetenv("UGENE_USE_DIRECT_API_TO_OPEN_FILES") == "1";
    if (!isDirectApiMode) {
        openFileWithDialog(path, fileName, button, m);
        return;
    }
    class OpenFileScenario : public CustomScenario {
    public:
        OpenFileScenario(const QString& pathToFile)
            : pathToFile(pathToFile) {
        }
        void run() override {
            auto openFileTask = U2::AppContext::getProjectLoader()->openWithProjectTask(pathToFile);
            if (openFileTask == nullptr) {
                GT_FAIL("Failed to create open file task: " + pathToFile + ". Is the file already in the project?", );
            }
            U2::AppContext::getTaskScheduler()->registerTopLevelTask(openFileTask);
        }
        QString pathToFile;
    };
    GTThread::runInMainThread(new OpenFileScenario(path + "/" + fileName));
    GTThread::waitForMainThread();
}

void GTFileDialog::openFileWithDialog(const QString& path, const QString& fileName, Button button, GTGlobals::UseMethod m) {
    auto utils = new GTFileDialogUtils(path, fileName, (GTFileDialogUtils::Button)button, m);
    GTUtilsDialog::waitForDialog(utils);
    utils->openFileDialog();
    GTThread::waitForMainThread();
}

void GTFileDialog::openFile(const QString& filePath, Button button, GTGlobals::UseMethod m) {
    int num = filePath.lastIndexOf('/');
    if (num == -1) {
        num = filePath.lastIndexOf('\\');
        GT_CHECK(num != -1, QString("String %1 does not look like file path").arg(filePath));
    }
    QString path = filePath.left(num);
    QString name = filePath.right(filePath.length() - num - 1);

    openFile(path, name, button, m);
}

void GTFileDialog::openFileList(const QString& path, const QStringList& fileNameList) {
    auto fileListFiller = new GTFileDialogUtils_list(path, fileNameList);
    GTUtilsDialog::waitForDialog(fileListFiller);
    fileListFiller->openFileDialog();
}

void GTFileDialog::openFileList(const QStringList& filePaths) {
    GTFileDialogUtils_list* openFileDialogFiller = new GTFileDialogUtils_list(filePaths);
    GTUtilsDialog::waitForDialog(openFileDialogFiller);
    openFileDialogFiller->openFileDialog();
}

QString GTFileDialog::toAbsoluteNativePath(const QString& path, bool appendSlash) {
    QString result = path;
    if (!QFileInfo(result).isAbsolute()) {
        result = QDir::currentPath() + "/" + result;
    }
    result = QDir::cleanPath(result);
    result = QDir::toNativeSeparators(result);
    if (appendSlash && !result.endsWith(QDir::separator())) {
        result += QDir::separator();
    }
    return result;
}

#undef GT_CLASS_NAME

}  // namespace HI
