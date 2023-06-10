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
    static Dashboard* findDashboard();

    /** Returns active dashboard. Fails if not found */
    static Dashboard* getDashboard();

    /** Returns load-schema button or nullptr if not found. */
    static QToolButton* findLoadSchemaButton();

    static QTabWidget* getTabWidget();

    static QString getDashboardName(int dashboardNumber);

    static QStringList getInputFiles();

    static QStringList getOutputFiles();

    /** Returns list of output file URLs: 1 per output file button. */
    static QStringList getOutputFileUrls();

    static void clickOutputFile(const QString& outputFileName);

    static QString getTabObjectName(Tabs tab);

    static bool hasNotifications();

    // Returns a list of dashboard notifications
    static QList<Notification> getNotifications();

    // Returns a QString containing all dashboard notifications, splitted by '\n'
    static QString getJoinedNotificationsString();

    static void openTab(Tabs tab);

    static bool hasTab(Tabs tab);

    // External tools tab
    static QString getNodeText(const QString& nodeId);

    static int getChildrenNodesCount(const QString& nodeId);

    static QList<ExternalToolsTreeNode*> getChildNodes(const QString& nodeId);

    static QString getChildNodeId(const QString& nodeId, int childIndex);
    static QString getDescendantNodeId(const QString& nodeId, const QList<int>& childIndexes);
    static QString getChildWithTextId(const QString& nodeId, const QString& text);  // childrens has to have unique texts

    static bool hasLimitationMessage(const QString& nodeId);
    static QString getLimitationMessage(const QString& nodeId);
    static QString getLogUrlFromNodeLimitationMessage(const QString& nodeId);
    static QString getLogUrlFromOutputContent(const QString& outputNodeId);

    static QSize getCopyButtonSize(const QString& toolRunNodeId);
    static void clickCopyButton(const QString& toolRunNodeId);

    static bool isNodeVisible(const QString& nodeId);
    static bool isNodeCollapsed(const QString& nodeId);
    static void collapseNode(const QString& nodeId);
    static void expandNode(const QString& nodeId);
    static void clickNodeTitle(ExternalToolsTreeNode* node);

    /** Returns DashboardTabPage widget with for 'Input' tab. The widget must be visible. */
    static QWidget* getInputTabPageWidget();

    /** Click on label with the given text in the currently visible 'ParametersDashboardWidget'. */
    static void clickLabelInParametersWidget(const QString& labelText);

    /** Click on file button with the given text in the currently visible 'ParametersDashboardWidget'. */
    static void clickFileButtonInParametersWidget(const QString& buttonText);

    static const QString TREE_ROOT_ID;

    static QWidget* getCopyButton(const QString& toolRunNodeId);
    static ExternalToolsDashboardWidget* getExternalToolsWidget();
    static ExternalToolsTreeNode* getExternalToolNode(const QString& nodeId);

    /** Returns external tool matched by text on any level. */
    static ExternalToolsTreeNode* getExternalToolNodeByText(const QString& textPattern, bool isExactMatch = true);

    /** Returns external tool matched by text with the given parent only. If parent is nullptr - any level is searched. */
    static ExternalToolsTreeNode* getExternalToolNodeByText(ExternalToolsTreeNode* parent, const QString& textPattern, bool isExactMatch = true);

    /** Returns list of external tool nodes matched by text with the given parent only. If parent is nullptr - any level is searched. */
    static QList<ExternalToolsTreeNode*> getExternalToolNodesByText(ExternalToolsTreeNode* parent, const QString& textPattern, bool isExactMatch = true);

private:
    /** The <img> tag with the class attribute is searched for in the |html|. The attribute value is the type of notification. */
    static QString getNotificationTypeFromHtml(const QString& html);

    /** Returns text from the (row, column) notification table cell. The (row, column) cell must exist. */
    static QString getNotificationCellText(const QGridLayout& tableLayout, int row, int col);

    static const QMap<QString, Tabs> tabMap;
};

}  // namespace U2
