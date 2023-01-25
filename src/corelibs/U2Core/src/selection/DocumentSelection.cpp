/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include <U2Core/AppContext.h>
#include <U2Core/ProjectModel.h>

namespace U2 {

static QList<Document*> emptyDocs;
DocumentSelection::DocumentSelection(QObject* p)
    : GSelection(GSelectionTypes::DOCUMENTS, p) {
    connect(this, SIGNAL(si_selectionChanged(DocumentSelection*, QList<Document*>, QList<Document*>)), SLOT(sl_selectionChanged()));
}
const QList<Document*>& DocumentSelection::getSelectedDocuments() const {
    return selectedDocs;
}

bool DocumentSelection::isEmpty() const {
    return selectedDocs.isEmpty();
}

bool DocumentSelection::contains(Document* doc) const {
    return selectedDocs.contains(doc);
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

    QList<Document*> addedDocuments;
    QList<Document*> removedDocuments;

    for (Document* document : qAsConst(docs)) {
        if (!selectedDocs.contains(document)) {
            addedDocuments.append(document);
        }
    }
    for (Document* document : qAsConst(selectedDocs)) {
        if (!docs.contains(document)) {
            removedDocuments.append(document);
        }
    }
    for (Document* document : qAsConst(removedDocuments)) {
        selectedDocs.removeAll(document);
    }
    for (Document* document : qAsConst(addedDocuments)) {
        selectedDocs.append(document);
    }
    if (!addedDocuments.isEmpty() || !removedDocuments.isEmpty()) {
        emit si_selectionChanged(this, addedDocuments, removedDocuments);
    }
}

void DocumentSelection::addToSelection(const QList<Document*>& documentsToAdd) {
    QList<Document*> addedDocuments;
    int documentCountBefore = selectedDocs.size();
    for (Document* document : qAsConst(documentsToAdd)) {
        if (!selectedDocs.contains(document)) {
            addedDocuments.append(document);
            selectedDocs.append(document);
        }
    }
    if (selectedDocs.size() != documentCountBefore) {
        emit si_selectionChanged(this, addedDocuments, emptyDocs);
    }
}

void DocumentSelection::removeFromSelection(const QList<Document*>& documentsToRemove) {
    QList<Document*> removedDocuments;
    int documentCountBefore = selectedDocs.size();
    for (Document* document : qAsConst(documentsToRemove)) {
        if (selectedDocs.removeAll(document) != 0) {
            removedDocuments.append(document);
        }
    }
    if (selectedDocs.size() != documentCountBefore) {
        emit si_selectionChanged(this, emptyDocs, removedDocuments);
    }
}

}  // namespace U2
