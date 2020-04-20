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

#ifndef _U2_DOCUMENT_SELECTION_H_
#define _U2_DOCUMENT_SELECTION_H_

#include "SelectionTypes.h"

#include <U2Core/SelectionModel.h>


namespace U2 {

class Document;

class U2CORE_EXPORT DocumentSelection : public GSelection {
    Q_OBJECT
public:
    DocumentSelection(QObject* p = NULL);

    const QList<Document*>& getSelectedDocuments() const {return selectedDocs;}

    virtual bool isEmpty() const {return selectedDocs.isEmpty();}

    virtual void clear();

    void setSelection(const QList<Document*>& docs);

    void addToSelection(const QList<Document*>& docs);

    void removeFromSelection(const QList<Document*>& docs);

    bool contains(Document* doc) const {return selectedDocs.contains(doc);}

signals:
    void si_selectionChanged(DocumentSelection* thiz, const QList<Document*>& docsAdded, const QList<Document*>& docsRemoved);

private:
    QList<Document*> selectedDocs;
};

/*
class  ProjectDocumentSelection : public DocumentSelection {
    Q_OBJECT

public:
    ProjectDocumentSelection(QObject* parent = NULL);

private slots:
    void sl_onDocumentRemoved(Document*);
};
*/

}//namespace

#endif
