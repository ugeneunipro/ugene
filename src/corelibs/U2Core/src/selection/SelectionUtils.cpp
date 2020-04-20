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

#include "SelectionUtils.h"
#include "DocumentSelection.h"
#include "GObjectSelection.h"
#include "SelectionTypes.h"

#include <U2Core/DocumentModel.h>

#include <U2Core/GObjectUtils.h>

#include <U2Core/SelectionModel.h>

namespace U2 {

//Warn: works only for pointer type
template<class T> static void removeDuplicatesPointersFromList(QList<T*>& list) {
    QSet<const T*> tracked;
    for (int i = 0, n = list.size(); i < n; i++) {
        const T* val = list.at(i);
        if (tracked.contains(val)) {
            list[i] = NULL;
        } else {
            tracked.insert(val);
        }
    }
    list.removeAll(NULL);
}

QList<GObject*> SelectionUtils::findObjects(GObjectType t, const MultiGSelection* ms, UnloadedObjectFilter f) {
    QList<GObject*> res;
    foreach(const GSelection* s, ms->getSelections()) {
        QList<GObject*> tmp = findObjects(t, s, f);
        res += tmp;
    }

    removeDuplicatesPointersFromList(res);
    return res;
}

QList<GObject*> SelectionUtils::findObjectsKeepOrder(GObjectType t, const MultiGSelection* ms, UnloadedObjectFilter f) {
    QList<GObject*> res;
    foreach(const GSelection* s, ms->getSelections()) {
        QList<GObject*> tmp = findObjectsKeepOrder(t, s, f);
        res += tmp;
    }

    //now remove duplicates from list
    removeDuplicatesPointersFromList<GObject>(res);

    return res;
}

QList<GObject*> SelectionUtils::findObjects(GObjectType t, const GSelection* s, UnloadedObjectFilter f) {
    QList<GObject*> res = findObjectsKeepOrder(t, s, f);
    return res;
}

QList<GObject*> SelectionUtils::findObjectsKeepOrder(GObjectType t, const GSelection* s, UnloadedObjectFilter f) {
    QList<GObject*> res;
    GSelectionType stype = s->getSelectionType();
    if (stype == GSelectionTypes::DOCUMENTS) {
        const DocumentSelection* ds = qobject_cast<const DocumentSelection*>(s);
        foreach(Document* d, ds->getSelectedDocuments()) {
            const QList<GObject*>& objs = d->getObjects();
            QList<GObject*> tmp = GObjectUtils::select(objs, t, f);
            res += tmp;
        }
    } else  if (stype == GSelectionTypes::GOBJECTS) {
        const GObjectSelection* os = qobject_cast<const GObjectSelection*>(s);
        const QList<GObject*>& objs = os->getSelectedObjects();
        res = GObjectUtils::select(objs, t, f);
    }
    return res;
}


QSet<Document*> SelectionUtils::findDocumentsWithObjects(GObjectType t, const MultiGSelection* ms, UnloadedObjectFilter f, bool deriveDocsFromObjectSelection) {
    QSet<Document*> res;
    foreach(const GSelection* s, ms->getSelections()) {
        res += findDocumentsWithObjects(t, s, f, deriveDocsFromObjectSelection);
    }
    return res;
}

QSet<Document*> SelectionUtils::findDocumentsWithObjects(GObjectType t, const GSelection* s, UnloadedObjectFilter f, bool deriveDocsFromObjectSelection) {
    QSet<Document*> res;
    GSelectionType st = s->getSelectionType();
    if (st == GSelectionTypes::DOCUMENTS) {
        DocumentFormatConstraints c;
        c.supportedObjectTypes += t;
        const DocumentSelection* ds = qobject_cast<const DocumentSelection*>(s);
        const QList<Document*>& docs = ds->getSelectedDocuments();
        foreach(Document* d, docs) {
            if (!d->getObjects().isEmpty()) {
                QList<GObject*> objs = d->findGObjectByType(t, f);
                if (!objs.isEmpty()) {
                    res += d;
                }
            } else if (f == UOF_LoadedAndUnloaded && !d->isLoaded()) { //document is unloaded
                DocumentFormat* df = d->getDocumentFormat();
                if (df->checkConstraints(c)) {
                    res += d;
                }
            }
        }
    } else if (st == GSelectionTypes::GOBJECTS && deriveDocsFromObjectSelection) {
        QList<GObject*> objects = findObjects(t, s, f);
        foreach(GObject* o, objects) {
            res += o->getDocument();
        }
    }
    return res;
}

bool SelectionUtils::isDocumentInSelection(const Document* doc, const MultiGSelection& ms, bool deriveDocsFromObjectSelection) {
    foreach(const GSelection* s, ms.getSelections()) {
        GSelectionType st = s->getSelectionType();
        if (st == GSelectionTypes::DOCUMENTS) {
            const DocumentSelection* ds = qobject_cast<const DocumentSelection*>(s);
            const QList<Document*>& docs = ds->getSelectedDocuments();
            if (docs.contains((Document* const &)doc)) {//TODO? why cast
                return true;
            }
        } else if (st == GSelectionTypes::GOBJECTS && deriveDocsFromObjectSelection) {
            const GObjectSelection* os = qobject_cast<const GObjectSelection*>(s);
            const QList<GObject*>& objects = os->getSelectedObjects();
            foreach(GObject* o, objects) {
                if (o->getDocument() == doc) {
                    return true;
                }
            }
        }
    }
    return false;
}

QList<Document*> SelectionUtils::getSelectedDocs(const MultiGSelection& ms) {
    foreach(const GSelection* s, ms.getSelections()) {
        GSelectionType st = s->getSelectionType();
        if (st == GSelectionTypes::DOCUMENTS) {
            const DocumentSelection* ds = qobject_cast<const DocumentSelection*>(s);
            return  ds->getSelectedDocuments();
        }
    }
    return QList<Document*>();
}

QList<GObject*> SelectionUtils::getSelectedObjects(const MultiGSelection& ms) {
    foreach(const GSelection* s, ms.getSelections()) {
        GSelectionType st = s->getSelectionType();
        if (st == GSelectionTypes::GOBJECTS) {
            const GObjectSelection* os = qobject_cast<const GObjectSelection*>(s);
            return os->getSelectedObjects();
        }
    }
    return QList<GObject*>();
}


U2Region SelectionUtils::normalizeRegionBy3(U2Region reg, int seqLen, bool direct) {
    assert(reg.length > 0);
    if (seqLen < 3) {
        return reg;
    }
    int d = reg.length % 3;
    if (d == 1) {
        if (direct) {
            reg.length -= 1;
        } else {
            reg.startPos += 1;
            reg.length -= 1;
        }
    } else if (d == 2) {
        if (direct) {
            reg.length += (reg.length + 1 < seqLen) ? +1 : -2;
        } else {
            int prevStart = reg.startPos;
            reg.startPos += (reg.startPos > 0) ? -1 : +2;
            reg.length += prevStart - reg.startPos;
        }
    }
    return reg;
}



}//namespace
