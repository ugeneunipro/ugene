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

#include "GObjectSelection.h"

namespace U2 {

static QList<GObject*> emptyObjs;
GObjectSelection::GObjectSelection(QObject *p) : GSelection(GSelectionTypes::GOBJECTS, p) {
    connect(this, SIGNAL(si_selectionChanged(GObjectSelection*,QList<GObject*>,QList<GObject*>)), SLOT(sl_selectionChanged()));
}

void GObjectSelection::clear() {
    QList<GObject*> tmpRemoved = selectedObjects;
    selectedObjects.clear();
    if (!tmpRemoved.isEmpty()) {
        emit si_selectionChanged(this, emptyObjs, tmpRemoved);
    }
}

void GObjectSelection::setSelection(const QList<GObject*>& objs) {
    if (objs.isEmpty()) {
        clear();
        return;
    }
    if (isEmpty()) {
        addToSelection(objs);
        return;
    }
    QList<GObject*> tmpAdded;
    QList<GObject*> tmpRemoved;

    foreach(GObject* o, objs) {
        if (!selectedObjects.contains(o)) {
            tmpAdded.append(o);
        }
    }
    foreach(GObject* o, selectedObjects) {
        if (!objs.contains(o)) {
            tmpRemoved.append(o);
        }
    }
    foreach(GObject* o, tmpRemoved) {
        selectedObjects.removeAll(o);
    }
    foreach(GObject* o, tmpAdded) {
        selectedObjects.append(o);
    }
    emit si_selectionChanged(this, tmpAdded, tmpRemoved);
}

void GObjectSelection::addToSelection(GObject* obj) {
    QList<GObject*> objs;
    objs.append(obj);
    addToSelection(objs);
}

void GObjectSelection::addToSelection(const QList<GObject*>& obj) {
    QList<GObject*> tmpAdded;
    int sBefore = selectedObjects.size();
    foreach(GObject* o, obj) {
        if (!selectedObjects.contains(o)) {
            tmpAdded.append(o);
            selectedObjects.append(o);
        }
    }
    if (selectedObjects.size() != sBefore) {
        emit si_selectionChanged(this, tmpAdded, emptyObjs);
    }
}

void GObjectSelection::removeFromSelection(GObject* obj) {
    QList<GObject*> l;
    l.append(obj);
    removeFromSelection(l);
}

void GObjectSelection::removeFromSelection(const QList<GObject*>& obj) {
    QList<GObject*> tmpRemoved;
    int sBefore = selectedObjects.size();
    foreach(GObject* o, obj) {
        if (selectedObjects.removeAll(o)!=0) {
            tmpRemoved.push_back(o);
        }
    }
    if (selectedObjects.size() != sBefore) {
        emit si_selectionChanged(this, emptyObjs, tmpRemoved);
    }
}
/*
//////////////////////////////////////////////////////////////////////////
/// GDocumentObjectSelection

void GDocumentObjectSelection::_append(GObject* o) {
    Document* d = o->getDocument();
    assert(d!=NULL);
    connect(d, SIGNAL(si_objectRemoved(GObject*)), SLOT(void sl_onObjectRemoved(GObject*)));
    GObjectSelection::_append(o);
}

void GDocumentObjectSelection::_remove(GObject* o) {
    o->disconnect(this);
    GObjectSelection::_remove(o);
}

void GDocumentObjectSelection::sl_onObjectRemoved(GObject* o) {
    remove(o);
}

//////////////////////////////////////////////////////////////////////////
/// GProjectObjectSelection

*/

}//namespace
