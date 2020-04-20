/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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

#ifndef _HI_GT_TABWIDGET_H_
#define _HI_GT_TABWIDGET_H_

#include "GTGlobals.h"
#include <QTabWidget>

namespace HI {

class HI_EXPORT GTTabWidget {
public:
    // fails if the tabwidget is NULL, index is not in a tabwidget's range
    // or a tabwidget's index differs from a given index in the end of method's execution
    static void setCurrentIndex(GUITestOpStatus& os, QTabWidget * const tabWidget, int index);
    static QTabBar* getTabBar(GUITestOpStatus &os, QTabWidget* tabWidget);

    static void clickTab(GUITestOpStatus &os, QTabWidget * const tabWidget, int tabIndex, Qt::MouseButton button = Qt::LeftButton);
    static void clickTab(GUITestOpStatus &os, const QString &tabWidgetName, QWidget const * const parent, int tabIndex, Qt::MouseButton button = Qt::LeftButton);
    static void clickTab(GUITestOpStatus &os, QTabWidget * const tabWidget, const QString &tabName, Qt::MouseButton button = Qt::LeftButton);
    static void clickTab(GUITestOpStatus &os, const QString &tabWidgetName, const QWidget * const parent, const QString &tabName, Qt::MouseButton button = Qt::LeftButton);

    static QString getTabName(GUITestOpStatus &os, QTabWidget* tabWidget, int idx);

    static int getTabNumByName(GUITestOpStatus &os, QTabWidget* tabWidget, QString tabName);
    static QWidget* getTabCornerWidget(GUITestOpStatus &os, QTabWidget* tabWidget, int idx);
    static QWidget* getTabCornerWidget(GUITestOpStatus &os, QTabWidget* tabWidget, QString tabName);

    static void closeTab(GUITestOpStatus &os, QTabWidget *tabWidget, int idx);
    static void closeTab(GUITestOpStatus &os, QTabWidget *tabWidget, QString tabName);


};

}
#endif // _HI_GT_TABWIDGET_H_
