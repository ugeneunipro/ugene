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

#ifndef _U2_DASHBOARDS_MANAGER_DIALOG_H_
#define _U2_DASHBOARDS_MANAGER_DIALOG_H_

#include <U2Designer/Dashboard.h>
#include <U2Designer/DashboardInfo.h>

#include <ui_DashboardsManagerDialog.h>

namespace U2 {

class DashboardsManagerDialog : public QDialog, public Ui_DashboardsManagerDialog {
    Q_OBJECT
public:
    DashboardsManagerDialog(QWidget *parent);

    QMap<QString, bool> getDashboardsVisibility() const;
    const QStringList &removedDashboards() const;

private slots:
    void sl_check();
    void sl_uncheck();
    void sl_selectAll();
    void sl_remove();

private:
    void setupList();
    QList<QTreeWidgetItem*> allItems() const;
    bool confirmDashboardsRemoving( ) const;

private:
    QStringList toRemove;
};

} // U2

#endif // _U2_DASHBOARDS_MANAGER_DIALOG_H_
