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

#ifndef _U2_GOBJECT_SELECTION_H_
#define _U2_GOBJECT_SELECTION_H_

#include "SelectionTypes.h"

#include <U2Core/SelectionModel.h>

namespace U2 {

class GObject;

class  U2CORE_EXPORT GObjectSelection : public GSelection {
    Q_OBJECT
public:
    GObjectSelection(QObject* p = NULL);

    const QList<GObject*>& getSelectedObjects() const {return selectedObjects;}

    virtual bool isEmpty() const {return selectedObjects.isEmpty();}

    virtual void clear();

    void setSelection(const QList<GObject*>& objs);

    void addToSelection(const QList<GObject*>& obj);

    void addToSelection(GObject* obj);

    void removeFromSelection(GObject* obj);

    void removeFromSelection(const QList<GObject*>& obj);

    bool contains(GObject* obj) const {return selectedObjects.contains(obj);}

signals:
    void si_selectionChanged(GObjectSelection* thiz, const QList<GObject*>& objectsAdded, const QList<GObject*>& objectsRemoved);

protected:
    virtual void _append(GObject* o) {selectedObjects.append(o);}
    virtual void _remove(GObject* o) {selectedObjects.removeAll(o);};

private:
    QList<GObject*> selectedObjects;
};



/*
class Document;
class GDocumentObjectSelection : public GObjectSelection {
    Q_OBJECT
public:
    GDocumentObjectSelection(QObject* p = NULL) : GObjectSelection(p){};

protected:
    virtual void _append(GObject* o);
    virtual void _remove(GObject* o);

private slots:
    void sl_onObjectRemoved(Document* d, GObject* o);
};


class  GProjectObjectSelection : public GObjectSelection {
    Q_OBJECT
public:
    GProjectObjectSelection(QObject* p = NULL);

private slots:
    void sl_onDocumentRemoved(Document* d);
};
*/


} //namespace
#endif
