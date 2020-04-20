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

#include <U2Core/DocumentModel.h>

#include "ProjectTreeControllerModeSettings.h"

namespace U2 {

bool PTCObjectRelationFilter::filter(GObject* o) const {
    return !o->hasObjectRelation(rel);
}

bool ProjectTreeControllerModeSettings::isDocumentShown(Document* doc) const {
    if (groupMode != ProjectTreeGroupMode_ByDocument && groupMode != ProjectTreeGroupMode_Flat) {
        return false;
    }
    if (groupMode == ProjectTreeGroupMode_Flat && (doc->isLoaded() || !doc->getObjects().isEmpty())) {
        return false; // only unloaded docs without cached object info are shown in flat mode
    }
    if (excludeDocList.contains(doc)) {
        return false;
    }

    //filter by readonly state
    //TODO: revise readonly filters;
    //if the only lock is unloaded state lock -> not show it
    bool isReadonly = ! (doc->getStateLocks().size() == 1 && doc->getDocumentModLock(DocumentModLock_UNLOADED_STATE)!=NULL);
    bool res = readOnlyFilter == TriState_Unknown ? true :
        (readOnlyFilter == TriState_Yes && !isReadonly) || (readOnlyFilter == TriState_No && isReadonly);
    if (!res) {
        return false;
    }

    //filter by object types
    const QList<GObject *> &docObjs = doc->getObjects();
    if (!docObjs.isEmpty()) { //ok we have mapping about document objects -> apply filter to the objects
        bool found = false;
        foreach (GObject *o, docObjs) {
            found = isObjectShown(o);
            if (found) {
                break;
            }
        }
        if (!found) {
            return false;
        }
    } else {
        if (!doc->isLoaded()) {
            DocumentFormatConstraints c;
            c.supportedObjectTypes += objectTypesToShow;
            res = doc->getDocumentFormat()->checkConstraints(c);
        } else {
            res = true;
        }
        if (!res) {
            return false;
        }
    }

    // check custom filter
    if (documentFilter!= NULL && documentFilter->filter(doc)) {
        return false;
    }

    return true;
}

bool ProjectTreeControllerModeSettings::isObjectShown(GObject* o) const {
    //filter by type
    GObjectType t = o->isUnloaded()
        ? qobject_cast<UnloadedObject*>(o)->getLoadedObjectType()
        : o->getGObjectType();
    bool res = isTypeShown(t);
    if (!res) {
        return false;
    }
    //filter by readonly flag
    Document* doc = o->getDocument();
    //TODO: revise readonly filters -> use isStateLocked or hasReadonlyLock ?
    res = readOnlyFilter == TriState_Unknown ? true :
         (readOnlyFilter == TriState_Yes && !doc->isStateLocked()) || (readOnlyFilter == TriState_No && doc->isStateLocked());
    if (!res) {
        return false;
    }

    //filter by exclude list
    foreach(const QPointer<GObject>& p, excludeObjectList) {
        if (p.isNull()) {
            continue;
        }
        if (o == p.data()) {
            return false;
        }
    }

    //filter by internal obj properties
    if (!objectConstraints.isEmpty()) {
        res = true;
        foreach(const GObjectConstraints* c, objectConstraints) {
            if (o->getGObjectType() != c->objectType) {
                continue;
            }
            res = o->checkConstraints(c);
            if (!res) {
                return false;
            }
        }
    }

    //filter by name
    if (!nameFilterAcceptsString(o->getGObjectName())) {
        return false;
    }

    // check custom filter
    if (objectFilter != NULL && objectFilter->filter(o)) {
        return false;
    }

    return true;
}

bool ProjectTreeControllerModeSettings::nameFilterAcceptsString(const QString &str) const {
    foreach (const QString &token, tokensToShow) {
        if (!str.contains(token, Qt::CaseInsensitive)) {
            return false;
        }
    }
    return true;
}

bool ProjectTreeControllerModeSettings::isTypeShown(GObjectType t) const {
    if (objectTypesToShow.isEmpty()) {
        return true;
    }
    return objectTypesToShow.contains(t);
}

bool ProjectTreeControllerModeSettings::isObjectFilterActive() const {
    return !tokensToShow.isEmpty() || !objectConstraints.isEmpty() || !excludeObjectList.isEmpty()
        || !objectTypesToShow.isEmpty() || NULL != objectFilter;
}

} // U2
