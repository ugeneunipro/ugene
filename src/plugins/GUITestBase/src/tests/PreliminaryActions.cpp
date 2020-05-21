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

#include <GTGlobals.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTWidget.h>
#include <system/GTFile.h>
#include <utils/GTUtilsDialog.h>

#include <QDir>
#include <QGuiApplication>
#include <QMainWindow>
#include <QProcess>

#include <U2Core/AppContext.h>
#include <U2Core/Log.h>

#include <U2Gui/MainWindow.h>

#include "GTUtilsTaskTreeView.h"
#include "PreliminaryActions.h"

namespace U2 {
namespace GUITest_preliminary_actions {

PRELIMINARY_ACTION_DEFINITION(pre_action_0000) {
    // Wait all startup tasks finished
    // Close all unexpected system messageboxes on Windows
    // Close all UGENE dialogs
    // Release mouse and keyboard buttons
    // Start dialogs hang checking

    GTUtilsTaskTreeView::waitTaskFinished(os);

#ifdef Q_OS_WIN
    QProcess::execute("closeAllErrors.exe");    //this exe file, compiled Autoit script
#endif

    GTUtilsDialog::cleanup(os, GTUtilsDialog::NoFailOnUnfinished);

#ifndef Q_OS_WIN
    GTMouseDriver::release(Qt::RightButton);
    GTMouseDriver::release();
    GTKeyboardDriver::keyRelease(Qt::Key_Control);
    GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    GTKeyboardDriver::keyRelease(Qt::Key_Alt);
    uiLog.trace(QString("pre_action_0000: next keyboard modifiers are pressed before test: %1").arg(QGuiApplication::queryKeyboardModifiers()));
#endif

    GTUtilsDialog::startHangChecking(os);
}

PRELIMINARY_ACTION_DEFINITION(pre_action_0001) {
    // Ensure there is no opened project

    CHECK_SET_ERR(AppContext::getProjectView() == NULL && AppContext::getProject() == NULL, "There is a project");
}

PRELIMINARY_ACTION_DEFINITION(pre_action_0002) {
    // Maximize the main window

    Q_UNUSED(os);
    QMainWindow *mainWindow = AppContext::getMainWindow()->getQMainWindow();
    CHECK_SET_ERR(mainWindow != NULL, "main window is NULL");

    if (!mainWindow->isMaximized()) {
        GTWidget::showMaximized(os, mainWindow);
        GTGlobals::sleep(1000);
    }
}

PRELIMINARY_ACTION_DEFINITION(pre_action_0003) {
    if (qgetenv("UGENE_TEST_SKIP_BACKUP_AND_RESTORE") == "1") {    // Restored by the parent process
        return;
    }
    // Backup some files used in tests directly.
    if (QDir(testDir).exists()) {
        GTFile::backup(os, testDir + "_common_data/scenarios/project/proj1.uprj");
        GTFile::backup(os, testDir + "_common_data/scenarios/project/proj2-1.uprj");
        GTFile::backup(os, testDir + "_common_data/scenarios/project/proj2.uprj");
        GTFile::backup(os, testDir + "_common_data/scenarios/project/proj3.uprj");
        GTFile::backup(os, testDir + "_common_data/scenarios/project/proj4.uprj");
        GTFile::backup(os, testDir + "_common_data/scenarios/project/proj5.uprj");

        // Files from the projects above.
        GTFile::backup(os, testDir + "_common_data/scenarios/project/1.gb");
    }
}

PRELIMINARY_ACTION_DEFINITION(pre_action_0004) {
    // create a directory for screenshots

    Q_UNUSED(os);
    QDir dir(QDir().absoluteFilePath(screenshotDir));
    if (!dir.exists(dir.absoluteFilePath(screenshotDir))) {
        dir.mkpath(dir.path());
    }
}

PRELIMINARY_ACTION_DEFINITION(pre_action_0005) {
    // Click somewhere to the main window in mac to be sure that the focus is on the application

    QMainWindow *mw = AppContext::getMainWindow()->getQMainWindow();
    CHECK_SET_ERR(mw != NULL, "main window is NULL");
#ifdef Q_OS_MAC
    GTWidget::click(os, mw, Qt::LeftButton, QPoint(200, 200));
#endif
}

}    // namespace GUITest_preliminary_actions
}    // namespace U2
