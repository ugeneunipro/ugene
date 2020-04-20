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

#include <QMessageBox>
#include <QPushButton>

#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/MarkerEditor.h>
#include <U2Designer/MarkerEditorWidget.h>

#include <U2Gui/HelpButton.h>

#include <U2Lang/Marker.h>
#include <U2Lang/MarkerUtils.h>

#include "EditMarkerGroupDialog.h"

namespace U2 {

/************************************************************************/
/* EditMarkerGroupDialog */
/************************************************************************/
EditMarkerGroupDialog::EditMarkerGroupDialog(bool isNew, Marker *marker, Workflow::MarkerGroupListCfgModel *_allModel, QWidget *parent)
    : QDialog(parent), isNew(isNew), marker(NULL), allModel(_allModel), currentTypeIndex(-1)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "28967144");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    {
        QStringList types;
        types << MarkerTypes::SEQ_LENGTH().getDisplayName(); typeIds << MarkerTypes::SEQ_LENGTH().getId();
        types << MarkerTypes::SEQ_NAME().getDisplayName(); typeIds << MarkerTypes::SEQ_NAME().getId();
        types << MarkerTypes::ANNOTATION_COUNT().getDisplayName(); typeIds << MarkerTypes::ANNOTATION_COUNT().getId();
        //types << MarkerTypes::ANNOTATION_LENGTH().getDisplayName(); typeIds << MarkerTypes::ANNOTATION_LENGTH().getId();
        types << MarkerTypes::QUAL_INT_VALUE().getDisplayName(); typeIds << MarkerTypes::QUAL_INT_VALUE().getId();
        types << MarkerTypes::QUAL_TEXT_VALUE().getDisplayName(); typeIds << MarkerTypes::QUAL_TEXT_VALUE().getId();
        types << MarkerTypes::QUAL_FLOAT_VALUE().getDisplayName(); typeIds << MarkerTypes::QUAL_FLOAT_VALUE().getId();
        types << MarkerTypes::TEXT().getDisplayName(); typeIds << MarkerTypes::TEXT().getId();
        typeBox->addItems(types);
        typeBox->setCurrentIndex(0);

        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
        table->horizontalHeader()->setSectionsClickable(false);
        table->horizontalHeader()->setStretchLastSection(true);
        table->verticalHeader()->hide();
        table->verticalHeader()->setDefaultSectionSize(QFontMetrics(QFont()).height() + 6);
    }

    if (!isNew) {
        this->marker = marker->clone();
        oldName = marker->getName();
        markerGroupNameEdit->setText(marker->getName());
        int res = typeIds.indexOf(marker->getType());
        if (-1 == res) {
            typeBox->setCurrentIndex(0);
        } else {
            typeBox->setCurrentIndex(res);
        }
    } else {
        this->setWindowTitle(tr("Create Marker Group"));
        this->marker = MarkerFactory::createInstanse(typeIds.at(typeBox->currentIndex()), QVariant());
        sl_onTypeChanged(typeBox->currentIndex());
    }
    currentTypeIndex = typeBox->currentIndex();

    editButton->setEnabled(false);
    removeButton->setEnabled(false);

    connect(addButton, SIGNAL(clicked()), SLOT(sl_onAddButtonClicked()));
    connect(editButton, SIGNAL(clicked()), SLOT(sl_onEditButtonClicked()));
    connect(removeButton, SIGNAL(clicked()), SLOT(sl_onRemoveButtonClicked()));
    connect(typeBox, SIGNAL(currentIndexChanged(int)), SLOT(sl_onTypeChanged(int)));
    connect(table, SIGNAL(entered(const QModelIndex &)), SLOT(sl_onItemEntered(const QModelIndex &)));
    connect(table, SIGNAL(pressed(const QModelIndex &)), SLOT(sl_onItemSelected(const QModelIndex &)));

    updateUi();
}

void EditMarkerGroupDialog::updateUi() {
    markerModel = new MarkerListCfgModel(this, this->marker);
    table->setModel(markerModel);

    if (NONE != marker->hasAdditionalParameter()) {
        addParamLabel->setText(marker->getAdditionalParameterName()+":");
        addParamEdit->setText(marker->getAdditionalParameter().toString());
        addParamLabel->setVisible(true);
        addParamEdit->setVisible(true);
    } else {
        addParamLabel->setVisible(false);
        addParamEdit->setVisible(false);
    }
}

