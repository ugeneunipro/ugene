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

#include "DocumentFormatComboboxController.h"

#include <U2Core/AppContext.h>

namespace U2 {

DocumentFormatComboboxController::DocumentFormatComboboxController(QObject* p, QComboBox* cb,
    const DocumentFormatConstraints& _c, DocumentFormatId active)
    : QObject(p), combo(cb), c(_c)
{
    assert(combo->count()==0);

    DocumentFormatRegistry* fr = AppContext::getDocumentFormatRegistry();
    connect(fr, SIGNAL(si_documentFormatRegistered(DocumentFormat*)), SLOT(sl_onDocumentFormatRegistered(DocumentFormat*)));
    connect(fr, SIGNAL(si_documentFormatUnregistered(DocumentFormat*)), SLOT(sl_onDocumentFormatUnregistered(DocumentFormat*)));

    updateCombo(active);
}

void DocumentFormatComboboxController::sl_onDocumentFormatRegistered(DocumentFormat* f) {
    if (!f->checkConstraints(c)) {
        return;
    }
    combo->addItem(QIcon(), f->getFormatName(), f->getFormatId());
}

void DocumentFormatComboboxController::sl_onDocumentFormatUnregistered(DocumentFormat* f) {
    for (int i=0;i<combo->count();i++) {
        DocumentFormatId id = combo->itemData(i).toString();
        if (id == f->getFormatId()) {
            combo->removeItem(i);
            return;
        }
    }
}

DocumentFormatId DocumentFormatComboboxController::getActiveFormatId() const {
    return getActiveFormatId(combo);
}

void DocumentFormatComboboxController::setActiveFormatId(DocumentFormatId id) {
    for (int i=0, n=combo->count(); i<n; i++) {
        DocumentFormatId tmpId = combo->itemData(i).toString();
        if (id == tmpId) {
            combo->setCurrentIndex(i);
            return;
        }
    }
    assert(0);
}

QList<DocumentFormatId> DocumentFormatComboboxController::getFormatsInCombo() {
    return getFormatsInCombo(combo);
}

void DocumentFormatComboboxController::updateConstraints(const DocumentFormatConstraints& _c) {
    c = _c;
    updateCombo(getActiveFormatId());
}

void DocumentFormatComboboxController::updateCombo(DocumentFormatId active) {
    DocumentFormatRegistry* fr = AppContext::getDocumentFormatRegistry();
    QList<DocumentFormatId> selectedFormats = fr->selectFormats(c);
    fill(combo, selectedFormats, active);
}

void DocumentFormatComboboxController::fill(QComboBox* combo, QList<DocumentFormatId>& formatIds, DocumentFormatId active) {
    combo->clear();
    DocumentFormatRegistry* fr = AppContext::getDocumentFormatRegistry();
    foreach(DocumentFormatId id, formatIds) {
        combo->addItem(QIcon(), fr->getFormatById(id)->getFormatName(), id);
        if (id == active) {
            combo->setCurrentIndex(combo->count()-1);
        }
    }
    combo->model()->sort(0);
}

DocumentFormatId DocumentFormatComboboxController::getActiveFormatId(QComboBox* combo) {
    int i = combo->currentIndex();
    if (i==-1) {
        return DocumentFormatId::null;
    }
    DocumentFormatId id = combo->itemData(i).toString();
    return id;
}

QList<DocumentFormatId> DocumentFormatComboboxController::getFormatsInCombo(QComboBox* combo) {
    QList<DocumentFormatId> res;
    for (int i = 0, n = combo->count(); i < n; i++) {
        DocumentFormatId id = combo->itemData(i).toString();
        res.append(id);
    }
    return res;
}


}//namespace

