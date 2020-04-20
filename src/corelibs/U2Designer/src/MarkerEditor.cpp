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

#include <QHeaderView>
#include <QSplitter>
#include <QTableView>

#include <U2Core/U2SafePoints.h>

#include <U2Designer/MarkerEditorWidget.h>

#include <U2Lang/BaseTypes.h>
#include <U2Lang/Marker.h>
#include <U2Lang/MarkerAttribute.h>

#include "MarkerEditor.h"

namespace U2 {

namespace Workflow {

/* ***********************************************************************
 * MarkerEditor
 * ***********************************************************************/
MarkerEditor::MarkerEditor()
: ActorConfigurationEditor(), markerModel(NULL)
{
}

MarkerEditor::~MarkerEditor() {
}

QWidget *MarkerEditor::getWidget() {
    return createGUI();
}

QWidget *MarkerEditor::createGUI() {
    if (NULL == markerModel) {
        return NULL;
    }

    return new MarkerEditorWidget(markerModel);
}

void MarkerEditor::setConfiguration(Actor *actor) {
    ActorConfigurationEditor::setConfiguration(actor);
    QMap<QString, Attribute*> attrs = cfg->getParameters();

    MarkerAttribute *mAttr = NULL;
    foreach (QString key, attrs.keys()) {
        Attribute *attr = attrs.value(key);
        if (MARKER_GROUP == attr->getGroup()) {
            if (NULL != mAttr) {
                assert(0);
                mAttr = NULL;
                break;
            }
            mAttr = dynamic_cast<MarkerAttribute*>(attr);
        }
    }

    if (NULL == mAttr) {
        return;
    }

    markerModel = new MarkerGroupListCfgModel(this, mAttr->getMarkers());
    connect(markerModel, SIGNAL(si_markerEdited(const QString &, const QString &)), SLOT(sl_onMarkerEdited(const QString &, const QString &)));
    connect(markerModel, SIGNAL(si_markerAdded(const QString &)), SLOT(sl_onMarkerAdded(const QString &)));
    connect(markerModel, SIGNAL(si_markerRemoved(const QString &)), SLOT(sl_onMarkerRemoved(const QString &)));
}

void MarkerEditor::sl_onMarkerEdited(const QString &newMarkerName, const QString &oldMarkerName) {
    Marker *marker = markerModel->getMarker(newMarkerName);
    SAFE_POINT(NULL != marker, "NULL marker", );

    { // TODO: make common way to get marked object output port
        assert(1 == cfg->getOutputPorts().size());
        Port *outPort = cfg->getOutputPorts().at(0);
        assert(outPort->getOutputType()->isMap());
        QMap<Descriptor, DataTypePtr> outTypeMap = outPort->getOutputType()->getDatatypesMap();

        Descriptor newSlot = MarkerSlots::getSlotByMarkerType(marker->getType(), marker->getName());
        outTypeMap.remove(oldMarkerName);
        outTypeMap[newSlot] = BaseTypes::STRING_TYPE();
        DataTypePtr newType(new MapDataType(dynamic_cast<Descriptor&>(*(outPort->getType())), outTypeMap));
        outPort->setNewType(newType);
    }
    emit si_configurationChanged();
}

void MarkerEditor::sl_onMarkerAdded(const QString &markerName) {
    Marker *marker = markerModel->getMarker(markerName);
    SAFE_POINT(NULL != marker, "NULL marker", );

    { // TODO: make common way to get marked object output port
        assert(1 == cfg->getOutputPorts().size());
        Port *outPort = cfg->getOutputPorts().at(0);
        assert(outPort->getOutputType()->isMap());
        QMap<Descriptor, DataTypePtr> outTypeMap = outPort->getOutputType()->getDatatypesMap();

        Descriptor newSlot = MarkerSlots::getSlotByMarkerType(marker->getType(), marker->getName());
        outTypeMap[newSlot] = BaseTypes::STRING_TYPE();
        DataTypePtr newType(new MapDataType(dynamic_cast<Descriptor&>(*(outPort->getType())), outTypeMap));
        outPort->setNewType(newType);
    }
}

void MarkerEditor::sl_onMarkerRemoved(const QString &markerName) {
    { // TODO: make common way to get marked object output port
        assert(1 == cfg->getOutputPorts().size());
        Port *outPort = cfg->getOutputPorts().at(0);
        assert(outPort->getOutputType()->isMap());
        QMap<Descriptor, DataTypePtr> outTypeMap = outPort->getOutputType()->getDatatypesMap();

        outTypeMap.remove(markerName);
        DataTypePtr newType(new MapDataType(dynamic_cast<Descriptor&>(*(outPort->getType())), outTypeMap));
        outPort->setNewType(newType);
    }
    emit si_configurationChanged();
}

/* ***********************************************************************
* MarkerCfgModel
* ***********************************************************************/
MarkerGroupListCfgModel::MarkerGroupListCfgModel(QObject *parent, QList<Marker*> &markers)
: QAbstractTableModel(parent), markers(markers)
{

}

QVariant MarkerGroupListCfgModel::data(const QModelIndex &index, int role) const {
    if (Qt::DisplayRole == role || Qt::ToolTipRole == role) {
        Marker *m = *(markers.begin()+index.row());
        if (NULL == m) {
            return QVariant();
        }

        if (0 == index.column()) {
            return m->getName();
        } else if (1 == index.column()) {
            return m->toString();
        }
    } else {
        return QVariant();
    }
    return QVariant();
}

int MarkerGroupListCfgModel::columnCount(const QModelIndex &) const {
    return 2;
}

int MarkerGroupListCfgModel::rowCount(const QModelIndex &) const {
    return markers.size();
}

Qt::ItemFlags MarkerGroupListCfgModel::flags( const QModelIndex & ) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant MarkerGroupListCfgModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
        case 0:
            return tr("Marker Group");
        case 1:
            return tr("Marker Value");
        default:
            assert(false);
        }
    }
    // unreachable code
    return QVariant();
}