EditMarkerGroupDialog::~EditMarkerGroupDialog() {
    delete marker;
}

Marker *EditMarkerGroupDialog::getMarker() {
    return marker->clone();
}

void EditMarkerGroupDialog::sl_onItemEntered(const QModelIndex &idx) {
    Qt::MouseButtons bs = QApplication::mouseButtons();
    if (bs.testFlag(Qt::LeftButton)) {
        sl_onItemSelected(idx);
    }
}

void EditMarkerGroupDialog::sl_onItemSelected(const QModelIndex &) {
    editButton->setEnabled(true);
    removeButton->setEnabled(true);
}

void EditMarkerGroupDialog::sl_onAddButtonClicked() {
    QObjectScopedPointer<EditMarkerDialog> dlg = new EditMarkerDialog(true, marker->getType(), "", QVariantList(), this);

    const int dialogResult = dlg->exec();
    CHECK(!dlg.isNull(), );

    if (QDialog::Accepted == dialogResult) {
        QString valueString;
        QString name = dlg->getName();

        MarkerUtils::valueToString(MarkerTypes::getDataTypeById(marker->getType()), dlg->getValues(), valueString);
        markerModel->addMarker(valueString, name);
    }
}

void EditMarkerGroupDialog::sl_onEditButtonClicked() {
    QItemSelectionModel *m = table->selectionModel();
    QModelIndexList selected = m->selectedRows();
    if (1 != selected.size()) {
        return;
    }

    QMap<QString, QString>::iterator i = marker->getValues().begin();
    i += selected.first().row();
    QVariantList values;
    MarkerUtils::stringToValue(MarkerTypes::getDataTypeById(marker->getType()),marker->getValues().key(*i), values);
    QObjectScopedPointer<EditMarkerDialog> dlg = new EditMarkerDialog(false, marker->getType(), *i, values, this);

    const int dialogResult = dlg->exec();
    CHECK(!dlg.isNull(), );

    if (QDialog::Accepted == dialogResult) {
        QString newValueString;
        QString newName = dlg->getName();

        MarkerUtils::valueToString(MarkerTypes::getDataTypeById(marker->getType()), dlg->getValues(), newValueString);
        markerModel->removeRows(selected.first().row(), 1, selected.first());
        markerModel->addMarker(newValueString, newName);
    }
}

void EditMarkerGroupDialog::sl_onRemoveButtonClicked() {
    QItemSelectionModel *m = table->selectionModel();
    QModelIndexList selected = m->selectedRows();
    if (1 != selected.size()) {
        return;
    }

    if (!markerModel->removeRows(selected.first().row(), 1, selected.first())) {
        QMessageBox::information(this, tr("Error"), tr("You can not remove the required marker \"rest\""));
    }
}

void EditMarkerGroupDialog::sl_onTypeChanged(int newTypeIndex) {
    if (newTypeIndex == currentTypeIndex) {
        return;
    }

    MarkerDataType oldType = MarkerTypes::getDataTypeById(marker->getType());
    MarkerDataType newType = MarkerTypes::getDataTypeById(typeIds.at(newTypeIndex));

    bool changeMarker = false;
    if (1 == marker->getValues().size()) { // contains only "rest"
        changeMarker = true;
    } else {
        if (oldType == newType) {
            changeMarker = true;
        } else {
            changeMarker = (QMessageBox::Ok == QMessageBox::question(this, tr("Warning"), tr("Are you really want to change marker's type? Some data can be lost!"), QMessageBox::Ok | QMessageBox::Cancel));
        }
    }

    if (changeMarker) {
        Marker *oldMarker = marker;
        marker = MarkerFactory::createInstanse(typeIds.at(newTypeIndex), addParamEdit->text());
        {
            marker->setName(oldMarker->getName());
            MarkerDataType oldType = MarkerTypes::getDataTypeById(oldMarker->getType());
            MarkerDataType newType = MarkerTypes::getDataTypeById(marker->getType());
            if (oldType == newType) {
                foreach (const QString &key, oldMarker->getValues().keys()) {
                    marker->addValue(key, oldMarker->getValues().value(key));
                }
            } else {
                marker->addValue(MarkerUtils::REST_OPERATION, oldMarker->getValues().value(MarkerUtils::REST_OPERATION));
            }
        }
        updateUi();
        currentTypeIndex = newTypeIndex;
        delete oldMarker;
    } else {
        typeBox->setCurrentIndex(currentTypeIndex);
    }
    markerGroupNameEdit->setText(allModel->suggestName(marker->getType()));
}

