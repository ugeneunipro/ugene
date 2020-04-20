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

#ifndef _U2_WEIGHT_MATRIX_JASPAR_DIALOG_CONTROLLER_H_
#define _U2_WEIGHT_MATRIX_JASPAR_DIALOG_CONTROLLER_H_

#include <ui_SearchJASPARDatabase.h>

#include <QDialog>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include <U2Core/PFMatrix.h>

#include "WeightMatrixPlugin.h"

namespace U2 {

class PWMJASPARDialogController : public QDialog, public Ui_SearchJASPARDatabase {
    Q_OBJECT

public:
    PWMJASPARDialogController(QWidget* w = NULL);
    QString fileName;

private slots:

    void sl_onOK();
    void sl_onCancel();
    void sl_onSelectionChanged();
    void sl_onDoubleClicked(QTreeWidgetItem* item, int col);
    void sl_onTableItemClicked(QTableWidgetItem* item);

};

class JasparTreeItem;
class JasparGroupTreeItem : public QTreeWidgetItem {
public:
    JasparGroupTreeItem(const QString& s);
    QString s;
    bool operator<(const QTreeWidgetItem & other) const;
};

class JasparTreeItem : public QTreeWidgetItem {
public:
    JasparTreeItem(const JasparInfo& ed);
    JasparInfo matrix;
    bool operator<(const QTreeWidgetItem & other) const;
};

} //namespace

#endif
