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
QWidget* GTUtilsDashboard::getCopyButton(const QString& toolRunNodeId) {
    auto node = getExternalToolNode(toolRunNodeId);
    return GTWidget::findWidget("copyButton", node);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExternalToolsWidget"
ExternalToolsDashboardWidget* GTUtilsDashboard::getExternalToolsWidget() {
    Dashboard* dashboard = getDashboard();
    return GTWidget::findWidgetByType<ExternalToolsDashboardWidget*>(dashboard, "External tools widget is not found");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExternalToolNode"
ExternalToolsTreeNode* GTUtilsDashboard::getExternalToolNode(const QString& nodeId) {
    ExternalToolsDashboardWidget* widget = getExternalToolsWidget();
    return GTWidget::findExactWidget<ExternalToolsTreeNode*>(nodeId, widget);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExternalToolNodeByText"
ExternalToolsTreeNode* GTUtilsDashboard::getExternalToolNodeByText(const QString& textPattern, bool isExactMatch) {
    return getExternalToolNodeByText(nullptr, textPattern, isExactMatch);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExternalToolNodesByText"
QList<ExternalToolsTreeNode*> GTUtilsDashboard::getExternalToolNodesByText(ExternalToolsTreeNode* parent, const QString& textPattern, bool isExactMatch) {
    QList<ExternalToolsTreeNode*> nodes = parent == nullptr ? getExternalToolsWidget()->findChildren<ExternalToolsTreeNode*>() : parent->children;
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
ExternalToolsTreeNode* GTUtilsDashboard::getExternalToolNodeByText(ExternalToolsTreeNode* parent, const QString& textPattern, bool isExactMatch) {
    QList<ExternalToolsTreeNode*> nodes = parent == nullptr ? getExternalToolsWidget()->findChildren<ExternalToolsTreeNode*>() : parent->children;
    for (auto node : qAsConst(nodes)) {
        if (node->content == textPattern) {
            return node;
        } else if (!isExactMatch && node->content.contains(textPattern)) {
            return node;
        }
    }
    GT_FAIL("External tool node by text not found: " + textPattern, nullptr);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getTabWidget"
QTabWidget* GTUtilsDashboard::getTabWidget() {
    return GTWidget::findTabWidget("WorkflowTabView", GTUtilsMdi::activeWindow());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findLoadSchemaButton"
QToolButton* GTUtilsDashboard::findLoadSchemaButton() {
    Dashboard* dashboard = findDashboard();
    return dashboard == nullptr ? nullptr : dashboard->findChild<QToolButton*>("loadSchemaButton");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDashboardName"
QString GTUtilsDashboard::getDashboardName(int dashboardNumber) {
    return GTTabWidget::getTabName(getTabWidget(), dashboardNumber);
}
#undef GT_METHOD_NAME

static QStringList getFileButtonLabels(QWidget* parentWidget) {
    QList<QToolButton*> buttons = parentWidget->findChildren<QToolButton*>();
    QStringList labels;
    for (auto button : qAsConst(buttons)) {
        labels << button->text();
    }
    return labels;
}

#define GT_METHOD_NAME "getInputFiles"
QStringList GTUtilsDashboard::getInputFiles() {
    openTab(Input);
    auto dashboard = getDashboard();
    auto parametersWidget = GTWidget::findWidget("ParametersDashboardWidget", dashboard);
    return getFileButtonLabels(parametersWidget);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getOutputFiles"
QStringList GTUtilsDashboard::getOutputFiles() {
    auto dashboard = getDashboard();
    auto outputFilesWidget = GTWidget::findWidget("OutputFilesDashboardWidget", dashboard);
    return getFileButtonLabels(outputFilesWidget);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getOutputFileUrls"
QStringList GTUtilsDashboard::getOutputFileUrls() {
    QStringList urls;
    auto dashboard = getDashboard();
    auto outputFilesWidget = GTWidget::findWidget("OutputFilesDashboardWidget", dashboard);
    QList<QToolButton*> buttons = outputFilesWidget->findChildren<QToolButton*>();
    for (auto button : qAsConst(buttons)) {
        QString typeAndUrl = button->property("file-url").toString();
        urls << typeAndUrl.split("\n")[1];
    }
    return urls;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickOutputFile"
void GTUtilsDashboard::clickOutputFile(const QString& outputFileName) {
    auto dashboard = getDashboard();
    auto outputFilesWidget = GTWidget::findWidget("OutputFilesDashboardWidget", dashboard);
    auto button = GTWidget::findButtonByText(outputFileName, outputFilesWidget);
    GTWidget::click(button);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "hasNotifications"
bool GTUtilsDashboard::hasNotifications() {
    openTab(Overview);
    auto dashboard = getDashboard();
    auto notificationsWidget = GTWidget::findWidget("NotificationsDashboardWidget", dashboard);
    return notificationsWidget->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNotificationTypeFromHtml"
QString GTUtilsDashboard::getNotificationTypeFromHtml(const QString& html) {
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
QString GTUtilsDashboard::getNotificationCellText(const QGridLayout& tableLayout, const int row, const int col) {
    const QWidget* cellWidget = tableLayout.itemAtPosition(row, col)->widget();
    QString text;

    if (cellWidget != nullptr && cellWidget->objectName() == "tableCell") {
        if (const QLayout* const cellLayout = cellWidget->layout()) {
            for (int labelInd = 0; labelInd < cellLayout->count(); ++labelInd) {
                if (auto label = qobject_cast<QLabel*>(cellLayout->itemAt(labelInd)->widget())) {
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
QList<GTUtilsDashboard::Notification> GTUtilsDashboard::getNotifications() {
    const QString notificationsWidgetName = "NotificationsDashboardWidget";
    auto notificationsWidget = GTWidget::findWidget(notificationsWidgetName, GTUtilsDashboard::getDashboard());
    auto tableLayout = qobject_cast<QGridLayout*>(notificationsWidget->layout());
    QList<Notification> notifications;

    GT_CHECK_RESULT(tableLayout != nullptr && tableLayout->columnCount() == 3,
                    notificationsWidgetName % " was found, but cannot be used in a test",
                    notifications)

    const int notificationsNumber = tableLayout->rowCount();
    for (int row = 1; row < notificationsNumber; row++) {
        const QString type = getNotificationTypeFromHtml(getNotificationCellText(*tableLayout, row, 0));
        const QString element = getNotificationCellText(*tableLayout, row, 1);
        const QString message = getNotificationCellText(*tableLayout, row, 2);
        notifications << Notification {type, element, message};
    }
    return notifications;
}
#undef GT_METHOD_NAME

QString GTUtilsDashboard::getJoinedNotificationsString() {
    const auto notifications = getNotifications();
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
Dashboard* GTUtilsDashboard::findDashboard() {
    QTabWidget* tabWidget = getTabWidget();
    return tabWidget == nullptr ? nullptr : qobject_cast<Dashboard*>(tabWidget->currentWidget());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDashboard"
Dashboard* GTUtilsDashboard::getDashboard() {
    auto dashboard = findDashboard();
    GT_CHECK_RESULT(dashboard != nullptr, "Dashboard widget not found", nullptr);
    return dashboard;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openTab"
void GTUtilsDashboard::openTab(Tabs tab) {
    QWidget* dashboard = findDashboard();
    GT_CHECK(dashboard != nullptr, "Dashboard widget not found");

    QString tabButtonObjectName = getTabObjectName(tab);
    auto tabButton = GTWidget::findToolButton(tabButtonObjectName, dashboard);

    GTWidget::click(tabButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "hasTab"
bool GTUtilsDashboard::hasTab(Tabs tab) {
    QWidget* dashboard = findDashboard();
    GT_CHECK_RESULT(dashboard != nullptr, "Dashboard is not found", false);

    QString tabButtonObjectName = getTabObjectName(tab);
    QWidget* button = dashboard->findChild<QWidget*>(tabButtonObjectName);
    return button != nullptr && button->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNodeText"
QString GTUtilsDashboard::getNodeText(const QString& nodeId) {
    return getExternalToolNode(nodeId)->content;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getChildrenNodesCount"
int GTUtilsDashboard::getChildrenNodesCount(const QString& nodeId) {
    return nodeId == TREE_ROOT_ID ? getExternalToolsWidget()->getTopLevelNodes().size() : getExternalToolNode(nodeId)->children.count();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getChildNodes"
QList<ExternalToolsTreeNode*> GTUtilsDashboard::getChildNodes(const QString& nodeId) {
    return nodeId == TREE_ROOT_ID ? getExternalToolsWidget()->getTopLevelNodes() : getExternalToolNode(nodeId)->children;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getChildNodeId"
QString GTUtilsDashboard::getChildNodeId(const QString& nodeId, int childIndex) {
    return getDescendantNodeId(nodeId, {childIndex});
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDescendantNodeId"
QString GTUtilsDashboard::getDescendantNodeId(const QString& nodeId, const QList<int>& childIndexes) {
    QList<ExternalToolsTreeNode*> childNodes = getChildNodes(nodeId);
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
QString GTUtilsDashboard::getChildWithTextId(const QString& nodeId, const QString& text) {
    int childrenCount = getChildrenNodesCount(nodeId);
    QString resultChildId;
    QStringList quotedChildrenTexts;
    for (int i = 0; i < childrenCount; i++) {
        const QString currentChildId = getChildNodeId(nodeId, i);
        const QString childText = getNodeText(currentChildId);
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
bool GTUtilsDashboard::hasLimitationMessage(const QString& nodeId) {
    return !getLimitationMessage(nodeId).isEmpty();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLimitationMessage"
QString GTUtilsDashboard::getLimitationMessage(const QString& nodeId) {
    return nodeId == TREE_ROOT_ID ? getExternalToolsWidget()->getLimitationWarningHtml() : getExternalToolNode(nodeId)->limitationWarningHtml;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "parseUrlFromContent"
static QString parseUrlFromContent(const QString& content) {
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
QString GTUtilsDashboard::getLogUrlFromNodeLimitationMessage(const QString& nodeId) {
    QString limitationMessage = getLimitationMessage(nodeId);
    return parseUrlFromContent(limitationMessage);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLogUrlFromOutputContent"
QString GTUtilsDashboard::getLogUrlFromOutputContent(const QString& outputNodeId) {
    auto content = getExternalToolNode(outputNodeId)->content;
    return parseUrlFromContent(content);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getCopyButtonSize"
QSize GTUtilsDashboard::getCopyButtonSize(const QString& toolRunNodeId) {
    return getCopyButton(toolRunNodeId)->rect().size();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickCopyButton"
void GTUtilsDashboard::clickCopyButton(const QString& toolRunNodeId) {
    GTWidget::click(getCopyButton(toolRunNodeId));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isNodeVisible"
bool GTUtilsDashboard::isNodeVisible(const QString& nodeId) {
    return getExternalToolNode(nodeId)->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isNodeCollapsed"
bool GTUtilsDashboard::isNodeCollapsed(const QString& nodeId) {
    return !getExternalToolNode(nodeId)->isExpanded();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "collapseNode"
void GTUtilsDashboard::collapseNode(const QString& nodeId) {
    GT_CHECK(isNodeVisible(nodeId), QString("Node with ID '%1' is not visible. Some of the parent nodes are collapsed?").arg(nodeId));
    GT_CHECK(!isNodeCollapsed(nodeId), QString("Node with ID '%1' is already collapsed.").arg(nodeId));
    clickNodeTitle(getExternalToolNode(nodeId));
    GT_CHECK(isNodeCollapsed(nodeId), QString("Node with ID '%1' was not collapsed.").arg(nodeId));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "expandNode"
void GTUtilsDashboard::expandNode(const QString& nodeId) {
    GT_CHECK(isNodeVisible(nodeId), QString("Node with ID '%1' is not visible. Some of the parent nodes are collapsed?").arg(nodeId));
    GT_CHECK(isNodeCollapsed(nodeId), QString("Node with ID '%1' is already expanded.").arg(nodeId));
    clickNodeTitle(getExternalToolNode(nodeId));
    GT_CHECK(!isNodeCollapsed(nodeId), QString("Node with ID '%1' was not expanded.").arg(nodeId));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickNodeTitle"
void GTUtilsDashboard::clickNodeTitle(ExternalToolsTreeNode* node) {
    GT_CHECK(node != nullptr, "Node is null!");
    GT_CHECK(node->badgeLabel->titleLabel != nullptr, "Node title label is null!");
    GTWidget::click(node->badgeLabel->titleLabel);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getInputTabPageWidget"
QWidget* GTUtilsDashboard::getInputTabPageWidget() {
    return GTWidget::findWidget("input_tab_page", getDashboard());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickLabelInParametersWidget"
void GTUtilsDashboard::clickLabelInParametersWidget(const QString& labelText) {
    auto parametersWidget = GTWidget::findWidget("ParametersDashboardWidget", getInputTabPageWidget());
    auto workerLabelWidgets = GTWidget::findLabelByText(labelText, parametersWidget);
    GT_CHECK(workerLabelWidgets.size() == 1, QString("clickLabelInParametersWidget: labels count: %1, label text: %2").arg(workerLabelWidgets.size()).arg(labelText));
    GTWidget::click(workerLabelWidgets[0]);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickFileButtonInParametersWidget"
void GTUtilsDashboard::clickFileButtonInParametersWidget(const QString& buttonText) {
    auto parametersWidget = GTWidget::findWidget("ParametersDashboardWidget", getInputTabPageWidget());
    auto fileButton = GTWidget::findButtonByText(buttonText, parametersWidget);
    GTWidget::click(fileButton);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
