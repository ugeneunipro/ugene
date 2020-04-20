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

#ifndef U2_SELECTMODELSDIALOG_H
#define U2_SELECTMODELSDIALOG_H

#include "ui_SelectModelsDialog.h"
#include <QDialog>

namespace U2 {

class SelectModelsDialog : public QDialog, public Ui_SelectModelsDialog
{
    Q_OBJECT

public:
    SelectModelsDialog(const QList<int> &modelIds, const QList<int> &selectedItems, QWidget *parent = 0);
    ~SelectModelsDialog();

    const QList<int> &getSelectedModelsIndexes() const { return selectedModelsIndexes; }

public slots:
    virtual void accept();

    void sl_onItemDoubleClicked(QListWidgetItem *item);

    void sl_onSlectAll();
    void sl_onInvertSelection();

private:
    QList<int> selectedModelsIndexes;
};

}   // namespace U2

#endif  // #ifndef U2_SELECTMODELSDIALOG_H
