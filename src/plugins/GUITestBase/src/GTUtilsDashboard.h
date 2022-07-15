/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include <QToolButton>

#include <U2Designer/Dashboard.h>
#include <U2Designer/ExternalToolsDashboardWidget.h>

#include "GTGlobals.h"

class QTabWidget;

namespace U2 {

class GTUtilsDashboard {
public:
    enum Tabs { Overview,
                Input,
                ExternalTools };

    // Dashboard notification class. Contains the type of notification (info/warning/error), the name of the workflow
    // element to which the notification relates and the notification message
    struct Notification {
        Notification() = delete;  // No default dashboard notification

        QString type;  // The type of dashboard notification (info/warning/error)
        QString element;  // The name of workflow element to which the dashboard notification relates
        QString message;  // The dashboard notification message

        // Returns the notification as a string
        QString toString() const;
    };

    /** Returns active dashboard or nullptr if not found. */
    static Dashboard* findDashboard(HI::GUITestOpStatus& os);

    /** Returns active dashboard. Fails if not found */
    static Dashboard* getDashboard(HI::GUITestOpStatus& os);

    /** Returns load-schema button or nullptr if not found. */
    static QToolButton* findLoadSchemaButton(HI::GUITestOpStatus& os);

    static QTabWidget* getTabWidget(HI::GUITestOpStatus& os);

    static const QString getDashboardName(HI::GUITestOpStatus& os, int dashboardNumber);

    static QStringList getInputFiles(HI::GUITestOpStatus& os);

    static QStringList getOutputFiles(HI::GUITestOpStatus& os);

    static void clickOutputFile(HI::GUITestOpStatus& os, const QString& outputFileName);

    static QString getTabObjectName(Tabs tab);

    static bool hasNotifications(HI::GUITestOpStatus& os);

    // Returns a list of dashboard notifications
    static QList<Notification> getNotifications(HI::GUITestOpStatus& os);

    // Returns a QString containing all dashboard notifications, splitted by '\n'
    static QString getJoinedNotificationsString(HI::GUITestOpStatus& os);

    static void openTab(HI::GUITestOpStatus& os, Tabs tab);

    static bool hasTab(HI::GUITestOpStatus& os, Tabs tab);

    // External tools tab
    static QString getNodeText(HI::GUITestOpStatus& os, const QString& nodeId);

    static int getChildrenNodesCount(HI::GUITestOpStatus& os, const QString& nodeId);

    static QList<ExternalToolsTreeNode*> getChildNodes(HI::GUITestOpStatus& os, const QString& nodeId);

    static QString getChildNodeId(HI::GUITestOpStatus& os, const QString& nodeId, int childIndex);
    static QString getDescendantNodeId(HI::GUITestOpStatus& os, const QString& nodeId, const QList<int>& childIndexes);
    static QString getChildWithTextId(HI::GUITestOpStatus& os, const QString& nodeId, const QString& text);  // childrens has to have unique texts

    static bool hasLimitationMessage(HI::GUITestOpStatus& os, const QString& nodeId);
    static QString getLimitationMessage(HI::GUITestOpStatus& os, const QString& nodeId);
    static QString getLogUrlFromNodeLimitationMessage(HI::GUITestOpStatus& os, const QString& nodeId);
    static QString getLogUrlFromOutputContent(HI::GUITestOpStatus& os, const QString& outputNodeId);

    static QSize getCopyButtonSize(HI::GUITestOpStatus& os, const QString& toolRunNodeId);
    static void clickCopyButton(HI::GUITestOpStatus& os, const QString& toolRunNodeId);

    static bool isNodeVisible(HI::GUITestOpStatus& os, const QString& nodeId);
    static bool isNodeCollapsed(HI::GUITestOpStatus& os, const QString& nodeId);
    static void collapseNode(HI::GUITestOpStatus& os, const QString& nodeId);
    static void expandNode(HI::GUITestOpStatus& os, const QString& nodeId);
    static void clickNodeTitle(HI::GUITestOpStatus& os, ExternalToolsTreeNode* node);

    /** Returns DashboardTabPage widget with for 'Input' tab. The widget must be visible. */
    static QWidget* getInputTabPageWidget(HI::GUITestOpStatus& os);

    /** Click on label with the given text in the currently visible 'ParametersDashboardWidget'. */
    static void clickLabelInParametersWidget(HI::GUITestOpStatus& os, const QString& labelText);

    /** Click on file button with the given text in the currently visible 'ParametersDashboardWidget'. */
    static void clickFileButtonInParametersWidget(HI::GUITestOpStatus& os, const QString& buttonText);

    static const QString TREE_ROOT_ID;

    static QWidget* getCopyButton(HI::GUITestOpStatus& os, const QString& toolRunNodeId);
    static ExternalToolsDashboardWidget* getExternalToolsWidget(HI::GUITestOpStatus& os);
    static ExternalToolsTreeNode* getExternalToolNode(HI::GUITestOpStatus& os, const QString& nodeId);

    /** Returns external tool matched by text on any level. */
    static ExternalToolsTreeNode* getExternalToolNodeByText(HI::GUITestOpStatus& os, const QString& textPattern, bool isExactMatch = true);

    /** Returns external tool matched by text with the given parent only. If parent is nullptr - any level is searched. */
    static ExternalToolsTreeNode* getExternalToolNodeByText(HI::GUITestOpStatus& os, ExternalToolsTreeNode* parent, const QString& textPattern, bool isExactMatch = true);

    /** Returns list of external tool nodes matched by text with the given parent only. If parent is nullptr - any level is searched. */
    static QList<ExternalToolsTreeNode*> getExternalToolNodesByText(HI::GUITestOpStatus& os, ExternalToolsTreeNode* parent, const QString& textPattern, bool isExactMatch = true);

private:
    /** The <img> tag with the class attribute is searched for in the |html|. The attribute value is the type of notification. */
    static QString getNotificationTypeFromHtml(HI::GUITestOpStatus& os, const QString& html);

    /** Returns text from the (row, column) notification table cell. The (row, column) cell must exist. */
    static QString getNotificationCellText(HI::GUITestOpStatus& os, const QGridLayout& tableLayout, int row, int col);

    static const QMap<QString, Tabs> tabMap;
};

}  // namespace U2

#endif  // _U2_GT_UTILS_DASHBOARD_H_
