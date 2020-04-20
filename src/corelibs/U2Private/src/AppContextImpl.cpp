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

#include "AppContextImpl.h"
#include <U2Core/AppFileStorage.h>
#include <U2Core/AppGlobalObject.h>

namespace U2 {

static AppContextImpl appContext;

AppContextImpl::~AppContextImpl() {
    for (int i=appGlobalObjects.size();  --i>=0; ) { //remove objects in reverse mode, so the last added deleted first
        AppGlobalObject* obj = appGlobalObjects[i];
        delete obj;
    }
    delete appFileStorage;
}

AppContextImpl* AppContextImpl::getApplicationContext() {
    return &appContext;
}


void AppContextImpl::_registerGlobalObject(AppGlobalObject* go) {
    const QString& id = go->getId();
    assert(_getGlobalObjectById(id) == NULL);
    Q_UNUSED(id);
    appGlobalObjects.append(go);
}

void AppContextImpl::_unregisterGlobalObject(const QString& id) {
    for (int i=0, n = appGlobalObjects.size(); i < n; i++) {
        AppGlobalObject* obj = appGlobalObjects.at(i);
        if (obj->getId() == id) {
            appGlobalObjects.removeAt(i);
            break;
        }
    }
}

AppGlobalObject* AppContextImpl::_getGlobalObjectById(const QString& id) const {
    foreach(AppGlobalObject* obj, appGlobalObjects) {
        if (obj->getId() == id) {
            return obj;
        }
    }
    return NULL;
}




}//namespace

