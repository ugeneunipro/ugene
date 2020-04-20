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
#include <U2Core/Log.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include "JavaScriptAgent.h"

namespace U2 {

const QString JavaScriptAgent::ID = "ugene";

JavaScriptAgent::JavaScriptAgent(QObject *_parent)
    : QObject(_parent),
      needAdditionalInitialization(false)
{
    Settings *settings = AppContext::getSettings();
    SAFE_POINT(settings != NULL, "AppContext settings is NULL", );
    lang = settings->getValue("UGENE_CURR_TRANSL", "en").toString();
}

const QString &JavaScriptAgent::getId() const {
    return ID;
}

void JavaScriptAgent::sl_pageInitialized() {
    emit si_pageInitialized();
}

}   // namespace U2
