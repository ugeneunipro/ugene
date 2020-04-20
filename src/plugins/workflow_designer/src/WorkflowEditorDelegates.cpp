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

#include "WorkflowEditor.h"

#include "WorkflowEditorDelegates.h"

namespace U2 {

/********************************
 * ProxyDelegate
 ********************************/
QWidget *ProxyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    //if (owner->custom)
    QWidget* editor;
    {
        PropertyDelegate* itemDelegate = index.model()->data(index, DelegateRole).value<PropertyDelegate*>();
        if (itemDelegate) {
            connect(itemDelegate, SIGNAL(commitData(QWidget*)), SIGNAL(commitData(QWidget*)));
            editor = itemDelegate->createEditor(parent, option, index);
        }
        else{
            editor = QItemDelegate::createEditor(parent, option, index);
        }
    }
    return editor;
}

void ProxyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    //if (owner->custom)
    {
        PropertyDelegate* itemDelegate = index.model()->data(index, DelegateRole).value<PropertyDelegate*>();
        if (itemDelegate) {
            itemDelegate->setEditorData(editor, index);
            return;
        }
    }
    QItemDelegate::setEditorData(editor, index);
}

void ProxyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QVariant old;
    QVariant expanded;
    PropertyDelegate* propertyDelegate = model->data(index, DelegateRole).value<PropertyDelegate*>();
    if (/*owner->custom &&*/ propertyDelegate ) {
        old = model->data(index, ConfigurationEditor::ItemValueRole);
        propertyDelegate->setModelData(editor, model, index);
        expanded = model->data(index, ConfigurationEditor::ItemListValueRole);
    } else {
        old = model->data(index, Qt::EditRole);
        QItemDelegate::setModelData(editor, model, index);
    }
   /* QString name = model->data(index, DescriptorRole).value<Descriptor>().getId();
    if (handlePropertyValueList(name, expanded)) {
        return;
    }*/
    QVariant val = model->data(index, (propertyDelegate == NULL) ? (int)Qt::EditRole : (int)ConfigurationEditor::ItemValueRole);
    if (val != old) {
         QString name = model->data(index, DescriptorRole).value<Descriptor>().getId();
        if (handlePropertyValueList(name, expanded)) {
            return;
        }

        setPropertyValue(name, val);
        if (propertyDelegate) {
            model->setData(index, propertyDelegate->getDisplayValue(val), Qt::DisplayRole);
        }
        model->setData(index, model->data(index, Qt::DisplayRole).toString(), Qt::ToolTipRole);
    }
}

/********************************
 * SuperDelegate
 ********************************/
SuperDelegate::SuperDelegate(WorkflowEditor *parent) : ProxyDelegate(parent) {
}

bool SuperDelegate::handlePropertyValueList(const QString& /*name*/, QVariant /*list*/) const {
    return true;
}

} // U2
