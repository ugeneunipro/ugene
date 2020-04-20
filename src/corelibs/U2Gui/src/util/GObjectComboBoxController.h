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

#ifndef _U2_GOBJECT_COMBOBOX_CONTROLLER_H_
#define _U2_GOBJECT_COMBOBOX_CONTROLLER_H_

#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectReference.h>

#include <QComboBox>

namespace U2 {

//TODO: use custom class for objects filtering, share it with ProjectTreeController

class GObjectComboBoxControllerConstraints {
public:
    GObjectComboBoxControllerConstraints() {
        onlyWritable = false;
        uof = UOF_LoadedOnly;
    }

    GObjectType             typeFilter;
    GObjectRelation         relationFilter;
    bool                    onlyWritable;
    UnloadedObjectFilter    uof;
};

class U2GUI_EXPORT GObjectComboBoxController : public QObject {
    Q_OBJECT
public:
    GObjectComboBoxController(QObject* p, const GObjectComboBoxControllerConstraints& c, QComboBox* combo);

    bool setSelectedObject(GObject* obj) {return setSelectedObject(GObjectReference(obj));}
    bool setSelectedObject(const GObjectReference& ref);

    GObjectReference getSelectedObjectReference() const;
    GObject* getSelectedObject() const;

    void updateConstrains(const GObjectComboBoxControllerConstraints& c);

signals:
    void si_comboBoxChanged();

private slots:
    void sl_onDocumentAdded(Document* d);
    void sl_onDocumentRemoved(Document* d);
    void sl_onObjectAdded(GObject* o);
    void sl_onObjectRemoved(GObject* o);
    void sl_lockedStateChanged();

private:
    void updateCombo();
    void connectDocument(Document *document);
    void addObject(GObject*);
    void removeObject(const GObjectReference& ref);
    void addDocumentObjects(Document* d);
    void removeDocumentObjects(Document* d);
    QString itemText(GObject* o);

    GObjectComboBoxControllerConstraints settings;
    QComboBox*          combo;
    QIcon               objectIcon;
    QIcon               unloadedObjectIcon;
};

}//namespace

#endif
