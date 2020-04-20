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

#ifndef _U2_WORKFLOW_DESIGNER_EDITOR_DELEGATES_H_
#define _U2_WORKFLOW_DESIGNER_EDITOR_DELEGATES_H_

#include <U2Lang/ConfigurationEditor.h>
#include <QItemDelegate>

Q_DECLARE_METATYPE(U2::PropertyDelegate*)

namespace U2 {

class WorkflowEditor;

enum WorkflowEditorRoles {
    DelegateRole = Qt::UserRole + 100,
    DescriptorRole
};

class ProxyDelegate : public QItemDelegate {
public:
    ProxyDelegate(QWidget *parent = 0) : QItemDelegate(parent) {}

    virtual void setPropertyValue(const QString& name, QVariant val) const { Q_UNUSED(name);Q_UNUSED(val); }
    virtual bool handlePropertyValueList(const QString& name, QVariant list) const { Q_UNUSED(name);Q_UNUSED(list); return false; }

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

}; // ProxyDelegate

class SuperDelegate : public ProxyDelegate {
public:
    SuperDelegate(WorkflowEditor *parent);

    virtual bool handlePropertyValueList(const QString& name, QVariant list) const;
}; // SuperDelegate

} // U2

#endif //_U2_WORKFLOW_DESIGNER_EDITOR_DELEGATES_H_
