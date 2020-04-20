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

#ifndef _U2_GROUPED_COMBOBOX_DELEGATE_H_
#define _U2_GROUPED_COMBOBOX_DELEGATE_H_

#include <QItemDelegate>
#include <QPainter>

#include <U2Core/global.h>

class QStandardItemModel;

namespace U2 {

class U2GUI_EXPORT GroupedComboBoxDelegate : public QItemDelegate {
    Q_OBJECT
public:
    explicit GroupedComboBoxDelegate(QObject *parent = 0);

    static void addParentItem(QStandardItemModel * model, const QString& text, bool setItalic = true, bool setBold = true);
    static void addChildItem(QStandardItemModel * model, const QString& text, const QVariant& data);
    static void addUngroupedItem(QStandardItemModel* model, const QString& text, const QVariant& data);
protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

}

#endif // _U2_GROUPED_COMBOBOX_DELEGATE_H_
