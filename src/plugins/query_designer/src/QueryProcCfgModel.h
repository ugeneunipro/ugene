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

#ifndef _U2_QUERY_PROC_CFG_MODEL_H_
#define _U2_QUERY_PROC_CFG_MODEL_H_

#include "QueryProcCfgDelegate.h"

#include <U2Lang/Attribute.h>
#include <U2Lang/Configuration.h>
#include <U2Lang/WorkflowUtils.h>

#include <QAbstractTableModel>


namespace U2 {

class QueryProcCfgModel : public QAbstractTableModel {
    Q_OBJECT
private:
    QList<Attribute*>       attrs;
    ConfigurationEditor*    editor;
    Configuration*          cfg;
public:
    QueryProcCfgModel(QObject* parent) : QAbstractTableModel(parent), editor(NULL), cfg(NULL) {}

    QModelIndex modelIndexById(const QString& id) {
        foreach(Attribute* a, attrs) {
            if (a->getId()==id) {
                int row = attrs.indexOf(a);
                QModelIndex modelIndex = index(row, 1);
                return modelIndex;
            }
        }
        return QModelIndex();
    }

    void setConfiguration(Configuration* cfg) {
        beginResetModel();
        this->cfg = cfg;
        if (cfg) {
            editor = cfg->getEditor();
            attrs = cfg->getAttributes();
        } else {
            editor = NULL;
            attrs.clear();
        }
        endResetModel();
    }

    void setConfiguration(ConfigurationEditor* ed, const QList<Attribute*>& al) {
        beginResetModel();
        editor = ed;
        attrs = al;
        endResetModel();
    }

    int columnCount(const QModelIndex&) const { return 2; }

    int rowCount(const QModelIndex&) const { return attrs.size(); }

    Qt::ItemFlags flags (const QModelIndex& index) const {
        if (index.column() == 0) {
            return Qt::ItemIsEnabled;
        }
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            switch(section) {
                case 0: return tr("Name");
                case 1: return tr("Value");
            }
        }
        return QVariant();
    }

    QVariant data(const QModelIndex &index, int role /* = Qt::DisplayRole */) const {
        const Attribute* item = attrs.at(index.row());
        if (role == DescriptorRole) {
            return qVariantFromValue<Descriptor>(*item);
        }
        if (index.column() == 0) {
            switch (role) {
            case Qt::DisplayRole: return item->getDisplayName();
            case Qt::ToolTipRole: return item->getDocumentation();
            case Qt::FontRole:
                if (item->isRequiredAttribute()) {
                    QFont fnt;
                    fnt.setBold(true);
                    return QVariant(fnt);
                }
                return QVariant();
            default:
                return QVariant();
            }
        }
        QVariant val = item->getAttributePureValue();
        PropertyDelegate* pd = editor ? editor->getDelegate(item->getId()) : NULL;
        switch (role) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            {
                if(pd) {
                    return pd->getDisplayValue(val);
                } else {
                    QString valueStr = WorkflowUtils::getStringForParameterDisplayRole(val);
                    return !valueStr.isEmpty() ? valueStr : val;
                }
            }
        case DelegateRole:
            return qVariantFromValue<PropertyDelegate*>(pd);
        case Qt::EditRole:
        case ConfigurationEditor::ItemValueRole:
            return val;
        }
        return QVariant();
    }

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole ) {
        if (index.column() != 1) {
            return false;
        }
        Attribute* item = attrs[index.row()];
        switch (role) {
        case Qt::EditRole:
        case ConfigurationEditor::ItemValueRole:
            const QString& key = item->getId();
            if (item->getAttributePureValue() != value) {
                cfg->setParameter(key, value);
                emit dataChanged(index, index);
            }
            return true;
        }
        return false;
    }
};

}//namespace

#endif
