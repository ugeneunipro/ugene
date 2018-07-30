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

#include "WevoteSupport.h"
#include "WevoteSupportPlugin.h"
#include "WevoteWorkerFactory.h"

namespace U2 {

const QString WevoteSupportPlugin::PLUGIN_NAME = QObject::tr("WEVOTE external tool support");
const QString WevoteSupportPlugin::PLUGIN_DESCRIPRION = QObject::tr("The plugin supports WEVOTE (WEighted VOting Taxonomic idEntification) - a tool that "
                                                                    "implements an algorithm to improve the reads classification. (https://github.com/aametwally/WEVOTE)");

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    WevoteSupportPlugin *plugin = new WevoteSupportPlugin();
    return plugin;
}

WevoteSupportPlugin::WevoteSupportPlugin()
    : Plugin(PLUGIN_NAME, PLUGIN_DESCRIPRION)
{
    ExternalToolRegistry *etRegistry = AppContext::getExternalToolRegistry();
    CHECK(NULL != etRegistry, );

    etRegistry->registerEntry(new WevoteSupport());

    LocalWorkflow::WevoteWorkerFactory::init();
}

WevoteSupportPlugin::~WevoteSupportPlugin() {
    ExternalToolRegistry *etRegistry = AppContext::getExternalToolRegistry();
    CHECK(NULL != etRegistry, );
    etRegistry->unregisterEntry(WevoteSupport::TOOL_NAME);
}

}   // namespace U2
