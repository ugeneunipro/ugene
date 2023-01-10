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

#include <QDir>

#if defined(Q_OS_WIN)
// clang-format off
#    include <shlobj.h>
#    include <shlguid.h>
#    include <windows.h>
// clang-format on
#elif defined(Q_OS_LINUX)
#    include <QCoreApplication>
#    include <QFile>
#elif defined(Q_OS_DARWIN)
#    include <QCoreApplication>
#    include <QDir>
#    include <QFileInfo>
#    include <QProcess>
#    include <QTemporaryFile>
#endif  // Q_OS_WIN || Q_OS_LINUX || Q_OS_DARWIN

#include <base_dialogs/DefaultDialogFiller.h>
#include <base_dialogs/MessageBoxFiller.h>

#include "GTTestsCreateShortcut.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsStartPage.h"
#include "GTUtilsTaskTreeView.h"
#include "primitives/GTMenu.h"
#include "runnables/ugene/plugins/workflow_designer/StartupDialogFiller.h"

namespace U2 {

namespace GUITest_common_scenarios_create_shortcut {
using namespace HI;
GUI_TEST_CLASS_DEFINITION(test_0001) {
    // Use main menu: Help->Create desktop shortcut & click OK.
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTMenu::clickMainMenuItem(os, {"Help", "Create desktop shortcut"});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Expected state: the shortcut file is created.
#if defined(Q_OS_WIN)
    HRESULT hres;
    IShellLink* psl;

    // Initialize COM
    CoInitialize(0);
    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
    // has already been called.
    hres = CoCreateInstance(CLSID_ShellLink,
                            nullptr,
                            CLSCTX_INPROC_SERVER,
                            IID_IShellLink,
                            (LPVOID*)&psl);
    if (SUCCEEDED(hres)) {
        // Set the path to the shortcut target and add the description.
        WCHAR path[MAX_PATH];
        GetModuleFileNameW(nullptr, path, MAX_PATH);
        psl->SetPath(path);
        psl->SetDescription(L"Unipro UGENE");

        // Query IShellLink for the IPersistFile interface, used for saving the
        // shortcut in persistent storage.
        IPersistFile* ppf;
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

        if (SUCCEEDED(hres)) {
            WCHAR wsz[MAX_PATH + 1];
            CHAR pathLink[MAX_PATH + 1];

            if (SHGetSpecialFolderPathA(HWND_DESKTOP, pathLink, CSIDL_DESKTOP, FALSE)) {
                if ((MAX_PATH - strlen(pathLink)) >= strlen("\\UGENE.lnk")) {
                    strncat(pathLink, "\\UGENE.lnk", strlen("\\UGENE.lnk"));

                    // Ensure that the string is Unicode.
                    MultiByteToWideChar(CP_ACP, 0, pathLink, -1, wsz, MAX_PATH);

                    QFile link(QString::fromStdWString(wsz));
                    if (link.exists()) {
                        if (!link.permissions().testFlag(QFileDevice::ExeOwner) || !link.permissions().testFlag(QFileDevice::ExeUser)) {
                            CHECK_SET_ERR(false, "Unexpected the desktop shortcut file permissions");
                        }
                    } else {
                        CHECK_SET_ERR(false, "Can't find the desktop shortcut file");
                    }
                }
            }
        }
        psl->Release();
    }
#elif defined(Q_OS_LINUX)
    QString homeDir = QDir::homePath();
    QFile link(homeDir + "/Desktop/UGENE.desktop");
    if (link.exists()) {
        if (!link.permissions().testFlag(QFileDevice::ExeOwner) || !link.permissions().testFlag(QFileDevice::ExeUser)) {
            CHECK_SET_ERR(false, "Unexpected the desktop shortcut file permissions");
        }
    } else {
        CHECK_SET_ERR(false, "Can't find the desktop shortcut file");
    }
#elif defined(Q_OS_DARWIN)
    QFile ugeneui_path(QCoreApplication::applicationFilePath());
    QFileInfo fileInfo(ugeneui_path);
    QString filename(fileInfo.fileName());
    QFile link(QDir::homePath() + "/Desktop/" + filename);
    if (QProcess::execute(QString("/usr/bin/mdls ") + link.fileName()) != 0) {
        CHECK_SET_ERR(false, "Can't find the desktop shortcut file");
    }
#endif  // Q_OS_WIN
}

}  // namespace GUITest_common_scenarios_create_shortcut

}  // namespace U2