bool EditMarkerGroupDialog::checkEditMarkerResult(const QString &oldName, const QString &newName, const QString &newValue, QString &message) {
    QMap<QString, QString> values = marker->getValues();

    if (newName.contains(",")) {
        message.append(tr("Marker's name contains a comma symbol: \"%1\". It is not permitted for marker names").arg(newName));
        return false;
    }

    if (values.contains(newValue)) {
        if (values.value(newValue) != oldName) { // adding duplicating marker value
            message.append(tr("Duplicate marker's value: %1").arg(newValue));
            return false;
        }
    }

    if (newName != oldName) {
        if (values.values().contains(newName)) {
            message.append(tr("Duplicate marker's name: %1").arg(newName));
            return false;
        }
    }
    return true;
}

bool EditMarkerGroupDialog::checkAddMarkerResult(const QString &newName, const QString &newValue, QString &message) {
    QMap<QString, QString> values = marker->getValues();

    if (newName.contains(",")) {
        message.append(tr("Marker's name contains a comma symbol: \"%1\". It is not permitted for marker names").arg(newName));
        return false;
    }

    if (values.contains(newValue)) {
        message.append(tr("Duplicate marker's value: %1").arg(newValue));
        return false;
    }

    if (values.values().contains(newName)) {
        message.append(tr("Duplicate marker's name: %1").arg(newName));
        return false;
    }
    return true;
}

void EditMarkerGroupDialog::accept() {
    marker->setName(markerGroupNameEdit->text());
    { // check edit/add marker result
        MarkerEditorWidget *parent = dynamic_cast<MarkerEditorWidget*>(this->parent());
        QString message;

        ParameterState state = marker->hasAdditionalParameter();
        if (NONE != state) {
            marker->setAdditionalParameter(addParamEdit->text());
            if (REQUIRED == state && addParamEdit->text().isEmpty()) {
                QMessageBox::critical(this, tr("Error"), tr("Parameter '%1' is not set").arg(marker->getAdditionalParameterName()));
                return;
            }
        }

        if (isNew) {
            if (!parent->checkAddMarkerGroupResult(marker, message)) {
                QMessageBox::critical(this, tr("Error"), message);
                return;
            }
        } else {
            if (!parent->checkEditMarkerGroupResult(oldName, marker, message)) {
                QMessageBox::critical(this, tr("Error"), message);
                return;
            }
        }
    }
    QDialog::accept();
}

/************************************************************************/
/* MarkerListCfgModel */
/************************************************************************/
MarkerListCfgModel::MarkerListCfgModel(QObject *parent, Marker *marker)
: QAbstractTableModel(parent), marker(marker)
{

}

QVariant MarkerListCfgModel::data(const QModelIndex &index, int role) const {
    if (Qt::DisplayRole == role || Qt::ToolTipRole == role) {
        QString key = marker->getValues().keys()[index.row()];

        if (0 == index.column()) {
            return marker->getValues().value(key);
        } else if (1 == index.column()) {
            return key;
        }
    } else {
        return QVariant();
    }
    return QVariant();
}

int MarkerListCfgModel::columnCount(const QModelIndex &) const {
    return 2;
}

int MarkerListCfgModel::rowCount(const QModelIndex &) const {
    return marker->getValues().size();
}

Qt::ItemFlags MarkerListCfgModel::flags( const QModelIndex & index ) const {
    Q_UNUSED(index);
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant MarkerListCfgModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
        case 0:
            return tr("Marker name");
        case 1:
            return tr("Marker value");
        default:
            assert(false);
        }
    }
    // unreachable code
    return QVariant();
}

bool MarkerListCfgModel::setData(const QModelIndex & index, const QVariant & value, int role) {
    Q_UNUSED(index); Q_UNUSED(value); Q_UNUSED(role);
    return true;
}

