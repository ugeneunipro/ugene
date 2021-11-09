/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "PCRPrimerDesignForDNAAssemblyPlugin.h"

#include <U2Core/GAutoDeleteList.h>
#include <U2Core/L10n.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>
#include <U2Gui/OPWidgetFactoryRegistry.h>

#include <U2Test/GTestFrameworkComponents.h>

#include "PCRPrimerDesignForDNAAssemblyOPWidgetFactory.h"
#include "tasks/PCRPrimerDesignForDNAAssemblyTaskTest.h"

namespace U2 {

extern "C" Q_DECL_EXPORT Plugin *U2_PLUGIN_INIT_FUNC() {
    return new PCRPrimerDesignForDNAAssemblyPlugin();
}

PCRPrimerDesignForDNAAssemblyPlugin::PCRPrimerDesignForDNAAssemblyPlugin()
    : Plugin(tr("PCR Primer Design for DNA assembly"), tr("PCR Primer Design for DNA assembly.")) {

    if (AppContext::getMainWindow() != nullptr) {
        OPWidgetFactoryRegistry* opRegistry = AppContext::getOPWidgetFactoryRegistry();
        SAFE_POINT(opRegistry != nullptr, L10N::nullPointerError("Options Panel Registry"), );

        opRegistry->registerFactory(new PCRPrimerDesignForDNAAssemblyOPWidgetFactory());
    }
    
    GTestFormatRegistry *tfr = AppContext::getTestFramework()->getTestFormatRegistry();
    XMLTestFormat *xmlTestFormat = qobject_cast<XMLTestFormat *>(tfr->findFormat("XML"));
    assert(xmlTestFormat != nullptr);

    GAutoDeleteList<XMLTestFactory> *l = new GAutoDeleteList<XMLTestFactory>(this);
    l->qlist = PCRPrimerDesignForDNAAssemblyTaskTest::createTestFactories();

    foreach (XMLTestFactory *f, l->qlist) {
        bool res = xmlTestFormat->registerTestFactory(f);
        assert(res);
        Q_UNUSED(res);
    }
}

}    // namespace U2
