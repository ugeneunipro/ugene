/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <QWebView>
#include <QWebElement>

#include <primitives/GTWebView.h>
#include <primitives/GTWidget.h>

#include "GTUtilsMdi.h"

#include "GTUtilsStartPage.h"

namespace U2 {
#define GT_CLASS_NAME "GTUtilsStartPage"

void GTUtilsStartPage::openStartPage(HI::GUITestOpStatus &os){
    GTUtilsMdi::activateWindow(os, "Start Page");
}

QWebView* GTUtilsStartPage::getStartPage(HI::GUITestOpStatus &os){
    QWidget* startPageWidget = GTWidget::findWidget(os, "Start Page");
    QWebView* result = GTWidget::findExactWidget<QWebView*>(os, "webView", startPageWidget);
    return result;
}

#define GT_METHOD_NAME "getButton"
HIWebElement GTUtilsStartPage::getButton(HI::GUITestOpStatus &os, Buttons button) {
    switch (button) {
    case OpenFile:
        return GTWebView::findElement(os, getStartPage(os), "Open File(s)", "DIV");
    case CreateSequence:
        return GTWebView::findElement(os, getStartPage(os), "Create Sequence", "DIV");
    case CreateWorkflow:
        return GTWebView::findElement(os, getStartPage(os), "Run or Create Workflow", "DIV");
    case QuickStartGuide:
         // You do not need this button
        GT_CHECK_RESULT(false, "Not implemented", QWebElement());
    default:
        GT_CHECK_RESULT(false, "Not implemented", QWebElement());
    }
}
#undef GT_METHOD_NAME

void GTUtilsStartPage::clickButton(HI::GUITestOpStatus &os, Buttons b) {
    HIWebElement but = getButton(os, b);
    QWebView* start = getStartPage(os);
    GTWebView::click(os, start, but);
}

void GTUtilsStartPage::clickResentDocument(HI::GUITestOpStatus &os, QString text){
    HIWebElement el = GTWebView::findElement(os, getStartPage(os), "- " + text, "A");
    GTWebView::click(os, getStartPage(os), el);
}

void GTUtilsStartPage::clickResentProject(HI::GUITestOpStatus &os, QString text){
    clickResentDocument(os, text);
}

#undef GT_CLASS_NAME

}
