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

#include <U2Core/AppContext.h>
#include <U2Core/L10n.h>
#include <U2Core/Settings.h>
#include <U2Core/Task.h>
#include <U2Core/U2SafePoints.h>

#include "WelcomePageMdi.h"
#include "WelcomePageMdiController.h"
#include "project_support/ProjectLoaderImpl.h"

namespace U2 {

WelcomePageMdiController::WelcomePageMdiController()
    : QObject(NULL),
      welcomePage(NULL)
{
    MWMDIManager *mdiManager = getMdiManager();
    CHECK(NULL != mdiManager, );

    connect(mdiManager, SIGNAL(si_windowClosing(MWMDIWindow*)), SLOT(sl_onMdiClose(MWMDIWindow*)));
}

MWMDIManager * WelcomePageMdiController::getMdiManager() {
    MainWindow *mainWindow = AppContext::getMainWindow();
    SAFE_POINT(NULL != mainWindow, L10N::nullPointerError("Main Window"), NULL);

    MWMDIManager *result = mainWindow->getMDIManager();
    SAFE_POINT(NULL != result, L10N::nullPointerError("MDI Manager"), NULL);
    return result;
}

void WelcomePageMdiController::sl_showPage() {
    MWMDIManager *mdiManager = getMdiManager();
    CHECK(mdiManager != nullptr, );

    if (welcomePage != nullptr) {
        if (mdiManager->getWindows().contains(welcomePage)) {
            mdiManager->activateWindow(welcomePage);
        } // else: it means that the page has already been called but it is loading now
        return;
    }

    welcomePage = new WelcomePageMdi(tr("Start Page"), this);
    mdiManager->addMDIWindow(welcomePage);
    sl_onRecentChanged();
}

void WelcomePageMdiController::sl_onMdiClose(MWMDIWindow *mdi) {
    CHECK(mdi == welcomePage, );
    welcomePage = NULL;
}

void WelcomePageMdiController::sl_onRecentChanged() {
    CHECK(welcomePage != nullptr, );
    QStringList recentProjects = AppContext::getSettings()->getValue(SETTINGS_DIR + RECENT_PROJECTS_SETTINGS_NAME, QStringList(), true).toStringList();
    QStringList recentFiles = AppContext::getSettings()->getValue(SETTINGS_DIR + RECENT_ITEMS_SETTINGS_NAME, QStringList(), true).toStringList();
    welcomePage->updateRecent(recentProjects, recentFiles);
}

} // U2
