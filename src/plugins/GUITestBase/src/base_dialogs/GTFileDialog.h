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

#ifndef GTFILE_DIALOG_H
#define GTFILE_DIALOG_H

#include <GTGlobals.h>
#include <utils/GTUtilsDialog.h>

namespace HI {

class GTFileDialogUtils : public Filler {
    friend class GTFileDialogUtils_list;

public:
    enum Button { Open,
                  Cancel,
                  Save,
                  Choose };
    enum ViewMode { List,
                    Detail };
    enum TextInput { Typing,
                     CopyPaste };

#ifdef Q_OS_DARWIN
    GTFileDialogUtils(GUITestOpStatus &os, const QString &folderPath, const QString &fileName, Button b = Open, GTGlobals::UseMethod = GTGlobals::UseMouse, const QString& filter = QString(), TextInput = CopyPaste);
    GTFileDialogUtils(GUITestOpStatus &os, const QString &filePath, GTGlobals::UseMethod method = GTGlobals::UseMouse, Button b = Open, const QString& filter = QString(), TextInput = CopyPaste);
#else
    GTFileDialogUtils(GUITestOpStatus &os, const QString &folderPath, const QString &fileName, Button b = Open, GTGlobals::UseMethod = GTGlobals::UseMouse, const QString& filter = QString(), TextInput = Typing);
    GTFileDialogUtils(GUITestOpStatus &os, const QString &filePath, GTGlobals::UseMethod method = GTGlobals::UseMouse, Button b = Open, const QString& filter = QString(), TextInput = Typing);
#endif

    GTFileDialogUtils(GUITestOpStatus &os, CustomScenario *customScenario);
    void openFileDialog();
    void commonScenario();

protected:
    void init(const QString &filePath);

    bool setPath();
    void setName();
    void selectFile();
    void clickButton(Button);
    void setViewMode(ViewMode);
    void applyFilter();

    QWidget *fileDialog;
    QString path, fileName;
    Button button;
    GTGlobals::UseMethod method;
    QString filter;
    TextInput textInput;
};

class GTFileDialogUtils_list : public GTFileDialogUtils {
public:
    GTFileDialogUtils_list(GUITestOpStatus &os, const QString &folderPath, const QStringList &fileNames);
    GTFileDialogUtils_list(GUITestOpStatus &os, const QStringList &filePaths);

    void setNameList(GUITestOpStatus &os, const QStringList &filePaths, QWidget *parent);
    void commonScenario();

private:
    void selectFile();

    QString path;
    QStringList fileNamesList;
    QStringList filePaths;
};

class GTFileDialog {
public:
    enum Button { Open,
                  Cancel };

    static void openFile(GUITestOpStatus &os, const QString &path, const QString &fileName, Button button = Open, GTGlobals::UseMethod m = GTGlobals::UseMouse);

    static void openFile(GUITestOpStatus &os, const QString &filePath, Button button = Open, GTGlobals::UseMethod m = GTGlobals::UseMouse);

    /** Open file with file dialog. Ignores UGENE_USE_DIRECT_API_TO_OPEN_FILES option. */
    static void openFileWithDialog(GUITestOpStatus &os, const QString &path, const QString &fileName, Button button = Open, GTGlobals::UseMethod m = GTGlobals::UseMouse);

    static void openFileList(GUITestOpStatus &, const QString &, const QStringList &);
    static void openFileList(GUITestOpStatus &os, const QStringList &filePaths);

    /**
     * Converts absolute or relative path to the absolute path with native file separators which is safe to use in QT file dialog.
     * If non-absolute path is given appends "QDir::currentPath()" to the path.
     * If 'appendSlash' is true ensures that the returned path is a director path (ends with slash).
     */
    static QString toAbsoluteNativePath(const QString &path, bool appendSlash = false);
};

}  // namespace HI

#endif  // GTFILE_DIALOG_H
