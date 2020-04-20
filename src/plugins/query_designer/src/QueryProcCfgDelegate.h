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

/************************************************************************/
/* Copy of "workflow_designer/ProxyDelegate.h"                          */
/************************************************************************/

#ifndef _U2_QUERY_PROC_CFG_DELEGATE_H_
#define _U2_QUERY_PROC_CFG_DELEGATE_H_

#include <QItemDelegate>

#include <U2Lang/ConfigurationEditor.h>

#include "QueryEditor.h"

Q_DECLARE_METATYPE(U2::PropertyDelegate*)

namespace U2 {


enum {
    DelegateRole = Qt::UserRole + 100,
    DescriptorRole
};

class QueryProcCfgDelegate : public QItemDelegate {
public:
    QueryProcCfgDelegate(QueryEditor* parent) : QItemDelegate(parent) {}

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
        PropertyDelegate* d = index.model()->data(index, DelegateRole).value<PropertyDelegate*>();
        if (d) {
            connect(d, SIGNAL(commitData(QWidget*)), SIGNAL(commitData(QWidget*)));
            return d->createEditor(parent, option, index);
        }
        return QItemDelegate::createEditor(parent, option, index);
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const {
        PropertyDelegate* d = index.model()->data(index, DelegateRole).value<PropertyDelegate*>();
        if (d) {
            d->setEditorData(editor, index);
            return;
        }
        QItemDelegate::setEditorData(editor, index);
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
        QVariant old;
        PropertyDelegate* d = model->data(index, DelegateRole).value<PropertyDelegate*>();
        if (d) {
            old = model->data(index, ConfigurationEditor::ItemValueRole);
            d->setModelData(editor, model, index);
        } else {
            old = model->data(index, Qt::EditRole);
            QItemDelegate::setModelData(editor, model, index);
        }

        QVariant val = model->data(index, (d == NULL) ? (int)Qt::EditRole : (int)ConfigurationEditor::ItemValueRole);
        if (val != old) {
            if (d) {
                model->setData(index, d->getDisplayValue(val), Qt::DisplayRole);
            }
            model->setData(index, model->data(index, Qt::DisplayRole).toString(), Qt::ToolTipRole);
        }
    }
};

}//namespace

#endif
