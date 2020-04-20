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

#ifndef _U2_DATASETS_LIST_WIDGET_H_
#define _U2_DATASETS_LIST_WIDGET_H_

#include <QTabWidget>

#include "DatasetWidget.h"

namespace U2 {

class DatasetsController;

class DatasetsTabWidget : public QTabWidget {
    Q_OBJECT
public:
    DatasetsTabWidget(QWidget *parent);

signals:
    void si_contextMenu(const QPoint &p, int idx);

private slots:
    void sl_contextMenu(const QPoint &p);
};

class DatasetsListWidget : public QWidget {
    Q_OBJECT
public:
    DatasetsListWidget(DatasetsController *ctrl);

    void appendPage(const QString &name, QWidget *page);

private slots:
    void sl_deleteDataset(int idx);
    void sl_renameDataset();
    void sl_newDataset();
    void sl_contextMenu(const QPoint &p, int idx);

private:
    DatasetsController *ctrl;
    QTabWidget *tabs;

private:
    QString askDatasetName(const QString &tip, const QString &title, bool &ok);
    QString getTip() const;
};

} // U2

#endif // _U2_DATASETS_LIST_WIDGET_H_