bool MarkerListCfgModel::removeRows(int row, int count, const QModelIndex &parent) {
    Q_UNUSED(parent);
    if (1 != count) {
        return true;
    }

    QMap<QString, QString>::iterator i = marker->getValues().begin();
    i += row;
    if (MarkerUtils::REST_OPERATION == marker->getValues().key(*i)) {
        return false;
    }
    beginRemoveRows(QModelIndex(), row, row+count-1);
    marker->getValues().erase(i);
    endRemoveRows();

    return true;
}

void MarkerListCfgModel::addMarker(const QString &valueString, const QString &name) {
    QMap<QString, QString> allValues = marker->getValues();
    allValues[valueString] = name;
    int newRow = allValues.keys().indexOf(valueString);

    beginInsertRows(QModelIndex(), newRow, newRow);
    marker->getValues().insert(valueString, name);
    endInsertRows();
}

/************************************************************************/
/* EditMarkerDialog */
/************************************************************************/
EditMarkerDialog::EditMarkerDialog(bool isNew, const QString &type, const QString &name, const QVariantList &values, QWidget *parent)
: QDialog(parent), isNew(isNew), type(type), name(name), values(values), editWidget(NULL)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "28967144");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    if (!isNew) {
        markerNameEdit->setText(name);

        if (values.at(0).toString() == MarkerUtils::REST_OPERATION) {
            return;
        }
    } else {
        this->setWindowTitle(tr("Create Marker"));
    }

    if (!type.isEmpty()) {
        MarkerDataType dataType = MarkerTypes::getDataTypeById(type);
        switch (dataType) {
            case INTEGER:
                editWidget = new EditIntegerMarkerWidget(isNew, values, this);
                break;
            case FLOAT:
                editWidget = new EditFloatMarkerWidget(isNew, values, this);
                break;
            case STRING:
                editWidget = new EditStringMarkerWidget(isNew, values, this);
                break;
            default:
                assert(0);
        }
        QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(this->layout());
        layout->insertWidget(1, editWidget);
    }
}

void EditMarkerDialog::accept() {
    { // check edit/add marker result
        EditMarkerGroupDialog *parent = dynamic_cast<EditMarkerGroupDialog*>(this->parent());
        QString message;
        QString valueString;
        QVariantList newVals;
        if (NULL == editWidget) {
            newVals << MarkerUtils::REST_OPERATION;
        } else {
            newVals = editWidget->getValues();
        }
        MarkerUtils::valueToString(MarkerTypes::getDataTypeById(type), newVals, valueString);

        if (isNew) {
            if (!parent->checkAddMarkerResult(markerNameEdit->text(), valueString, message)) {
                QMessageBox::critical(this, tr("Error"), message);
                return;
            }
        } else {
            if (!parent->checkEditMarkerResult(name, markerNameEdit->text(), valueString, message)) {
                QMessageBox::critical(this, tr("Error"), message);
                return;
            }
        }
    }

    if (NULL != editWidget) {
        values = editWidget->getValues();
    }
    name = markerNameEdit->text().trimmed();
    QDialog::accept();
}

/************************************************************************/
/* EditTypedMarkerWidget */
/************************************************************************/
EditIntegerMarkerWidget::EditIntegerMarkerWidget(bool isNew, const QVariantList &values, QWidget *parent)
: EditTypedMarkerWidget(values, parent)
{
    setupUi(this);
    lessButton->toggle();
    greaterButton->toggle();
    intervalButton->toggle();

    if (!isNew) {
        assert(values.size() > 1);
        if (MarkerUtils::INTERVAL_OPERATION == values.at(0)) {
            assert(3 == values.size());
            intervalButton->toggle();
            bool ok = false;
            intMinBox->setValue(values.at(1).toInt(&ok));
            assert(ok);
            intMaxBox->setValue(values.at(2).toInt(&ok));
            assert(ok);
        } else if (MarkerUtils::LESS_OPERATION == values.at(0)) {
            assert(2 == values.size());
            lessButton->toggle();
            bool ok = false;
            lessBox->setValue(values.at(1).toInt(&ok));
            assert(ok);
        } else if (MarkerUtils::GREATER_OPERATION == values.at(0)) {
            assert(2 == values.size());
            greaterButton->toggle();
            bool ok = false;
            greaterBox->setValue(values.at(1).toInt(&ok));
            assert(ok);
        } else {
            assert(0);
        }
    }
}

