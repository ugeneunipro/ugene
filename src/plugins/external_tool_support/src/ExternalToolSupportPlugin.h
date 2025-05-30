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

#pragma once

#include <U2Core/PluginModel.h>
#include <U2Core/ServiceModel.h>

#include "ExternalToolManager.h"

namespace U2 {
class ETSProjectViewItemsController;

class ExternalToolSupportPlugin : public Plugin {
    Q_OBJECT
public:
    ExternalToolSupportPlugin();
    ~ExternalToolSupportPlugin();

private:
    void registerSettingsController();
    void registerWorkers();
};

class ExternalToolSupportService : public Service {
    Q_OBJECT
public:
    ExternalToolSupportService();

protected:
    void serviceStateChangedCallback(ServiceState oldState, bool enabledStateChanged) override;

    ETSProjectViewItemsController* projectViewController;
};

}  // namespace U2
