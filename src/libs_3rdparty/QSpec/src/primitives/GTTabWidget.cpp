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

#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include "primitives/GTTabBar.h"
#include "primitives/GTTabWidget.h"
#include <primitives/GTWidget.h>

namespace HI {

#define GT_CLASS_NAME "GTTabWidget"

#define GT_METHOD_NAME "setCurrentIndex"
void GTTabWidget::setCurrentIndex(GUITestOpStatus& os, QTabWidget * const tabWidget, int index) {

    GT_CHECK(tabWidget != NULL, "QTabWidget* == NULL");

    int tabsCount = tabWidget->count();
    GT_CHECK(index>=0 && index<tabsCount, "invalid index");

    QTabBar* tabBar = tabWidget->findChild<QTabBar*>();
    GTTabBar::setCurrentIndex(os, tabBar, index);

    int currIndex = tabWidget->currentIndex();
    GT_CHECK(currIndex == index, "Can't set index");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTabBar"
QTabBar* GTTabWidget::getTabBar(GUITestOpStatus &os, QTabWidget* tabWidget){
    Q_UNUSED(os)
    GT_CHECK_RESULT(tabWidget != NULL, "tabWidget is NULL", NULL);
    QList<QTabBar*> tabBars= tabWidget->findChildren<QTabBar*>();
    int numToCheck = tabBars.size();
    GT_CHECK_RESULT(numToCheck < 2, QString("too many tab bars found: ").arg(numToCheck), NULL);
    GT_CHECK_RESULT(numToCheck != 0, "tab bar not found", NULL);
    return tabBars.first();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickTab"
void GTTabWidget::clickTab(GUITestOpStatus &os, QTabWidget * const tabWidget, int tabIndex, Qt::MouseButton button){
    GT_CHECK(tabWidget != NULL, "tabWidget is NULL");
    setCurrentIndex(os, tabWidget, tabIndex);
    QTabBar* tabBar = getTabBar(os, tabWidget);
    QRect r = tabBar->tabRect(tabIndex);
    GTMouseDriver::moveTo(tabBar->mapToGlobal(r.center()));
    GTMouseDriver::click(button);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickTab"
void GTTabWidget::clickTab(GUITestOpStatus &os, const QString &tabWidgetName, QWidget const * const parent, int tabIndex, Qt::MouseButton button) {
    clickTab(os, GTWidget::findExactWidget<QTabWidget *>(os, tabWidgetName, parent), tabIndex, button);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickTab"
void GTTabWidget::clickTab(GUITestOpStatus &os, QTabWidget * const tabWidget, const QString &tabName, Qt::MouseButton button){
    int num = getTabNumByName(os, tabWidget, tabName);
    clickTab(os, tabWidget, num, button);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickTab"
void GTTabWidget::clickTab(GUITestOpStatus &os, const QString &tabWidgetName, QWidget const * const parent, const QString &tabName, Qt::MouseButton button) {
    clickTab(os, GTWidget::findExactWidget<QTabWidget *>(os, tabWidgetName, parent), tabName, button);
}
#undef GT_METHOD_NAME

QString GTTabWidget::getTabName(GUITestOpStatus &os, QTabWidget *tabWidget, int idx){
    return getTabBar(os, tabWidget)->tabText(idx);
}

#define GT_METHOD_NAME "getTabNumByName"
int GTTabWidget::getTabNumByName(GUITestOpStatus &os, QTabWidget *tabWidget, QString tabName){
    GT_CHECK_RESULT(tabWidget != NULL, "tabWidget is NULL", -1);
    QTabBar* tabBar = getTabBar(os, tabWidget);
    int num = -1;
    for(int i=0; i<tabBar->count(); i++){
        QString text = tabBar->tabText(i);
        if(text == tabName){
            num = i;
            break;
        }
    }
    GT_CHECK_RESULT(num != -1, "tab " + tabName + " not found", -1);
    return num;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTabCornerWidget"
QWidget* GTTabWidget::getTabCornerWidget(GUITestOpStatus &os, QTabWidget *tabWidget, int idx){
    QWidget* result = GTTabWidget::getTabBar(os, tabWidget)->tabButton(idx, QTabBar::RightSide);
    GT_CHECK_RESULT(result != NULL, "corner widget not found", NULL);
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTabCornerWidget"
QWidget* GTTabWidget::getTabCornerWidget(GUITestOpStatus &os, QTabWidget *tabWidget, QString tabName){
    int idx = getTabNumByName(os, tabWidget, tabName);
    return getTabCornerWidget(os, tabWidget, idx);
}
#undef GT_METHOD_NAME

void GTTabWidget::closeTab(GUITestOpStatus &os, QTabWidget *tabWidget, int idx){
    GTWidget::click(os, getTabCornerWidget(os, tabWidget, idx));
}

void GTTabWidget::closeTab(GUITestOpStatus &os, QTabWidget *tabWidget, QString tabName){
    GTWidget::click(os, getTabCornerWidget(os, tabWidget, tabName));
}

#undef GT_CLASS_NAME

}
