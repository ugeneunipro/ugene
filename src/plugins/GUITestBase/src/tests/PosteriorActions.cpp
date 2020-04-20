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
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTMenu.h>
#include <primitives/GTWidget.h>
#include <system/GTClipboard.h>
#include <system/GTFile.h>
#include <utils/GTUtilsDialog.h>

#include <QApplication>
#include <QDir>
#include <QGuiApplication>
#include <QTreeView>

#include <U2Core/AppContext.h>
#include <U2Core/Log.h>
#include <U2Core/Task.h>

#include "GTUtilsMdi.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "PosteriorActions.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"

namespace U2 {
namespace GUITest_posterior_actions {

POSTERIOR_ACTION_DEFINITION(post_action_0000) {
    // Release all hold keyboard modifier keys

    const Qt::KeyboardModifiers modifiers = QGuiApplication::queryKeyboardModifiers();
    if (modifiers & Qt::ShiftModifier) {
        GTKeyboardDriver::keyRelease(Qt::Key_Shift);
    }

    if (modifiers & Qt::ControlModifier) {
        GTKeyboardDriver::keyRelease(Qt::Key_Control);
    }

    if (modifiers & Qt::AltModifier) {
        GTKeyboardDriver::keyRelease(Qt::Key_Alt);
    }

    uiLog.trace(QString("post_action_0000: next keyboard modifiers are pressed after test: %1").arg(QGuiApplication::queryKeyboardModifiers()));
}

POSTERIOR_ACTION_DEFINITION(post_action_0001) {
    // Close all popup widgets
    // Close all modal widgets
    // Clear the clipboard

    QWidget* popupWidget = QApplication::activePopupWidget();
    while (popupWidget != NULL) {
        GTWidget::close(os, popupWidget);
        popupWidget = QApplication::activePopupWidget();
    }

    QWidget* modalWidget = QApplication::activeModalWidget();
    while (modalWidget != NULL) {
        GTWidget::close(os, modalWidget);
        modalWidget = QApplication::activeModalWidget();
    }

    GTClipboard::clear(os);
}

POSTERIOR_ACTION_DEFINITION(post_action_0002) {
    // Clear the project
    // Close the project
    // Close all MDI windows
    // Cancel all tasks

    GTGlobals::sleep(1000);

    if (AppContext::getProject() != NULL) {
        GTWidget::click(os, GTUtilsProjectTreeView::getTreeView(os));
        GTKeyboardDriver::keyClick('a', Qt::ControlModifier);
        GTGlobals::sleep(100);

        GTUtilsDialog::waitForDialog(os, new SaveProjectDialogFiller(os, QDialogButtonBox::No));
        GTUtilsDialog::waitForDialog(os, new AppCloseMessageBoxDialogFiller(os));
        GTKeyboardDriver::keyClick(Qt::Key_Delete);
        GTGlobals::sleep(500);
#ifdef Q_OS_MAC
        GTMenu::clickMainMenuItem(os, QStringList() << "File" << "Close project");
#else
        GTKeyboardDriver::keyClick('q', Qt::ControlModifier);
        GTGlobals::sleep(100);
#endif
        GTGlobals::sleep(500);

        GTUtilsDialog::cleanup(os, GTUtilsDialog::NoFailOnUnfinished);
        GTGlobals::sleep();
    }

    GTUtilsMdi::closeAllWindows(os);

    AppContext::getTaskScheduler()->cancelAllTasks();
    GTUtilsTaskTreeView::waitTaskFinished(os, 60000);
}

POSTERIOR_ACTION_DEFINITION(post_action_0003) {
    // Restore backuped files

    if (QDir(testDir).exists()) {
        GTFile::restore(os, testDir + "_common_data/scenarios/project/proj1.uprj");
        GTFile::restore(os, testDir + "_common_data/scenarios/project/proj2-1.uprj");
        GTFile::restore(os, testDir + "_common_data/scenarios/project/proj2.uprj");
        GTFile::restore(os, testDir + "_common_data/scenarios/project/proj3.uprj");
        GTFile::restore(os, testDir + "_common_data/scenarios/project/proj4.uprj");
        GTFile::restore(os, testDir + "_common_data/scenarios/project/proj5.uprj");
    }
}

POSTERIOR_ACTION_DEFINITION(post_action_0004) {
    if (QDir(sandBoxDir).exists()) {
        GTFile::setReadWrite(os, sandBoxDir, true);
        QDir sandBox(sandBoxDir);
        foreach (const QString &path, sandBox.entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Hidden)) {
            GTFile::removeDir(sandBox.absolutePath() + "/" + path);
        }
    }
}

}   // namespace GUITest_posterior_actions
}   // namespace U2
