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
#include <U2Core/Counter.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include "PrimerLibraryWidget.h"

#include "PrimerLibraryMdiWindow.h"

namespace U2 {

void PrimerLibraryMdiWindow::showLibrary() {
    GCOUNTER(cvar, tvar, "Primer Library");
    MWMDIManager *mdiManager = AppContext::getMainWindow()->getMDIManager();
    SAFE_POINT(NULL != mdiManager, L10N::nullPointerError("MDI Manager"), );

    static const QString title = tr("Primer Library");
    foreach (MWMDIWindow *mdiWindow, mdiManager->getWindows()) {
        if (title == mdiWindow->windowTitle()) {
            mdiManager->activateWindow(mdiWindow);
            return;
        }
    }

    mdiManager->addMDIWindow(new PrimerLibraryMdiWindow(title));
}

PrimerLibraryMdiWindow::PrimerLibraryMdiWindow(const QString &title)
: MWMDIWindow(title)
{
    QVBoxLayout *l = new QVBoxLayout(this);
    l->setMargin(0);

    PrimerLibraryWidget *libraryWidget = new PrimerLibraryWidget(this);
    l->addWidget(libraryWidget);
    connect(libraryWidget, SIGNAL(si_close()), SLOT(sl_closeWindow()));
}

void PrimerLibraryMdiWindow::sl_closeWindow() {
    MWMDIManager *mdiManager = AppContext::getMainWindow()->getMDIManager();
    SAFE_POINT(NULL != mdiManager, L10N::nullPointerError("MDI Manager"), );

    mdiManager->closeMDIWindow(this);
}

} // U2
