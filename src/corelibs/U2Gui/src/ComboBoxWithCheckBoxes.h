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

#include <QComboBox>
#include <QStandardItem>

#include <U2Core/global.h>

namespace U2 {

//https://habr.com/ru/post/215289/
class U2GUI_EXPORT ComboBoxWithCheckBoxes : public QComboBox {
    Q_OBJECT

public:
    ComboBoxWithCheckBoxes(QWidget *parent = nullptr);

    const QStringList& getCheckedItems() const;
    void setCheckedItems(const QStringList &items);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void sl_modelRowsInserted(const QModelIndex &parent, int start, int end);
    void sl_modelRowsRemoved(const QModelIndex &parent, int start, int end);
    void sl_modelItemChanged(QStandardItem *item);

signals:
    void si_checkedChanged(QStringList checked);

private:
    void updateOnCheckedItemsChange();
    void updateDisplayText();

    QStringList checkedItems;
    QString displayText;
    const QRect displayRectDelta;
};

}
