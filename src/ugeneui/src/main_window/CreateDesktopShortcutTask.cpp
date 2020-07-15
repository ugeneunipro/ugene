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

#include "CreateDesktopShortcutTask.h"


#if defined(Q_OS_WIN)
#include <windows.h>
#include <shlguid.h>
#include <shlobj.h>
#elif defined(Q_OS_LINUX)
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#elif defined(Q_OS_MAC)
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QTemporaryFile>
#endif // Q_OS_WIN || Q_OS_LINUX || Q_OS_MAC

#include <QMainWindow>
#include <QMessageBox>
#include <QNetworkReply>
#include <QPushButton>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/NetworkConfiguration.h>
#include <U2Core/Settings.h>
#include <U2Core/SyncHttp.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>

namespace U2 {

CreateDesktopShortcutTask::CreateDesktopShortcutTask(bool startUp)
    : Task(tr("Create desktop shortcut"), TaskFlag_None) {
    runOnStartup = startUp;
    setVerboseLogMode(true);
    startError = false;
}

bool CreateDesktopShortcutTask::createDesktopShortcut() {
#if defined(Q_OS_WIN)
    HRESULT hres;
    IShellLink *psl;

    // Initialize COM
    CoInitialize(0);
    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
    // has already been called.
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *)&psl);
    if (SUCCEEDED(hres)) {
        // Set the path to the shortcut target and add the description.
        WCHAR path[MAX_PATH];
        GetModuleFileNameW(NULL, path, MAX_PATH);
        psl->SetPath(path);
        psl->SetDescription(L"Unipro UGENE");

        // Query IShellLink for the IPersistFile interface, used for saving the
        // shortcut in persistent storage.
        IPersistFile *ppf;
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID *)&ppf);

        if (SUCCEEDED(hres)) {
            WCHAR wsz[MAX_PATH + 1];
            CHAR pathLink[MAX_PATH + 1];

            hres = 0;
            if (SHGetSpecialFolderPathA(HWND_DESKTOP, pathLink, CSIDL_DESKTOP, FALSE)) {
                if ((MAX_PATH - strlen(pathLink)) >= strlen("\\UGENE.lnk")) {
                    strncat(pathLink, "\\UGENE.lnk", strlen("\\UGENE.lnk"));

                    // Ensure that the string is Unicode.
                    MultiByteToWideChar(CP_ACP, 0, pathLink, -1, wsz, MAX_PATH);

                    // Add code here to check return value from MultiByteWideChar
                    // for success.

                    // Save the link by calling IPersistFile::Save.
                    hres = ppf->Save(wsz, TRUE);
                    ppf->Release();
                }
            }
        }
        psl->Release();
    }
    return SUCCEEDED(hres);
#elif defined(Q_OS_LINUX)
    QString homeDir = QDir::homePath();
    QFile link(homeDir + "/Desktop/UGENE.desktop");
    if (link.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&link);
        out.setCodec("UTF-8");
        out << "[Desktop Entry]" << endl
            << "Encoding=UTF-8" << endl
            << "Version=1.0" << endl
            << "Type=Application" << endl
            << "Terminal=false" << endl
            << "Exec=" + QCoreApplication::applicationFilePath() << endl
            << "Name=Unipro UGENE" << endl
            << "Icon=" + QCoreApplication::applicationDirPath() + "/ugene.png" << endl
            << "Name[en_US]=Unipro UGENE" << endl;
        link.close();
        if (!link.setPermissions(link.permissions() | QFileDevice::ExeOwner | QFileDevice::ExeUser)) {
            return false;
        }
        return true;
    }
    return false;
#elif defined(Q_OS_MAC)
    QTemporaryFile file;
    if (file.open()) {
        // We're going to streaming text to the file
        QTextStream stream(&file);
        stream << "#!/bin/bash" << '\n'
               << "" << '\n'
               << "osascript <<END_SCRIPT" << '\n'
               << "tell application \"Finder\" to make alias file to file (posix file \"$1\") at desktop" << '\n'
               << "END_SCRIPT" << '\n';
        file.close();
        if (!file.setPermissions(file.permissions() | QFileDevice::ExeOwner | QFileDevice::ExeUser)) {
            return false;
        }
        QFileInfo script(file);
        QString ugeneui_path = QCoreApplication::applicationFilePath();
        if (QProcess::execute(QString("/bin/sh ") + script.absoluteFilePath() + " " + ugeneui_path) != 0) {
            return false;
        }

        QFileInfo fileInfo(ugeneui_path);
        QString filename(fileInfo.fileName());
        QFile link(QDir::homePath() + "/Desktop/" + filename);
        if (QProcess::execute(QString("/usr/bin/mdls ") + link.fileName()) != 0) {
            return false;
        }
    }
    return true;
#endif    // Q_OS_WIN
}
void CreateDesktopShortcutTask::run() {
    if (!runOnStartup) {
        createDesktopShortcut();
    }
}

Task::ReportResult CreateDesktopShortcutTask::report() {
    if (!(hasError() || startError)) {
        CreateDesktopShortcutTask::getAnswer();
    }
    return ReportResult_Finished;
}

CreateDesktopShortcutTask::Answer CreateDesktopShortcutTask::getAnswer() {
    QMessageBox::information(AppContext::getMainWindow()->getQMainWindow(),
        tr("Desktop shortcut"),
        tr("A new shortcut to the UGENE application was created on the desktop."));
    return CreateDesktopShortcutTask::DoNothing;
}

}    // namespace U2
