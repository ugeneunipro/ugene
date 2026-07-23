/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTWidget.h>
#include <system/GTClipboard.h>
#include <system/GTFile.h>
#include <utils/GTThread.h>
#include <utils/GTUtilsDialog.h>

#include <QApplication>
#include <QDir>
#include <QProcess>

#include <U2Core/AppContext.h>

#include "GTUtilsTask.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "PosteriorActions.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"
#include <U2Core/ProjectModel.h>
namespace U2 {
namespace GUITest_posterior_actions {

POSTERIOR_ACTION_DEFINITION(post_action_0000) {
    uiLog.trace(QString("post_action_0000: next keyboard modifiers are pressed after the test: %1").arg((quint32)QGuiApplication::queryKeyboardModifiers()));
    GTMouseDriver::releasePressedButtons();
    GTKeyboardDriver::releasePressedKeys();
}

/**
 * Fetches the widget returned by `getActiveWidget` and closes it, all in one call run on the main
 * thread. This runs on GUITestThread, a worker thread separate from the main/GUI thread: fetching
 * the widget pointer here and closing it later (e.g. via a separate GTWidget::close() call) leaves
 * a window where the main thread can destroy that widget in between, turning the pointer into a
 * dangling one and crashing on close - so fetch-and-close must happen as a single atomic step on
 * the main thread. Returns true if a widget was found (and closed).
 */
static bool closeActiveWidgetOnMainThreadIfAny(QWidget* (*getActiveWidget)()) {
    bool found = false;
    GTThread::runInMainThread([getActiveWidget, &found]() {
        QWidget* widget = getActiveWidget();
        if (widget != nullptr) {
            widget->close();
            found = true;
        }
    });
    return found;
}

POSTERIOR_ACTION_DEFINITION(post_action_0001) {
    // Close all popup widgets
    // Close all modal widgets
    // Clear the clipboard

    for (int i = 0; closeActiveWidgetOnMainThreadIfAny(&QApplication::activePopupWidget); i++) {
        GTGlobals::sleep(100);
    }

    for (int i = 0; closeActiveWidgetOnMainThreadIfAny(&QApplication::activeModalWidget); i++) {
        GTGlobals::sleep(100);
    }

    GTClipboard::clear();
}

POSTERIOR_ACTION_DEFINITION(post_action_0002) {
    // Clear the project
    // Close the project
    // Close all MDI windows
    // Cancel all tasks
    GTUtilsDialog::checkNoActiveWaiters(10000);

    qputenv("UGENE_GUI_TEST_SHUTDOWN", "1");
    GTUtilsMdi::closeAllWindows();

    GTUtilsTask::cancelAllTasks();
    GTUtilsTaskTreeView::waitTaskFinished(10000);
}

POSTERIOR_ACTION_DEFINITION(post_action_0003) {
    if (qgetenv("UGENE_TEST_SKIP_BACKUP_AND_RESTORE") == "1") {  // Restored by the parent process
        qDebug("Skipping restore: UGENE_TEST_SKIP_BACKUP_AND_RESTORE = 1");
        return;
    }
    // Restore backup files
    if (QDir(testDir).exists()) {
        GTFile::restore(testDir + "_common_data/scenarios/project/proj1.uprj");
        GTFile::restore(testDir + "_common_data/scenarios/project/proj2-1.uprj");
        GTFile::restore(testDir + "_common_data/scenarios/project/proj2.uprj");
        GTFile::restore(testDir + "_common_data/scenarios/project/proj3.uprj");
        GTFile::restore(testDir + "_common_data/scenarios/project/proj4.uprj");
        GTFile::restore(testDir + "_common_data/scenarios/project/proj5.uprj");

        // Files from the projects above.
        GTFile::restore(testDir + "_common_data/scenarios/project/1.gb");

        GTFile::restore(dataDir + "workflow_samples/NGS/consensus.uwl");
    }
}

POSTERIOR_ACTION_DEFINITION(post_action_0004) {
    if (qgetenv("UGENE_TEST_SKIP_BACKUP_AND_RESTORE") == "1") {  // Restored by the parent process
        qDebug("Skipping restore: UGENE_TEST_SKIP_BACKUP_AND_RESTORE = 1");
        return;
    }
    if (QDir(sandBoxDir).exists()) {
        GTFile::setReadWrite(sandBoxDir, true);
        QDir sandBox(sandBoxDir);
        const QStringList entryList = sandBox.entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Hidden);
        for (const QString& path : qAsConst(entryList)) {
            GTFile::removeDir(sandBox.absolutePath() + "/" + path);
        }
    }
}

}  // namespace GUITest_posterior_actions
}  // namespace U2
