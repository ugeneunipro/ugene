/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#pragma once

#include "ui_EditFloatMarkerWidget.h"
#include "ui_EditIntegerMarkerWidget.h"
#include "ui_EditMarkerDialog.h"
#include "ui_EditMarkerGroupDialog.h"
#include "ui_EditStringMarkerWidget.h"

namespace U2 {
namespace Workflow {
class MarkerGroupListCfgModel;
}

class Marker;
class MarkerListCfgModel;

/************************************************************************/
/* EditMarkerGroupDialog */
/************************************************************************/
class EditMarkerGroupDialog : public QDialog, public Ui_EditMarkerGroupDialog {
    Q_OBJECT
public:
    EditMarkerGroupDialog(bool isNew, Marker* marker, Workflow::MarkerGroupListCfgModel* allModel, QWidget* parent);
    ~EditMarkerGroupDialog();
    void accept() override;

    bool checkEditMarkerResult(const QString& oldName, const QString& newName, const QString& newValue, QString& message);
    bool checkAddMarkerResult(const QString& newName, const QString& newValue, QString& message);

    Marker* getMarker();

private:
    bool isNew;
    Marker* marker;
    QString oldName;
    MarkerListCfgModel* markerModel;
    Workflow::MarkerGroupListCfgModel* allModel;
    int currentTypeIndex;

    QStringList typeIds;

    void updateUi();

private slots:
    void sl_onAddButtonClicked();
    void sl_onEditButtonClicked();
    void sl_onRemoveButtonClicked();
    void sl_onTypeChanged(int newTypeIndex);
    void sl_onItemSelected(const QModelIndex& idx);
    void sl_onItemEntered(const QModelIndex& idx);
};

/************************************************************************/
/* MarkerListCfgModel */
/************************************************************************/
class MarkerListCfgModel : public QAbstractTableModel {
    Q_OBJECT
public:
    MarkerListCfgModel(QObject* parent, Marker* marker);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    int columnCount(const QModelIndex&) const override;
    int rowCount(const QModelIndex&) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

    void addMarker(const QString& valueString, const QString& name);

private:
    Marker* marker;
};

/************************************************************************/
/* EditMarkerDialog */
/************************************************************************/
class EditTypedMarkerWidget;

class EditMarkerDialog : public QDialog, public Ui_EditMarkerDialog {
    Q_OBJECT
public:
    EditMarkerDialog(bool isNew, const QString& type, const QString& name, const QVariantList& values, QWidget* parent);
    QString getName() {
        return name;
    }
    QVariantList getValues() {
        return values;
    }
    void accept() override;

private:
    bool isNew;
    QString type;
    QString name;
    QVariantList values;

    EditTypedMarkerWidget* editWidget;
};

/************************************************************************/
/* EditTypedMarkerWidget */
/************************************************************************/
class EditTypedMarkerWidget : public QWidget {
    Q_OBJECT
public:
    EditTypedMarkerWidget(const QVariantList& values, QWidget* parent)
        : QWidget(parent), values(values) {
    }
    virtual QVariantList getValues() = 0;

protected:
    QVariantList values;
};

class EditIntegerMarkerWidget : public EditTypedMarkerWidget, public Ui_EditIntegerMarkerWidget {
    Q_OBJECT
public:
    EditIntegerMarkerWidget(bool isNew, const QVariantList& values, QWidget* parent);

    QVariantList getValues() override;
};

class EditFloatMarkerWidget : public EditTypedMarkerWidget, public Ui_EditFloatMarkerWidget {
    Q_OBJECT
public:
    EditFloatMarkerWidget(bool isNew, const QVariantList& values, QWidget* parent);

    QVariantList getValues() override;
};

class EditStringMarkerWidget : public EditTypedMarkerWidget, public Ui_EditStringMarkerWidget {
    Q_OBJECT
public:
    EditStringMarkerWidget(bool isNew, const QVariantList& values, QWidget* parent);

    QVariantList getValues() override;
};

}  // namespace U2
