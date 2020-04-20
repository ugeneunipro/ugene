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
#include <U2Core/U2SafePoints.h>

#include "Metaphlan2Support.h"
#include "Metaphlan2SupportPlugin.h"
#include "Metaphlan2WorkerFactory.h"

namespace U2 {

const QString MetaphlanSupportPlugin::PLUGIN_NAME = QCoreApplication::translate("MetaphlanSupportPlugin", "MetaPhlAn2 external tool support");
const QString MetaphlanSupportPlugin::PLUGIN_DESCRIPRION = QCoreApplication::translate("MetaphlanSupportPlugin", "MetaPhlAn2 (METAgenomic PHyLogenetic ANalysis) is a tool for profiling the composition of microbial communities (bacteria, archaea, eukaryotes, and viruses) from whole-metagenome shotgun sequencing data.");

extern "C" Q_DECL_EXPORT Plugin* U2_PLUGIN_INIT_FUNC() {
    MetaphlanSupportPlugin *plugin = new MetaphlanSupportPlugin();
    return plugin;
}

MetaphlanSupportPlugin::MetaphlanSupportPlugin()
    : Plugin(PLUGIN_NAME, PLUGIN_DESCRIPRION) {
    ExternalToolRegistry *etRegistry = AppContext::getExternalToolRegistry();
    CHECK(nullptr != etRegistry, );

    etRegistry->registerEntry(new Metaphlan2Support(Metaphlan2Support::TOOL_ID, Metaphlan2Support::TOOL_NAME));

    LocalWorkflow::Metaphlan2WorkerFactory::init();
}

MetaphlanSupportPlugin::~MetaphlanSupportPlugin() {
    ExternalToolRegistry *etRegistry = AppContext::getExternalToolRegistry();
    CHECK(nullptr != etRegistry, );

    etRegistry->unregisterEntry(Metaphlan2Support::TOOL_ID);
}


}   // namespace U2
