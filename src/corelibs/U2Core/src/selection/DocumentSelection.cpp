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

#include "DocumentSelection.h"

#include <U2Core/ProjectModel.h>
#include <U2Core/AppContext.h>

namespace U2 {

static QList<Document*> emptyDocs;
DocumentSelection::DocumentSelection(QObject *p) : GSelection(GSelectionTypes::DOCUMENTS, p) {
    connect(this, SIGNAL(si_selectionChanged(DocumentSelection*,QList<Document*>,QList<Document*>)), SLOT(sl_selectionChanged()));
}

void DocumentSelection::clear() {
    QList<Document*> tmpRemoved = selectedDocs;
    selectedDocs.clear();
    if (!tmpRemoved.isEmpty()) {
        emit si_selectionChanged(this, emptyDocs, tmpRemoved);
    }
}

void DocumentSelection::setSelection(const QList<Document*>& docs) {
    if (docs.isEmpty()) {
        clear();
        return;
    }
    if (isEmpty()) {
        addToSelection(docs);
        return;
    }
    QList<Document*> tmpAdded;
    QList<Document*> tmpRemoved;

    foreach(Document* d, docs) {
        if (!selectedDocs.contains(d)) {
            tmpAdded.push_back(d);
        }
    }
    foreach(Document* d, selectedDocs) {
        if (!docs.contains(d)) {
            tmpRemoved.push_back(d);
        }
    }
    foreach(Document* d, tmpRemoved) {
        selectedDocs.removeAll(d);
    }
    foreach(Document* d, tmpAdded) {
        selectedDocs.push_back(d);
    }
    emit si_selectionChanged(this, tmpAdded, tmpRemoved);
}


void DocumentSelection::addToSelection(const QList<Document*>& docs) {
    QList<Document*> tmpAdded;
    int sBefore = selectedDocs.size();
    foreach(Document* d, docs) {
        if (!selectedDocs.contains(d)) {
            tmpAdded.push_back(d);
            selectedDocs.push_back(d);
        }
    }
    if (selectedDocs.size()!=sBefore) {
        emit si_selectionChanged(this, tmpAdded, emptyDocs);
    }
}

void DocumentSelection::removeFromSelection(const QList<Document*>& docs) {
    QList<Document*> tmpRemoved;
    int sBefore = selectedDocs.size();
    foreach(Document* d, docs) {
        if (selectedDocs.removeAll(d)!=0) {
            tmpRemoved.push_back(d);
        }
    }
    if (selectedDocs.size()!=sBefore) {
        emit si_selectionChanged(this, emptyDocs, tmpRemoved);
    }
}


/*
ProjectDocumentSelection::ProjectDocumentSelection(QObject* parent) : DocumentSelection(parent) {
    Project* p = AppContext::getProject();
    connect(p, SIGNAL(si_documentRemoved(Document*)), SLOT(sl_onDocumentRemoved(Document*)));
}

void ProjectDocumentSelection::sl_onDocumentRemoved(Document* d) {
    remove(QList<Document*>()<<d);
}*/

}//namespace
