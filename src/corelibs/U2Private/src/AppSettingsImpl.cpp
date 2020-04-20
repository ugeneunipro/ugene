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
#include <U2Core/AppResources.h>
#include <U2Core/FormatSettings.h>
#include <U2Core/NetworkConfiguration.h>
#include <U2Core/PasswordStorage.h>
#include <U2Core/Settings.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Test/TestRunnerSettings.h>

#include "AppSettingsImpl.h"

namespace U2 {

AppSettingsImpl::AppSettingsImpl() {
    nc = new NetworkConfiguration();
    ri = new AppResourcePool();
    userAppsSettings = new UserAppsSettings();
    trs = new TestRunnerSettings();
    formatSettings = new FormatAppsSettings();
}

AppSettingsImpl::~AppSettingsImpl() {
    delete nc;
    delete ri;
    delete userAppsSettings;
    delete trs;
}

void AppSettingsImpl::addPublicDbCredentials2Settings() {
    Settings *settings = AppContext::getSettings();
    SAFE_POINT(NULL != settings, "Invalid application settings", );
    settings->setValue("/shared_database/recent_connections/" + U2DbiUtils::PUBLIC_DATABASE_NAME, U2DbiUtils::PUBLIC_DATABASE_URL);

    PasswordStorage *passStorage = AppContext::getPasswordStorage();
    SAFE_POINT(NULL != passStorage, "Invalid shared DB passwords storage", );
    passStorage->addEntry(U2DbiUtils::PUBLIC_DATABASE_URL, U2DbiUtils::PUBLIC_DATABASE_PASSWORD, true);
}

} // namespace U2