bool MarkerGroupListCfgModel::setData( const QModelIndex & index, const QVariant & value, int role ) {
    Q_UNUSED(index); Q_UNUSED(value); Q_UNUSED(role);
    return true;
}

bool MarkerGroupListCfgModel::removeRows(int row, int count, const QModelIndex &) {
    if (1 != count) {
        return true;
    }
    Marker *toRemove = markers.at(row);
    QString markerName = toRemove->getName();
    beginRemoveRows(QModelIndex(), row, row+count-1);
    markers.removeAt(row);
    endRemoveRows();

    emit si_markerRemoved(markerName);

    return true;
}

Marker * MarkerGroupListCfgModel::getMarker(int row) const {
    SAFE_POINT(row < markers.size(), "Markers: out of range", NULL);
    return markers.at(row);
}

Marker * MarkerGroupListCfgModel::getMarker(const QString &markerName) const {
    foreach (Marker *marker, markers) {
        if (marker->getName() == markerName) {
            return marker;
        }
    }
    return NULL;
}


QList<Marker*> & MarkerGroupListCfgModel::getMarkers() {
    return markers;
}

void MarkerGroupListCfgModel::addMarker(Marker *newMarker) {
    beginInsertRows(QModelIndex(), markers.size(), markers.size());
    markers.insert(markers.size(), newMarker);
    endInsertRows();

    emit si_markerAdded(newMarker->getName());
}

void MarkerGroupListCfgModel::replaceMarker(int row, Marker *newMarker) {
    Marker *oldMarker = getMarker(row);
    CHECK(NULL != oldMarker, );

    beginRemoveRows(QModelIndex(), row, row);
    markers.removeAt(row);
    endRemoveRows();

    QString oldName = oldMarker->getName();
    delete oldMarker;

    beginInsertRows(QModelIndex(), row, row);
    markers.insert(row, newMarker);
    endInsertRows();

    emit si_markerEdited(newMarker->getName(), oldName);
}

QString MarkerGroupListCfgModel::suggestName(const QString &type) {
    QString result = type;
    int idx = 1;
    while (containsName(result)) {
        result = type + "-" + QString::number(idx);
        idx++;
    }
    return result;
}

bool MarkerGroupListCfgModel::containsName(const QString &name) {
    foreach (Marker *m, getMarkers()) {
        if (m->getName() == name) {
            return true;
        }
    }
    return false;
}

} // Workflow

} // U2
