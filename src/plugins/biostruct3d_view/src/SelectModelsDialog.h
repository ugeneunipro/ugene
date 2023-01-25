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

#ifndef U2_SELECTMODELSDIALOG_H
#define U2_SELECTMODELSDIALOG_H

#include <QDialog>

#include "ui_SelectModelsDialog.h"

namespace U2 {

class SelectModelsDialog : public QDialog, public Ui_SelectModelsDialog {
    Q_OBJECT

public:
    SelectModelsDialog(const QList<int>& modelIds, const QList<int>& initiallySelectedModelIds, QWidget* parent);

    /** Returns list of selected models. Available after the dialog is accepted. */
    const QList<int>& getSelectedModelsIds() const;

public slots:
    void accept() override;

    void sl_onItemDoubleClicked(QListWidgetItem* item);

    void sl_onSelectAll();
    void sl_onInvertSelection();

private:
    QList<int> modelIds;
    QList<int> selectedModelIds;
};

}  // namespace U2

#endif  // #ifndef U2_SELECTMODELSDIALOG_H
