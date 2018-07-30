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

#include "ClarkBuildWorker.h"
#include "ClarkClassifyWorker.h"
#include "ClarkSupport.h"
#include "ClarkSupportPlugin.h"

namespace U2 {

const QString ClarkSupportPlugin::PLUGIN_NAME = QObject::tr("CLARK external tool support");
const QString ClarkSupportPlugin::PLUGIN_DESCRIPRION = QObject::tr("The plugin supports CLARK: fast, accurate and versatile sequence classification system (http://clark.cs.ucr.edu)");

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    ClarkSupportPlugin *plugin = new ClarkSupportPlugin();
    return plugin;
}

ClarkSupportPlugin::ClarkSupportPlugin()
    : Plugin(PLUGIN_NAME, PLUGIN_DESCRIPRION)
{
    ExternalToolRegistry *etRegistry = AppContext::getExternalToolRegistry();
    CHECK(NULL != etRegistry, );

    ClarkSupport::registerTools(etRegistry);

    LocalWorkflow::ClarkBuildWorkerFactory::init();
    LocalWorkflow::ClarkClassifyWorkerFactory::init();
}

ClarkSupportPlugin::~ClarkSupportPlugin() {
    ExternalToolRegistry *etRegistry = AppContext::getExternalToolRegistry();
    CHECK(NULL != etRegistry, );
    ClarkSupport::unregisterTools(etRegistry);
}

}   // namespace U2
