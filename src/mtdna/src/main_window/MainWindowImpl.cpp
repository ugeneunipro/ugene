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

#include <qglobal.h>
#ifdef Q_OS_DARWIN
#    include <Security/Authorization.h>
#    include <errno.h>
#    include <unistd.h>
#endif
#include <algorithm>

#include <QAction>
#include <QDesktopServices>
#include <QMessageBox>
#include <QMovie>
#include <QPainter>
#include <QPixmap>
#include <QToolBar>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentSelection.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/Task.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/ObjectViewModel.h>

#include "MainWindowImpl.h"
#include "ShutdownTask.h"
#include "TmpDirChangeDialogController.h"

namespace U2 {

MainWindowImpl::MainWindowImpl(): mw(new QMainWindow) {
}

MainWindowImpl::~MainWindowImpl() {
    mw->deleteLater();
}

void MainWindowImpl::setWindowTitle(const QString& title) {
    if (title.isEmpty()) {
        mw->setWindowTitle(U2_APP_TITLE);
    } else {
        mw->setWindowTitle(title + " " + U2_APP_TITLE);
    }
}

void MainWindowImpl::registerAction(QAction*) {
}

bool MainWindowImpl::isDarkTheme() const {
    return false;
}

void MainWindowImpl::setNewStyle(const QString&, int) {
    return;
}

void MainWindowImpl::setShutDownInProcess(bool flag) {
    shutDownInProcess = flag;
}
QMenu* MainWindowImpl::getTopLevelMenu(const QString&) const {
    return nullptr;
}

QToolBar* MainWindowImpl::getToolbar(const QString& sysName) const {
    return nullptr;
}

void MainWindowImpl::addNotification(const QString&, NotificationType) {
}

}  // namespace U2