QVariantList EditIntegerMarkerWidget::getValues() {
    QVariantList values;
    if (lessButton->isChecked()) {
        values << MarkerUtils::LESS_OPERATION;
        values << lessBox->value();
    } else if (greaterButton->isChecked()) {
        values << MarkerUtils::GREATER_OPERATION;
        values << greaterBox->value();
    } else if (intervalButton->isChecked()) {
        values << MarkerUtils::INTERVAL_OPERATION;
        values << intMinBox->value();
        values << intMaxBox->value();
    }
    return values;
}

EditFloatMarkerWidget::EditFloatMarkerWidget(bool isNew, const QVariantList &values, QWidget *parent)
: EditTypedMarkerWidget(values, parent)
{
    setupUi(this);
    lessButton->toggle();
    greaterButton->toggle();
    intervalButton->toggle();

    if (!isNew) {
        assert(values.size() > 1);
        if (MarkerUtils::INTERVAL_OPERATION == values.at(0)) {
            assert(3 == values.size());
            intervalButton->toggle();
            bool ok = false;
            intMinBox->setValue(values.at(1).toFloat(&ok));
            assert(ok);
            intMaxBox->setValue(values.at(2).toFloat(&ok));
            assert(ok);
        } else if (MarkerUtils::LESS_OPERATION == values.at(0)) {
            assert(2 == values.size());
            lessButton->toggle();
            bool ok = false;
            lessBox->setValue(values.at(1).toFloat(&ok));
            assert(ok);
        } else if (MarkerUtils::GREATER_OPERATION == values.at(0)) {
            assert(2 == values.size());
            greaterButton->toggle();
            bool ok = false;
            greaterBox->setValue(values.at(1).toFloat(&ok));
            assert(ok);
        } else {
            assert(0);
        }
    }
}

QVariantList EditFloatMarkerWidget::getValues() {
    QVariantList values;
    if (lessButton->isChecked()) {
        values << MarkerUtils::LESS_OPERATION;
        values << lessBox->value();
    } else if (greaterButton->isChecked()) {
        values << MarkerUtils::GREATER_OPERATION;
        values << greaterBox->value();
    } else if (intervalButton->isChecked()) {
        values << MarkerUtils::INTERVAL_OPERATION;
        values << intMinBox->value();
        values << intMaxBox->value();
    }
    return values;
}

EditStringMarkerWidget::EditStringMarkerWidget(bool isNew, const QVariantList &values, QWidget *parent)
: EditTypedMarkerWidget(values, parent)
{
    setupUi(this);
    endsButton->toggle();
    containsButton->toggle();
    regexpButton->toggle();
    startsButton->toggle();

    if (!isNew) {
        assert(2 == values.size());
        if (MarkerUtils::STARTS_OPERATION == values.at(0)) {
            startsButton->toggle();
            startsEdit->setText(values.at(1).toString());
        } else if (MarkerUtils::ENDS_OPERATION == values.at(0)) {
            endsButton->toggle();
            endsEdit->setText(values.at(1).toString());
        } else if (MarkerUtils::CONTAINS_OPERATION == values.at(0)) {
            containsButton->toggle();
            containsEdit->setText(values.at(1).toString());
        } else if (MarkerUtils::REGEXP_OPERATION == values.at(0)) {
            regexpButton->toggle();
            regexpEdit->setText(values.at(1).toString());
        } else {
            assert(0);
        }
    }
}

QVariantList EditStringMarkerWidget::getValues() {
    QVariantList values;
    if (startsButton->isChecked()) {
        values << MarkerUtils::STARTS_OPERATION;
        values << startsEdit->text();
    } else if (endsButton->isChecked()) {
        values << MarkerUtils::ENDS_OPERATION;
        values << endsEdit->text();
    } else if (containsButton->isChecked()) {
        values << MarkerUtils::CONTAINS_OPERATION;
        values << containsEdit->text();
    } else if (regexpButton->isChecked()) {
        values << MarkerUtils::REGEXP_OPERATION;
        values << regexpEdit->text();
    }
    return values;
}

} // U2
