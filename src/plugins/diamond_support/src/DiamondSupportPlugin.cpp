/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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
#include <U2Core/ExternalToolRegistry.h>

#include "DiamondBuildWorkerFactory.h"
#include "DiamondClassifyWorkerFactory.h"
#include "DiamondSupport.h"
#include "DiamondSupportPlugin.h"

namespace U2 {

const QString DiamondSupportPlugin::PLUGIN_NAME = QObject::tr("DIAMOND external tool support");
const QString DiamondSupportPlugin::PLUGIN_DESCRIPRION = QObject::tr("The plugin supports DIAMOND: a sequence aligner for protein "
                                                                     "and translated DNA searches, designed for high performance analysis "
                                                                     "of big sequence data (https://github.com/bbuchfink/diamond)");

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    DiamondSupportPlugin *plugin = new DiamondSupportPlugin();
    return plugin;
}

DiamondSupportPlugin::DiamondSupportPlugin()
    : Plugin(PLUGIN_NAME, PLUGIN_DESCRIPRION)
{
    ExternalToolRegistry *etRegistry = AppContext::getExternalToolRegistry();
    CHECK(NULL != etRegistry, );

    etRegistry->registerEntry(new DiamondSupport(DiamondSupport::TOOL_NAME));

    LocalWorkflow::DiamondBuildWorkerFactory::init();
    LocalWorkflow::DiamondClassifyWorkerFactory::init();
}

DiamondSupportPlugin::~DiamondSupportPlugin() {
    ExternalToolRegistry *etRegistry = AppContext::getExternalToolRegistry();
    CHECK(NULL != etRegistry, );
    etRegistry->unregisterEntry(DiamondSupport::TOOL_NAME);
}

}   // namespace U2
