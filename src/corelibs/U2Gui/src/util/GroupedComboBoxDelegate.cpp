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

#include "GroupedComboBoxDelegate.h"

#include <QStandardItem>

namespace U2 {

GroupedComboBoxDelegate::GroupedComboBoxDelegate(QObject *parent) : QItemDelegate(parent){}

void GroupedComboBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if(index.data(Qt::AccessibleDescriptionRole).toString() == QLatin1String("separator")) {
        painter->setPen(Qt::gray);
        painter->drawLine(option.rect.left(), option.rect.center().y(), option.rect.right(), option.rect.center().y());
    } else if(index.data(Qt::AccessibleDescriptionRole).toString() == QLatin1String("parent")) {
        QStyleOptionViewItem parentOption = option;
        parentOption.state |= QStyle::State_Enabled;
        QItemDelegate::paint( painter, parentOption, index );
    } else if ( index.data(Qt::AccessibleDescriptionRole).toString() == QLatin1String( "child" ) ) {
        QStyleOptionViewItem childOption = option;
        int indent = option.fontMetrics.width( QString( 4, QChar( ' ' ) ) );
        childOption.rect.adjust( indent, 0, 0, 0 );
        childOption.textElideMode = Qt::ElideNone;
        QItemDelegate::paint( painter, childOption, index );
    } else {
        QItemDelegate::paint(painter, option, index);
    }
}

QSize GroupedComboBoxDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QString type = index.data(Qt::AccessibleDescriptionRole).toString();
    if (type == QLatin1String("separator")) {
        return QSize(0, 10);
    }
    return QItemDelegate::sizeHint( option, index );
}

void GroupedComboBoxDelegate::addParentItem(QStandardItemModel * model, const QString& text, bool setItalic, bool setBold) {
    QStandardItem* item = new QStandardItem(text);
    item->setData("parent", Qt::AccessibleDescriptionRole);
    item->setFlags(item->flags() & ~(Qt::ItemIsEnabled | Qt::ItemIsSelectable));
    QFont font = item->font();
    font.setItalic(setItalic);
    font.setBold(setBold);
    item->setFont(font);
    model->appendRow(item);
}

void GroupedComboBoxDelegate::addChildItem(QStandardItemModel * model, const QString& text, const QVariant& data) {
    QStandardItem* item = new QStandardItem(text + QString(4, QChar(' ')));
    item->setData(data, Qt::UserRole);
    item->setData("child", Qt::AccessibleDescriptionRole);
    model->appendRow(item);
}

void GroupedComboBoxDelegate::addUngroupedItem(QStandardItemModel* model, const QString& text, const QVariant& data) {
    QStandardItem* item = new QStandardItem(text);
    item->setData(data, Qt::UserRole);
    item->setData("ungrouped", Qt::AccessibleDescriptionRole);
    model->appendRow(item);
}

}
