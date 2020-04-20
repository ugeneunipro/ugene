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

#ifndef _U2_GT_UTILS_DASHBOARD_H_
#define _U2_GT_UTILS_DASHBOARD_H_

#include "GTGlobals.h"
#include <primitives/GTWebView.h>

#include <U2Gui/U2WebView.h>

class QTabWidget;

namespace U2 {

class GTUtilsDashboard {
public:
    enum Tabs{Overview, Input, ExternalTools};
    static WebView* getDashboard(HI::GUITestOpStatus &os);
    static QTabWidget* getTabWidget(HI::GUITestOpStatus &os);

    static const QString getDashboardName(HI::GUITestOpStatus &os, int dashboardNumber);

    static QStringList getOutputFiles(HI::GUITestOpStatus &os);
    static void clickOutputFile(HI::GUITestOpStatus &os, const QString &outputFileName);

    static HI::HIWebElement findElement(HI::GUITestOpStatus &os, QString text, QString tag = "*", bool exactMatch = false);
    static HI::HIWebElement findTreeElement(HI::GUITestOpStatus &os, QString text);
    static HI::HIWebElement findContextMenuElement(HI::GUITestOpStatus &os, QString text);
    static void click(HI::GUITestOpStatus &os, HI::HIWebElement el, Qt::MouseButton button = Qt::LeftButton);
    static bool areThereNotifications(HI::GUITestOpStatus &os);
    static void openTab(HI::GUITestOpStatus &os, Tabs tab);

    static bool doesTabExist(HI::GUITestOpStatus &os, Tabs tab);

// External tools tab
    static QString getRootNode(HI::GUITestOpStatus &os);
    static QString getNodeText(HI::GUITestOpStatus &os, const QString &nodeId);
    static int getChildrenNodesCount(HI::GUITestOpStatus &os, const QString &nodeId);
    static QString getChildNodeId(HI::GUITestOpStatus &os, const QString &nodeId, int childNum);
    static QString getDescendantNodeId(HI::GUITestOpStatus &os, const QString &nodeId, const QList<int> &childNums);
    static QString getChildWithTextId(HI::GUITestOpStatus &os, const QString &nodeId, const QString &text);     // childrens has to have unique texts

    static bool doesNodeHaveLimitationMessageNode(HI::GUITestOpStatus &os, const QString &nodeId);
    static QString getLimitationMessageNodeText(HI::GUITestOpStatus &os, const QString &nodeId);
    static QString getLimitationMessageLogUrl(HI::GUITestOpStatus &os, const QString &nodeId);

    static QSize getCopyButtonSize(HI::GUITestOpStatus &os, const QString &toolRunNodeId);
    static void clickCopyButton(HI::GUITestOpStatus &os, const QString &toolRunNodeId);

    // All parent nodes should be expanded
    static bool isNodeVisible(HI::GUITestOpStatus &os, const QString &nodeId);
    static bool isNodeCollapsed(HI::GUITestOpStatus &os, const QString &nodeId);
    static void collapseNode(HI::GUITestOpStatus &os, const QString &nodeId);
    static void expandNode(HI::GUITestOpStatus &os, const QString &nodeId);

    static QString getLogUrlFromNode(HI::GUITestOpStatus &os, const QString &outputNodeId);

    static const QString TREE_ROOT_ID;      // This constant is defined in ExternalToolWidget.js

private:
    static QMap<QString, Tabs> initTabMap();
    static QString getNodeSpanId(const QString &nodeId);
    static HI::HIWebElement getCopyButton(HI::GUITestOpStatus &os, const QString &toolRunNodeId);
    static HI::HIWebElement getNodeSpan(HI::GUITestOpStatus &os, const QString &nodeId);
    static HI::HIWebElement getNodeUl(HI::GUITestOpStatus &os, const QString &nodeId);

    static QString getLogUrlFromElement(HI::GUITestOpStatus &os, const HI::HIWebElement &element);

    static const QMap<QString, Tabs> tabMap;
    static const QString PARENT_LI;     // This constant is defined in ExternalToolWidget.js

    // Some CSS attributes
    static const QString WIDTH;
    static const QString HEIGHT;
    static const QString TITLE;
    static const QString COLLAPSED_NODE_TITLE;
    static const QString ON_CLICK;
};

}   // namespace U2

#endif // _U2_GT_UTILS_DASHBOARD_H_
