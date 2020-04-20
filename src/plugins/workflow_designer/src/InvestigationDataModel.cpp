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

#include <U2Lang/WorkflowTransport.h>

#include "InvestigationDataModel.h"

const int DEFAULT_COUNT = -1;

namespace U2 {

InvestigationDataModel::InvestigationDataModel(const Workflow::Link *bus, QObject *parent)
    : QAbstractTableModel(parent), investigatedLink(bus), cachedData(), countOfRows(DEFAULT_COUNT),
    hiddenColumns()
{

}

InvestigationDataModel::~InvestigationDataModel() {

}

int InvestigationDataModel::loadedRowCount() const {
    return (cachedData.isEmpty()) ? 0 : cachedData[cachedData.keys().first()].size();
}

bool InvestigationDataModel::isAnyColumnHidden() const {
    return (!hiddenColumns.isEmpty() && 0 != hiddenColumns.count(true));
}

void InvestigationDataModel::showAllHiddenColumns() {
    int insertedColumnCounter = 0;
    while(hiddenColumns.testBit(insertedColumnCounter)
        && insertedColumnCounter < hiddenColumns.size())
    {
        insertColumn(insertedColumnCounter);
        ++insertedColumnCounter;
    }
    for(int columnCounter = insertedColumnCounter; columnCounter < hiddenColumns.size(); ) {
        if(!hiddenColumns.testBit(columnCounter)) {
            insertedColumnCounter = columnCounter + 1;
            while(hiddenColumns.size() > insertedColumnCounter
                && hiddenColumns.testBit(insertedColumnCounter))
            {
                insertColumn(insertedColumnCounter);
                ++insertedColumnCounter;
            }
            columnCounter = insertedColumnCounter;
        }
    }
}

void InvestigationDataModel::setColumnsVisibility(const QBitArray &columns) {
    hiddenColumns = columns;
}

QBitArray InvestigationDataModel::getColumnsVisibility() const {
    return hiddenColumns;
}

int InvestigationDataModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    if(DEFAULT_COUNT == countOfRows) {
        emit si_countOfMessagesRequested(investigatedLink);
    }
    return countOfRows;
}

int InvestigationDataModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    if(hiddenColumns.isNull()) {
        emit si_investigationRequested(investigatedLink);
        if(!cachedData.isEmpty()) {
            emit si_columnsVisibilityRequested();
        }
    }
    return (hiddenColumns.size() - hiddenColumns.count(true));
}

QVariant InvestigationDataModel::data(const QModelIndex &index, int role) const {
    QVariant result;
    const QList<QString> headerLabelList = cachedData.keys();
    const int requestedRow = index.row();
    const int absoluteColumnNumber = getAbsoluteNumberOfVisibleColumn(index.column());
    if ( Qt::DisplayRole == role && requestedRow < countOfRows
        && index.column() < hiddenColumns.count(false)
        && headerLabelList.size() > absoluteColumnNumber)
    {
        const QString key = headerLabelList[absoluteColumnNumber];
        if(cachedData[key].size() <= requestedRow) {
            emit si_investigationRequested(investigatedLink, requestedRow);
        }
        if(cachedData[key].size() > requestedRow) {
            result.setValue<QString>(cachedData[key][requestedRow]);
        }
    }
    return result;
}

QVariant InvestigationDataModel::headerData(int section, Qt::Orientation orientation, int role)
    const
{
    QVariant result;
    if(Qt::DisplayRole == role) {
        switch(orientation) {
        case Qt::Horizontal:
            if(section < hiddenColumns.count(false) && !cachedData.isEmpty()) {
                const int absNumberOfVisibleColumn = getAbsoluteNumberOfVisibleColumn(section);
                const QList<QString> headerLabelList = cachedData.keys();
                if (headerLabelList.size() > absNumberOfVisibleColumn) {
                    result.setValue<QString>(cachedData.keys()[absNumberOfVisibleColumn]);
                } else {
                    emit si_investigationRequested(investigatedLink, 0);
                }
            }
            break;
        case Qt::Vertical:
            if(section < countOfRows) {
                result.setValue<QString>(QString::number(section + 1));
            }
            break;
        default:
            Q_ASSERT(false);
        }
    }
    return result;
}

