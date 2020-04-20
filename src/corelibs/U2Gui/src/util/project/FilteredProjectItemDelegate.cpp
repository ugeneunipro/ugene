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

#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QPainter>
#include <QTextDocument>

#include "FilteredProjectItemDelegate.h"

namespace U2 {

FilteredProjectItemDelegate::FilteredProjectItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

void FilteredProjectItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyleOptionViewItem localOption = option;
    initStyleOption(&localOption, index);

    QStyle *style = localOption.widget ? localOption.widget->style() : QApplication::style();

    QTextDocument doc;
    doc.setHtml(localOption.text);

    painter->save();

    // Painting item without text
    localOption.text = QString();
    style->drawControl(QStyle::CE_ItemViewItem, &localOption, painter);

    QAbstractTextDocumentLayout::PaintContext ctx;

    // Highlighting text if item is selected
    if (0 != (localOption.state & QStyle::State_Selected)) {
        ctx.palette.setColor(QPalette::Text, localOption.palette.color(QPalette::Active, QPalette::HighlightedText));
    } else {
        ctx.palette.setColor(QPalette::Text, localOption.palette.color(QPalette::Active, QPalette::Text));
    }

    if (0 == (localOption.state & QStyle::State_Active)) {
        ctx.palette.setColor(QPalette::Text, localOption.palette.color(QPalette::Active, QPalette::Text));
    }

    const QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &localOption);
    painter->translate(textRect.topLeft());
    painter->setClipRect(textRect.translated(-textRect.topLeft()));
    doc.documentLayout()->draw(painter, ctx);

    painter->restore();
}

QSize FilteredProjectItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyleOptionViewItem localOption = option;
    initStyleOption(&localOption, index);

    QTextDocument doc;
    doc.setHtml(localOption.text);
    doc.setDocumentMargin(index.parent().isValid() ? 1 : 2);
    doc.setDefaultFont(localOption.font);
    return QSize(doc.idealWidth(), doc.size().height());
}

}
