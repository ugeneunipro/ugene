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

#ifndef _U2_MARKER_EDITOR_H_
#define _U2_MARKER_EDITOR_H_

#include <U2Lang/ActorModel.h>
#include <U2Lang/Configuration.h>
#include <U2Lang/ConfigurationEditor.h>

namespace U2 {

class Marker;

namespace Workflow {

class MarkerGroupListCfgModel;

/**
 * base class for marker-actors' configuration editor
 */
class U2DESIGNER_EXPORT MarkerEditor : public ActorConfigurationEditor {
    Q_OBJECT
public:
    MarkerEditor();
    MarkerEditor(const MarkerEditor &) : ActorConfigurationEditor(), markerModel(NULL) {}
    virtual ~MarkerEditor();
    virtual QWidget *getWidget();
    virtual void setConfiguration(Actor *actor);
    virtual ConfigurationEditor *clone() {return new MarkerEditor(*this);}

public slots:
    void sl_onMarkerEdited(const QString &newMarkerName, const QString &oldMarkerName);
    void sl_onMarkerAdded(const QString &markerName);
    void sl_onMarkerRemoved(const QString &markerName);

private:
    MarkerGroupListCfgModel *markerModel;

private:
    QWidget * createGUI();
}; // MarkerEditor

class MarkerGroupListCfgModel : public QAbstractTableModel {
    Q_OBJECT
public:
    MarkerGroupListCfgModel(QObject *parent, QList<Marker*> &markers);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    int columnCount(const QModelIndex &) const;
    int rowCount(const QModelIndex &) const;
    Qt::ItemFlags flags( const QModelIndex & index ) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    Marker * getMarker(int row) const;
    Marker * getMarker(const QString &markerName) const;
    QList<Marker*> & getMarkers();
    void addMarker(Marker *newMarker);
    void replaceMarker(int row, Marker *newMarker);
    QString suggestName(const QString &type);
    bool containsName(const QString &name);

signals:
    void si_markerEdited(const QString &newMarkerName, const QString &oldMarkerName);
    void si_markerAdded(const QString &markerName);
    void si_markerRemoved(const QString &markerName);

private:
    QList<Marker*> &markers;
};

} // Workflow

} //U2

#endif // _U2_MARKER_EDITOR_H_
