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

#ifndef _U2_ACTOR_CFG_H_
#define _U2_ACTOR_CFG_H_

#include <QObject>
#include <QList>
#include <QPair>
#include <QString>
#include <QAbstractTableModel>

#include <U2Lang/ActorModel.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/Schema.h>
#include <U2Lang/SchemaConfig.h>
#include <U2Designer/DelegateEditors.h>

namespace U2 {

using namespace Workflow;

class ActorCfgModel : public QAbstractTableModel {
    Q_OBJECT
public:
    ActorCfgModel(QObject *parent, SchemaConfig *schemaConfig);
    ~ActorCfgModel();

    void setActor(Actor * cfg);

    void update();

    int columnCount(const QModelIndex &) const;

    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

    Qt::ItemFlags flags( const QModelIndex & index ) const;

    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    /*Used to supply item data to views and delegates.
    Generally, models only need to supply data for Qt::DisplayRole and any application-specific user roles,
    but it is also good practice to provide data for Qt::ToolTipRole, Qt::AccessibleTextRole, and Qt::AccessibleDescriptionRole.*/
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

    /*Used to modify the item of data associated with a specified model index.
    To be able to accept user input, provided by user interface elements, this function must handle data associated with Qt::EditRole.
    The implementation may also accept data associated with many different kinds of roles specified by Qt::ItemDataRole.
    After changing the item of data, models must emit the dataChanged() signal to inform other components of the change.*/
    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    void changeScriptMode(bool _mode);
    bool getScriptMode() const;

    QModelIndex modelIndexById(const QString &id) const;

    Attribute * getAttributeByRow(int row) const;
    bool isVisible(Attribute *a) const;

private:
    bool setAttributeValue( const Attribute * attr, QVariant & attrValue ) const;
    void setupAttributesScripts();
    bool canSetData(Attribute *attr, const QVariant &value);

    QMap<Attribute *, bool> getAttributeRelatedVisibility(Attribute *changedAttr,
        const QMap<Attribute *, bool> &foundRelatedAttrs = (QMap<Attribute *, bool>()) ) const;
    void checkIfAttributeVisibilityChanged(const QMap<Attribute *, bool> &attributeVisibility);

private:
    SchemaConfig*       schemaConfig;
    Actor*              subject;
    QList<Attribute*>   attrs;
    AttributeScriptDelegate * scriptDelegate;
    QVariantMap         listValues;
    bool scriptMode;

}; // ActorCfgModel

}//namespace U2

#endif // _U2_ACTOR_CFG_H_
