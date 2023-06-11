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
#include <QTabWidget>

#include "GTGlobals.h"

namespace HI {

class HI_EXPORT GTTabWidget {
public:
    // fails if the tabwidget is NULL, index is not in a tabwidget's range
    // or a tabwidget's index differs from a given index in the end of method's execution
    static void setCurrentIndex(QTabWidget* const tabWidget, int index);
    static QTabBar* getTabBar(QTabWidget* tabWidget);

    static void clickTab(QTabWidget* const tabWidget, int tabIndex, Qt::MouseButton button = Qt::LeftButton);
    static void clickTab(const QString& tabWidgetName, QWidget* parent, int tabIndex, Qt::MouseButton button = Qt::LeftButton);
    static void clickTab(QTabWidget* const tabWidget, const QString& tabName, Qt::MouseButton button = Qt::LeftButton);
    static void clickTab(const QString& tabWidgetName, QWidget* parent, const QString& tabName, Qt::MouseButton button = Qt::LeftButton);

    static QString getTabName(QTabWidget* tabWidget, int idx);
    // QTabWidget contais several tabs, each tab is some QWidget
    // Also, each tab has a name, which is written on this tab
    // The method returns a name of the corresponding tab
    // @tabWidget the pointer to QTabWidget
    // @widget the pointer to the QWidget, wich is one of @tabWidget tabs
    // @return the name of @widget, which is written on the tab in @tabWidget or empty string if fail to find
    static QString getTabNameByWidget(QTabWidget* tabWidget, QWidget* widget);

    static int getTabNumByName(QTabWidget* tabWidget, QString tabName);
    static QWidget* getTabCornerWidget(QTabWidget* tabWidget, int idx);
    static QWidget* getTabCornerWidget(QTabWidget* tabWidget, QString tabName);

    static void closeTab(QTabWidget* tabWidget, int idx);
    static void closeTab(QTabWidget* tabWidget, QString tabName);
};

}  // namespace HI
