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

#ifndef _U2_INVESTIGATION_DATA_MODEL_H_
#define _U2_INVESTIGATION_DATA_MODEL_H_

#include <QAbstractTableModel>
#include <QBitArray>

#include <U2Lang/Port.h>
#include <U2Lang/WorkflowInvestigationData.h>

namespace U2 {

using namespace Workflow;

// the class is intended to enhance the performance
// of the workflow debugging investigations tools, i.e.
// the table containing the content of workflow workers' messages

class InvestigationDataModel : public QAbstractTableModel {
    Q_OBJECT
public:
    InvestigationDataModel(const Workflow::Link *bus, QObject *parent = NULL);
    ~InvestigationDataModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole)
        const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole);
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant & value,
        int role = Qt::DisplayRole);
    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex());

    // the returning value is the count of rows which data is cached
    int loadedRowCount() const;
    bool isAnyColumnHidden() const;
    void showAllHiddenColumns();
    int getAbsoluteNumberOfVisibleColumn(int column) const;
    void setColumnsVisibility(const QBitArray &columns);
    QBitArray getColumnsVisibility() const;

signals:
    void si_investigationRequested(const Workflow::Link *bus, int messageNumber = 0) const;
    void si_countOfMessagesRequested(const Workflow::Link *bus) const;
    void si_columnsVisibilityRequested() const;

private:
    int getVisibleNumberOfAbsoluteColumn(int column) const;

    const Workflow::Link *investigatedLink;
    WorkflowInvestigationData cachedData;
    int countOfRows;
    QBitArray hiddenColumns;
};

} // namespace U2

#endif // _U2_INVESTIGATION_DATA_MODEL_H_
