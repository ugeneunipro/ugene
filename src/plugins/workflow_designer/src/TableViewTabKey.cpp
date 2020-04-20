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

#include <QKeyEvent>

#include "TableViewTabKey.h"

namespace U2 {
TableViewTabKey::TableViewTabKey(QWidget *parent) :
    QTableView(parent){
}

QModelIndex TableViewTabKey::getNextIndex() const{
    int column = this->currentIndex().column();
    int row = this->currentIndex().row();
    if (column != 0){
        row++;
    }
    column = 1;
    return this->model()->index(row, column);
}

void TableViewTabKey::setNextIndex(){
    QModelIndex nextIndex = getNextIndex();
    this->setCurrentIndex(nextIndex);
    this->edit(nextIndex);
}

void TableViewTabKey::closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint){
    if (hint == QAbstractItemDelegate::NoHint){
        QTableView::closeEditor(editor, QAbstractItemDelegate::SubmitModelCache);
        return;
    }
    if (hint == QAbstractItemDelegate::EditNextItem){
        setNextIndex();
        return ;
    }
    QTableView::closeEditor(editor, hint);
}

void TableViewTabKey::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Tab){
        setNextIndex();
        return ;
    }
    QTableView::keyPressEvent(event);
}
}
