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

#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTWidget.h>

#include "primitives/GTTabBar.h"
#include "primitives/GTTabWidget.h"

namespace HI {

#define GT_CLASS_NAME "GTTabWidget"

#define GT_METHOD_NAME "setCurrentIndex"
void GTTabWidget::setCurrentIndex(QTabWidget* const tabWidget, int index) {
    GT_CHECK(tabWidget != NULL, "QTabWidget* == NULL");

    int tabsCount = tabWidget->count();
    GT_CHECK(index >= 0 && index < tabsCount, "invalid index");

    QTabBar* tabBar = tabWidget->findChild<QTabBar*>();
    GTTabBar::setCurrentIndex(tabBar, index);

    int currIndex = tabWidget->currentIndex();
    GT_CHECK(currIndex == index, "Can't set index");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTabBar"
QTabBar* GTTabWidget::getTabBar(QTabWidget* tabWidget) {
    GT_CHECK_RESULT(tabWidget != NULL, "tabWidget is NULL", NULL);
    QList<QTabBar*> tabBars = tabWidget->findChildren<QTabBar*>();
    int numToCheck = tabBars.size();
    GT_CHECK_RESULT(numToCheck < 2, QString("too many tab bars found: %1").arg(numToCheck), NULL);
    GT_CHECK_RESULT(numToCheck != 0, "tab bar not found", NULL);
    return tabBars.first();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickTab"
void GTTabWidget::clickTab(QTabWidget* const tabWidget, int tabIndex, Qt::MouseButton button) {
    GT_CHECK(tabWidget != NULL, "tabWidget is NULL");
    setCurrentIndex(tabWidget, tabIndex);
    QTabBar* tabBar = getTabBar(tabWidget);
    QRect r = tabBar->tabRect(tabIndex);
    GTMouseDriver::moveTo(tabBar->mapToGlobal(r.center()));
    GTMouseDriver::click(button);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickTab"
void GTTabWidget::clickTab(const QString& tabWidgetName, QWidget* parent, int tabIndex, Qt::MouseButton button) {
    clickTab(GTWidget::findTabWidget(tabWidgetName, parent), tabIndex, button);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickTab"
void GTTabWidget::clickTab(QTabWidget* const tabWidget, const QString& tabName, Qt::MouseButton button) {
    int num = getTabNumByName(tabWidget, tabName);
    clickTab(tabWidget, num, button);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickTab"
void GTTabWidget::clickTab(const QString& tabWidgetName, QWidget* parent, const QString& tabName, Qt::MouseButton button) {
    clickTab(GTWidget::findTabWidget(tabWidgetName, parent), tabName, button);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTabName"
QString GTTabWidget::getTabName(QTabWidget* tabWidget, int idx) {
    return getTabBar(tabWidget)->tabText(idx);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTabNameByWidget"
QString GTTabWidget::getTabNameByWidget(QTabWidget* tabWidget, QWidget* widget) {
    return getTabBar(tabWidget)->tabText(tabWidget->indexOf(widget));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTabNumByName"
int GTTabWidget::getTabNumByName(QTabWidget* tabWidget, QString tabName) {
    GT_CHECK_RESULT(tabWidget != NULL, "tabWidget is NULL", -1);
    QTabBar* tabBar = getTabBar(tabWidget);
    int num = -1;
    for (int i = 0; i < tabBar->count(); i++) {
        QString text = tabBar->tabText(i);
        if (text == tabName) {
            num = i;
            break;
        }
    }
    GT_CHECK_RESULT(num != -1, "tab " + tabName + " not found", -1);
    return num;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTabCornerWidget"
QWidget* GTTabWidget::getTabCornerWidget(QTabWidget* tabWidget, int idx) {
    QWidget* result = GTTabWidget::getTabBar(tabWidget)->tabButton(idx, QTabBar::RightSide);
    GT_CHECK_RESULT(result != NULL, "corner widget not found", NULL);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTabCornerWidget"
QWidget* GTTabWidget::getTabCornerWidget(QTabWidget* tabWidget, QString tabName) {
    int idx = getTabNumByName(tabWidget, tabName);
    return getTabCornerWidget(tabWidget, idx);
}
#undef GT_METHOD_NAME

void GTTabWidget::closeTab(QTabWidget* tabWidget, int idx) {
    GTWidget::click(getTabCornerWidget(tabWidget, idx));
}

void GTTabWidget::closeTab(QTabWidget* tabWidget, QString tabName) {
    GTWidget::click(getTabCornerWidget(tabWidget, tabName));
}

#undef GT_CLASS_NAME

}  // namespace HI
