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

#include "KrakenBuildWorkerFactory.h"
#include "KrakenClassifyWorkerFactory.h"
#include "KrakenSupport.h"
#include "KrakenSupportPlugin.h"

namespace U2 {

const QString KrakenSupportPlugin::PLUGIN_NAME = QObject::tr("Kraken external tool support");
const QString KrakenSupportPlugin::PLUGIN_DESCRIPRION = QObject::tr("The plugin supports Kraken: taxonomic sequence classification system (https://ccb.jhu.edu/software/kraken/)");

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    KrakenSupportPlugin *plugin = new KrakenSupportPlugin();
    return plugin;
}

KrakenSupportPlugin::KrakenSupportPlugin()
    : Plugin(PLUGIN_NAME, PLUGIN_DESCRIPRION)
{
    ExternalToolRegistry *etRegistry = AppContext::getExternalToolRegistry();
    CHECK(NULL != etRegistry, );

    etRegistry->registerEntry(new KrakenSupport(KrakenSupport::BUILD_TOOL));
    etRegistry->registerEntry(new KrakenSupport(KrakenSupport::CLASSIFY_TOOL));
    etRegistry->setToolkitDescription(KrakenSupport::GROUP_NAME, tr("Kraken is a taxonomic sequence classifier that assigns taxonomic labels to short DNA reads."));

    LocalWorkflow::KrakenBuildWorkerFactory::init();
    LocalWorkflow::KrakenClassifyWorkerFactory::init();
}

KrakenSupportPlugin::~KrakenSupportPlugin() {
    ExternalToolRegistry *etRegistry = AppContext::getExternalToolRegistry();
    CHECK(NULL != etRegistry, );
    etRegistry->unregisterEntry(KrakenSupport::BUILD_TOOL);
    etRegistry->unregisterEntry(KrakenSupport::CLASSIFY_TOOL);
}

}   // namespace U2
