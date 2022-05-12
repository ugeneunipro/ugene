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

#include <GTUtilsMdi.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTWidget.h>

#include <QRegularExpression>
#include <QStringBuilder>
#include <QTabWidget>

#include <U2Gui/HoverQLabel.h>

#include "GTUtilsDashboard.h"

namespace U2 {
using namespace HI;

const QString GTUtilsDashboard::TREE_ROOT_ID = ExternalToolsDashboardWidget::TREE_ID;

QString GTUtilsDashboard::Notification::toString() const {
    return "[" % type.toUpper() % "][" % element % "] " % message;
}

#define GT_CLASS_NAME "GTUtilsDashboard"

#define GT_METHOD_NAME "getCopyButton"
QWidget* GTUtilsDashboard::getCopyButton(GUITestOpStatus& os, const QString& toolRunNodeId) {
    auto node = getExternalToolNode(os, toolRunNodeId);
    return GTWidget::findWidget(os, "copyButton", node);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExternalToolsWidget"
ExternalToolsDashboardWidget* GTUtilsDashboard::getExternalToolsWidget(GUITestOpStatus& os) {
    Dashboard* dashboard = getDashboard(os);
    return GTWidget::findWidgetByType<ExternalToolsDashboardWidget*>(os, dashboard, "External tools widget is not found");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExternalToolNode"
ExternalToolsTreeNode* GTUtilsDashboard::getExternalToolNode(GUITestOpStatus& os, const QString& nodeId) {
    ExternalToolsDashboardWidget* widget = getExternalToolsWidget(os);
    return GTWidget::findExactWidget<ExternalToolsTreeNode*>(os, nodeId, widget);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExternalToolNodeByText"
ExternalToolsTreeNode* GTUtilsDashboard::getExternalToolNodeByText(GUITestOpStatus& os, const QString& textPattern, bool isExactMatch) {
    return getExternalToolNodeByText(os, nullptr, textPattern, isExactMatch);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExternalToolNodesByText"
QList<ExternalToolsTreeNode*> GTUtilsDashboard::getExternalToolNodesByText(HI::GUITestOpStatus& os, ExternalToolsTreeNode* parent, const QString& textPattern, bool isExactMatch) {
    QList<ExternalToolsTreeNode*> nodes = parent == nullptr ? getExternalToolsWidget(os)->findChildren<ExternalToolsTreeNode*>() : parent->children;
    QList<ExternalToolsTreeNode*> result;
    for (auto node : qAsConst(nodes)) {
        if (node->content == textPattern) {
            result << node;
        } else if (!isExactMatch && node->content.contains(textPattern)) {
            result << node;
        }
    }
    return result;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExternalToolNodeByTextWithParent"
ExternalToolsTreeNode* GTUtilsDashboard::getExternalToolNodeByText(GUITestOpStatus& os, ExternalToolsTreeNode* parent, const QString& textPattern, bool isExactMatch) {
    QList<ExternalToolsTreeNode*> nodes = parent == nullptr ? getExternalToolsWidget(os)->findChildren<ExternalToolsTreeNode*>() : parent->children;
    for (auto node : qAsConst(nodes)) {
        if (node->content == textPattern) {
            return node;
        } else if (!isExactMatch && node->content.contains(textPattern)) {
            return node;
        }
    }
    GT_CHECK_RESULT(false, "External tool node by text not found: " + textPattern, nullptr);
}
#undef GT_METHOD_NAME

QTabWidget* GTUtilsDashboard::getTabWidget(HI::GUITestOpStatus& os) {
    return GTWidget::findTabWidget(os, "WorkflowTabView", GTUtilsMdi::activeWindow(os));
}

QToolButton* GTUtilsDashboard::findLoadSchemaButton(HI::GUITestOpStatus& os) {
    Dashboard* dashboard = findDashboard(os);
    return dashboard == nullptr ? nullptr : dashboard->findChild<QToolButton*>("loadSchemaButton");
}

const QString GTUtilsDashboard::getDashboardName(GUITestOpStatus& os, int dashboardNumber) {
    return GTTabWidget::getTabName(os, getTabWidget(os), dashboardNumber);
}

static QStringList getFileButtonLabels(QWidget* parentWidget) {
    QList<QToolButton*> buttons = parentWidget->findChildren<QToolButton*>();
    QStringList labels;
    for (auto button : qAsConst(buttons)) {
        labels << button->text();
    }
    return labels;
}

QStringList GTUtilsDashboard::getInputFiles(HI::GUITestOpStatus& os) {
    openTab(os, Input);
    auto dashboard = getDashboard(os);
    auto parametersWidget = GTWidget::findWidget(os, "ParametersDashboardWidget", dashboard);
    return getFileButtonLabels(parametersWidget);
}

QStringList GTUtilsDashboard::getOutputFiles(HI::GUITestOpStatus& os) {
    auto dashboard = getDashboard(os);
    auto outputFilesWidget = GTWidget::findWidget(os, "OutputFilesDashboardWidget", dashboard);
    return getFileButtonLabels(outputFilesWidget);
}

#define GT_METHOD_NAME "clickOutputFile"
void GTUtilsDashboard::clickOutputFile(GUITestOpStatus& os, const QString& outputFileName) {
    auto dashboard = getDashboard(os);
    auto outputFilesWidget = GTWidget::findWidget(os, "OutputFilesDashboardWidget", dashboard);
    auto button = GTWidget::findButtonByText(os, outputFileName, outputFilesWidget);
    GTWidget::click(os, button);
}
#undef GT_METHOD_NAME

bool GTUtilsDashboard::hasNotifications(HI::GUITestOpStatus& os) {
    openTab(os, Overview);
    auto dashboard = getDashboard(os);
    auto notificationsWidget = GTWidget::findWidget(os, "NotificationsDashboardWidget", dashboard);
    return notificationsWidget->isVisible();
}

#define GT_METHOD_NAME "getNotificationTypeFromHtml"
QString GTUtilsDashboard::getNotificationTypeFromHtml(HI::GUITestOpStatus& os, const QString& html) {
    QString type;

    int start = html.indexOf("<img class=\"");
    int end = html.indexOf("\"", start + 12);  // 12 = length of "<img class=\""
    GT_CHECK_RESULT(start >= 0 && end >= 0, "Dashboard notification type not found", type)

    start += 12;
    end -= start;
    type = html.mid(start, end);
    return type;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNotificationCellText"
QString GTUtilsDashboard::getNotificationCellText(HI::GUITestOpStatus& os, const QGridLayout& tableLayout, const int row, const int col) {
    const QWidget* cellWidget = tableLayout.itemAtPosition(row, col)->widget();
    QString text;

    if (cellWidget != nullptr && cellWidget->objectName() == "tableCell") {
        if (const QLayout* const cellLayout = cellWidget->layout()) {
            for (int labelInd = 0; labelInd < cellLayout->count(); ++labelInd) {
                if (const auto* const label = qobject_cast<QLabel*>(cellLayout->itemAt(labelInd)->widget())) {
                    text = label->text();
                }
            }
        }
    }
    GT_CHECK_RESULT(!text.isEmpty(),
                    QString("Error getting (%1,%2) cell of dashboard notification table").arg(row).arg(col),
                    text)
    return text;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNotifications"
QList<GTUtilsDashboard::Notification> GTUtilsDashboard::getNotifications(GUITestOpStatus& os) {
    const QString notificationsWidgetName = "NotificationsDashboardWidget";
    auto notificationsWidget = GTWidget::findWidget(os, notificationsWidgetName, GTUtilsDashboard::getDashboard(os));
    auto tableLayout = qobject_cast<QGridLayout*>(notificationsWidget->layout());
    QList<Notification> notifications;

    GT_CHECK_RESULT(tableLayout != nullptr && tableLayout->columnCount() == 3,
                    notificationsWidgetName % " was found, but cannot be used in a test",
                    notifications)

    const int notificationsNumber = tableLayout->rowCount();
    for (int row = 1; row < notificationsNumber; row++) {
        const QString type = getNotificationTypeFromHtml(os, getNotificationCellText(os, *tableLayout, row, 0));
        const QString element = getNotificationCellText(os, *tableLayout, row, 1);
        const QString message = getNotificationCellText(os, *tableLayout, row, 2);
        notifications << Notification {type, element, message};
    }
    return notifications;
}
#undef GT_METHOD_NAME

QString GTUtilsDashboard::getJoinedNotificationsString(GUITestOpStatus& os) {
    const auto notifications = getNotifications(os);
    QStringList stringNotifications;
    for (const auto& notification : qAsConst(notifications)) {
        stringNotifications << notification.toString();
    }
    return stringNotifications.join('\n');
}

QString GTUtilsDashboard::getTabObjectName(Tabs tab) {
    switch (tab) {
        case Overview:
            return "overviewTabButton";
        case Input:
            return "inputTabButton";
        case ExternalTools:
            return "externalToolsTabButton";
    }
    return "unknown tab";
}

#define GT_METHOD_NAME "findDashboard"
Dashboard* GTUtilsDashboard::findDashboard(HI::GUITestOpStatus& os) {
    QTabWidget* tabWidget = getTabWidget(os);
    return tabWidget == nullptr ? nullptr : qobject_cast<Dashboard*>(tabWidget->currentWidget());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDashboard"
Dashboard* GTUtilsDashboard::getDashboard(HI::GUITestOpStatus& os) {
    auto dashboard = findDashboard(os);
    GT_CHECK_RESULT(dashboard != nullptr, "Dashboard widget not found", nullptr);
    return dashboard;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openTab"
void GTUtilsDashboard::openTab(HI::GUITestOpStatus& os, Tabs tab) {
    QWidget* dashboard = findDashboard(os);
    GT_CHECK(dashboard != nullptr, "Dashboard widget not found");

    QString tabButtonObjectName = getTabObjectName(tab);
    auto tabButton = GTWidget::findToolButton(os, tabButtonObjectName, dashboard);

    GTWidget::click(os, tabButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "hasTab"
bool GTUtilsDashboard::hasTab(HI::GUITestOpStatus& os, Tabs tab) {
    QWidget* dashboard = findDashboard(os);
    GT_CHECK_RESULT(dashboard != nullptr, "Dashboard is not found", false);

    QString tabButtonObjectName = getTabObjectName(tab);
    QWidget* button = dashboard->findChild<QWidget*>(tabButtonObjectName);
    return button != nullptr && button->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNodeText"
QString GTUtilsDashboard::getNodeText(GUITestOpStatus& os, const QString& nodeId) {
    return getExternalToolNode(os, nodeId)->content;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getChildrenNodesCount"
int GTUtilsDashboard::getChildrenNodesCount(GUITestOpStatus& os, const QString& nodeId) {
    return nodeId == TREE_ROOT_ID ? getExternalToolsWidget(os)->getTopLevelNodes().size() : getExternalToolNode(os, nodeId)->children.count();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getChildNodes"
QList<ExternalToolsTreeNode*> GTUtilsDashboard::getChildNodes(GUITestOpStatus& os, const QString& nodeId) {
    return nodeId == TREE_ROOT_ID ? getExternalToolsWidget(os)->getTopLevelNodes() : getExternalToolNode(os, nodeId)->children;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getChildNodeId"
QString GTUtilsDashboard::getChildNodeId(GUITestOpStatus& os, const QString& nodeId, int childIndex) {
    return getDescendantNodeId(os, nodeId, {childIndex});
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDescendantNodeId"
QString GTUtilsDashboard::getDescendantNodeId(GUITestOpStatus& os, const QString& nodeId, const QList<int>& childIndexes) {
    QList<ExternalToolsTreeNode*> childNodes = getChildNodes(os, nodeId);
    QString resultNodeId = nodeId;
    for (int i : qAsConst(childIndexes)) {
        GT_CHECK_RESULT(i >= 0 && i < childNodes.size(), "Illegal child index: " + QString::number(i) + ", nodes: " + childNodes.size(), "");
        resultNodeId = childNodes[i]->objectName();
        childNodes = childNodes[i]->children;
    }
    return resultNodeId;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getChildWithTextId"
QString GTUtilsDashboard::getChildWithTextId(GUITestOpStatus& os, const QString& nodeId, const QString& text) {
    int childrenCount = getChildrenNodesCount(os, nodeId);
    QString resultChildId;
    QStringList quotedChildrenTexts;
    for (int i = 0; i < childrenCount; i++) {
        const QString currentChildId = getChildNodeId(os, nodeId, i);
        const QString childText = getNodeText(os, currentChildId);
        quotedChildrenTexts << "\'" + childText + "\'";
        if (text == childText) {
            GT_CHECK_RESULT(resultChildId.isEmpty(),
                            QString("Expected text '%1' is not unique among the node with ID '%2' children")
                                .arg(text)
                                .arg(nodeId),
                            "");
            resultChildId = currentChildId;
        }
    }

    GT_CHECK_RESULT(!resultChildId.isEmpty(),
                    QString("Child with text '%1' not found among the node with ID '%2' children; there are children with the following texts: %3")
                        .arg(text)
                        .arg(nodeId)
                        .arg(quotedChildrenTexts.join(", ")),
                    "");

    return resultChildId;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "hasLimitationMessage"
bool GTUtilsDashboard::hasLimitationMessage(GUITestOpStatus& os, const QString& nodeId) {
    return !getLimitationMessage(os, nodeId).isEmpty();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLimitationMessage"
QString GTUtilsDashboard::getLimitationMessage(GUITestOpStatus& os, const QString& nodeId) {
    return nodeId == TREE_ROOT_ID ? getExternalToolsWidget(os)->getLimitationWarningHtml() : getExternalToolNode(os, nodeId)->limitationWarningHtml;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "parseUrlFromContent"
static QString parseUrlFromContent(GUITestOpStatus& os, const QString& content) {
    QString urlStartToken = "<a href=\"";
    int urlStartTokenIdx = content.lastIndexOf(urlStartToken);
    GT_CHECK_RESULT(urlStartTokenIdx > 0, "urlStartToken is not found, text: " + content, "");
    int urlStartIdx = urlStartTokenIdx + urlStartToken.length();
    int urlEndIdx = content.indexOf("\"", urlStartIdx + 1);
    GT_CHECK_RESULT(urlEndIdx > 0, "urlEndToken is not found, text: " + content, "");
    return content.mid(urlStartIdx, urlEndIdx - urlStartIdx);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLimitationMessageLogUrl"
QString GTUtilsDashboard::getLogUrlFromNodeLimitationMessage(GUITestOpStatus& os, const QString& nodeId) {
    QString limitationMessage = getLimitationMessage(os, nodeId);
    return parseUrlFromContent(os, limitationMessage);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLogUrlFromOutputContent"
QString GTUtilsDashboard::getLogUrlFromOutputContent(GUITestOpStatus& os, const QString& outputNodeId) {
    auto content = getExternalToolNode(os, outputNodeId)->content;
    return parseUrlFromContent(os, content);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getCopyButtonSize"
QSize GTUtilsDashboard::getCopyButtonSize(GUITestOpStatus& os, const QString& toolRunNodeId) {
    return getCopyButton(os, toolRunNodeId)->rect().size();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickCopyButton"
void GTUtilsDashboard::clickCopyButton(GUITestOpStatus& os, const QString& toolRunNodeId) {
    GTWidget::click(os, getCopyButton(os, toolRunNodeId));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isNodeVisible"
bool GTUtilsDashboard::isNodeVisible(GUITestOpStatus& os, const QString& nodeId) {
    return getExternalToolNode(os, nodeId)->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isNodeCollapsed"
bool GTUtilsDashboard::isNodeCollapsed(GUITestOpStatus& os, const QString& nodeId) {
    return !getExternalToolNode(os, nodeId)->isExpanded();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "collapseNode"
void GTUtilsDashboard::collapseNode(GUITestOpStatus& os, const QString& nodeId) {
    GT_CHECK(isNodeVisible(os, nodeId), QString("Node with ID '%1' is not visible. Some of the parent nodes are collapsed?").arg(nodeId));
    GT_CHECK(!isNodeCollapsed(os, nodeId), QString("Node with ID '%1' is already collapsed.").arg(nodeId));
    clickNodeTitle(os, getExternalToolNode(os, nodeId));
    GT_CHECK(isNodeCollapsed(os, nodeId), QString("Node with ID '%1' was not collapsed.").arg(nodeId));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "expandNode"
void GTUtilsDashboard::expandNode(GUITestOpStatus& os, const QString& nodeId) {
    GT_CHECK(isNodeVisible(os, nodeId), QString("Node with ID '%1' is not visible. Some of the parent nodes are collapsed?").arg(nodeId));
    GT_CHECK(isNodeCollapsed(os, nodeId), QString("Node with ID '%1' is already expanded.").arg(nodeId));
    clickNodeTitle(os, getExternalToolNode(os, nodeId));
    GT_CHECK(!isNodeCollapsed(os, nodeId), QString("Node with ID '%1' was not expanded.").arg(nodeId));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickNodeTitle"
void GTUtilsDashboard::clickNodeTitle(GUITestOpStatus& os, ExternalToolsTreeNode* node) {
    GT_CHECK(node != nullptr, "Node is null!");
    GT_CHECK(node->badgeLabel->titleLabel != nullptr, "Node title label is null!");
    GTWidget::click(os, node->badgeLabel->titleLabel);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