bool InvestigationDataModel::setHeaderData(int section, Qt::Orientation orientation,
    const QVariant & value, int role)
{
    if(Qt::DisplayRole == role && Qt::Horizontal == orientation && value.isValid()) {
        cachedData[value.toString()] = QQueue<QString>();
        const int columnCount = hiddenColumns.size();
        if(section >= columnCount) {
            hiddenColumns.resize(columnCount + 1);
        }
        return true;
    }
    return false;
}

Qt::ItemFlags InvestigationDataModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }
    return QAbstractTableModel::flags(index) | Qt::ItemFlags(~Qt::ItemIsEditable);
}

bool InvestigationDataModel::insertRows(int position, int rows, const QModelIndex &index) {
    Q_UNUSED(position);
    Q_UNUSED(index);
    countOfRows = (DEFAULT_COUNT == countOfRows) ? rows : countOfRows + rows;
    return true;
}

bool InvestigationDataModel::removeRows(int position, int rows, const QModelIndex &index) {
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position + rows - 1);
    if(!cachedData.isEmpty()) {
        foreach(QString columnName, cachedData.keys()) {
            for(int row = 0; row < rows; ++row) {
                cachedData[columnName].removeAt(position);
            }
        }
    }
    countOfRows -= rows;
    endRemoveRows();
    return true;
}

bool InvestigationDataModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.isValid() && hiddenColumns.size() > index.column() && countOfRows > index.row()
        && value.isValid() && role == Qt::DisplayRole)
    {
        const QString data = value.toString();
        cachedData[cachedData.keys()[index.column()]].enqueue(data);
        if(!hiddenColumns.testBit(index.column())) {
            QModelIndex changedIndex(this->index(index.row(), getVisibleNumberOfAbsoluteColumn(index.column())));
            emit dataChanged(changedIndex, changedIndex);
        }
        return true;
    }
    return false;
}

bool InvestigationDataModel::removeColumns(int column, int count, const QModelIndex &parent) {
    Q_UNUSED(parent);
    if(hiddenColumns.count(false) > (column + count - 1) && 0 <= column && 0 < count) {
        beginRemoveColumns(QModelIndex(), column, column + count - 1);
        const int absoluteFirstColumnNumber = getAbsoluteNumberOfVisibleColumn(column);
        for(int columnCounter = absoluteFirstColumnNumber;
            columnCounter < absoluteFirstColumnNumber + count; ++columnCounter)
        {
            hiddenColumns.setBit(columnCounter, true);
        }
        endRemoveColumns();
        return true;
    }
    return false;
}

bool InvestigationDataModel::insertColumns(int column, int count, const QModelIndex &parent) {
    Q_UNUSED(parent);
    if(hiddenColumns.count(true) >= count && 0 <= column && 0 < count) {
        beginInsertColumns(QModelIndex(), column, column + count - 1);
        for(int columnCounter = column; columnCounter < column + count; ++columnCounter) {
            hiddenColumns.setBit(columnCounter, false);
        }
        endInsertColumns();
        return true;
    }
    return false;
}

int InvestigationDataModel::getAbsoluteNumberOfVisibleColumn(int column) const {
    int result = 0;
    if(!hiddenColumns.isEmpty()) {
        Q_ASSERT(hiddenColumns.count(false) > column);
        for(int columnCounter = -1; columnCounter < column; ++result) {
            if(!hiddenColumns.testBit(result)) {
                ++columnCounter;
            }
        }
        --result;
    }
    return result;
}

int InvestigationDataModel::getVisibleNumberOfAbsoluteColumn(int column) const {
    int result = 0;
    if(!hiddenColumns.isEmpty()) {
        Q_ASSERT(hiddenColumns.count() > column);
        for(int columnCounter = 0; columnCounter < column; ++columnCounter) {
            if(!hiddenColumns.testBit(columnCounter)) {
                ++result;
            }
        }
    }
    return result;
}

} // namespace U2
