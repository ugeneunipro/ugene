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
#include <QListWidget>

#include "GTGlobals.h"

namespace HI {

class HI_EXPORT GTListWidget {
public:
    static void click(QListWidget* listWidget, const QString& text, Qt::MouseButton button = Qt::LeftButton, int foundItemsNum = 0);
    static bool isItemChecked(QListWidget* listWidget, const QString& text);
    static void checkItem(QListWidget* listWidget, const QString& text, bool newState);
    static void checkAllItems(QListWidget* listWidget, bool newState);
    static QStringList getItems(QListWidget* listWidget);

    /** Selects QListWidget items by text. */
    static void selectItemsByText(QListWidget* listWidget, const QStringList& itemTexts);

    /** Selects QListWidget items with Control + mouse-click. */
    static void selectItems(const QList<QListWidgetItem*>& items);

    /** Scrolls the given item into view. */
    static void scrollToItem(QListWidgetItem* item);

    /** Finds QListWidget item by text. Asserts that there is only 1 item with the given text. */
    static QListWidgetItem* findItemByText(QListWidget* listWidget, const QString& text);

    /** Returns list of items matched by text. Allow multiple items to have the same text. */
    static QList<QListWidgetItem*> findItemsByText(QListWidget* listWidget, const QStringList& itemTexts);

    /** Returns center point of the item in global coordinates. The item must be scrolled in into visible area to have a valid center point. */
    static QPoint getItemCenter(QListWidgetItem* item);

    /** Returns bounding rect of the item in global coordinates. The item must be scrolled in into visible area to have a valid bounding rect. */
    static QRect getItemRect(QListWidgetItem* item);

    /** Check that the current selection is equal to the texts list. */
    static void checkSelection(QListWidget* listWidget, const QStringList& itemTexts);
};

}  // namespace HI
