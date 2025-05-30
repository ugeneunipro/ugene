/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "PcrPlugin.h"

#include <QMenu>

#include <U2Core/L10n.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>
#include <U2Gui/OPWidgetFactoryRegistry.h>
#include <U2Gui/ToolsMenu.h>

#include "FindPrimerPairsWorker.h"
#include "InSilicoPcrOPWidgetFactory.h"
#include "InSilicoPcrWorker.h"
#include "PrimerLibrary.h"
#include "PrimerLibraryMdiWindow.h"
#include "PrimersGrouperWorker.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    return new PcrPlugin();
}

PcrPlugin::PcrPlugin()
    : Plugin(tr("In silico PCR"), tr("In silico PCR")) {
    // Init primer library
    U2OpStatus2Log os;
    PrimerLibrary* library = PrimerLibrary::getInstance(os);

    // Init GUI elements
    if (AppContext::getMainWindow() != nullptr) {
        OPWidgetFactoryRegistry* opRegistry = AppContext::getOPWidgetFactoryRegistry();
        SAFE_POINT_NN(opRegistry, );
        opRegistry->registerFactory(new InSilicoPcrOPWidgetFactory());

        if (library != nullptr) {
            auto libraryAction = new QAction(QIcon(":/core/images/database_with_arrow.png"), tr("Primer library"), this);
            libraryAction->setObjectName(ToolsMenu::PRIMER_LIBRARY);
            connect(libraryAction, SIGNAL(triggered()), SLOT(sl_primerLibrary()));
            ToolsMenu::addAction(ToolsMenu::PRIMER_MENU, libraryAction);
        }
    }
    LocalWorkflow::FindPrimerPairsWorkerFactory::init();
    LocalWorkflow::PrimersGrouperWorkerFactory::init();
    LocalWorkflow::InSilicoPcrWorkerFactory::init();
}

PcrPlugin::~PcrPlugin() {
    PrimerLibrary::release();
}

void PcrPlugin::sl_primerLibrary() {
    PrimerLibraryMdiWindow::showLibrary();
}

}  // namespace U2
