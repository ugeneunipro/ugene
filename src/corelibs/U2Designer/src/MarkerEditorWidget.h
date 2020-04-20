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

#ifndef _MARKER_EDITOR_WIDGET_H_
#define _MARKER_EDITOR_WIDGET_H_

#include "ui_MarkerEditorWidget.h"
#include "ui_EditMarkerDialog.h"
#include "ui_EditIntegerMarkerWidget.h"

namespace U2 {

class Marker;

class MarkerEditorWidget: public QWidget, public Ui_MarkerEditorWidget {
    Q_OBJECT
public:
    MarkerEditorWidget(QAbstractTableModel *markerModel, QWidget *parent = NULL);
    bool checkEditMarkerGroupResult(const QString &oldName, Marker *newMarker, QString &message);
    bool checkAddMarkerGroupResult(Marker *newMarker, QString &message);

private:
    QAbstractTableModel *markerModel;

private slots:
    void sl_onAddButtonClicked();
    void sl_onEditButtonClicked();
    void sl_onRemoveButtonClicked();
    void sl_onItemSelected(const QModelIndex &idx);
    void sl_onItemEntered(const QModelIndex &idx);
};

} // U2

#endif // _MARKER_EDITOR_WIDGET_H_
