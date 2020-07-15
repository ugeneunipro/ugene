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

#include <GTUtilsMdi.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTWebView.h>
#include <primitives/GTWidget.h>

#include <QRegularExpression>
#include <QTabWidget>

#include <U2Gui/HoverQLabel.h>

#include "GTUtilsDashboard.h"

namespace U2 {
using namespace HI;

const QString GTUtilsDashboard::TREE_ROOT_ID = ExternalToolsDashboardWidget::TREE_ID;

#define GT_CLASS_NAME "GTUtilsDashboard"

#define GT_METHOD_NAME "getCopyButton"
QWidget *GTUtilsDashboard::getCopyButton(GUITestOpStatus &os, const QString &toolRunNodeId) {
    auto node = getExternalToolNode(os, toolRunNodeId);
    return GTWidget::findWidget(os, "copyButton", node);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExternalToolsWidget"
ExternalToolsDashboardWidget *GTUtilsDashboard::getExternalToolsWidget(GUITestOpStatus &os) {
    Dashboard *dashboard = getDashboard(os);
    return GTWidget::findWidgetByType<ExternalToolsDashboardWidget *>(os, dashboard, "External tools widget is not found");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExternalToolNode"
ExternalToolsTreeNode *GTUtilsDashboard::getExternalToolNode(GUITestOpStatus &os, const QString &nodeId) {
    ExternalToolsDashboardWidget *widget = getExternalToolsWidget(os);
    ExternalToolsTreeNode *node = qobject_cast<ExternalToolsTreeNode *>(GTWidget::findWidget(os, nodeId, widget));
    GT_CHECK_RESULT(node != nullptr, "External tool node not found: " + nodeId, nullptr);
    return node;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExternalToolNodeByText"
ExternalToolsTreeNode *GTUtilsDashboard::getExternalToolNodeByText(GUITestOpStatus &os, const QString &textPattern, bool isExactMatch) {
    return getExternalToolNodeByText(os, nullptr, textPattern, isExactMatch);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getExternalToolNodesByText"
QList<ExternalToolsTreeNode *> GTUtilsDashboard::getExternalToolNodesByText(HI::GUITestOpStatus &os, ExternalToolsTreeNode *parent, const QString &textPattern, bool isExactMatch) {
    QList<ExternalToolsTreeNode *> nodes = parent == nullptr ? getExternalToolsWidget(os)->findChildren<ExternalToolsTreeNode *>() : parent->children;
    QList<ExternalToolsTreeNode *> result;
    for (auto node : nodes) {
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
ExternalToolsTreeNode *GTUtilsDashboard::getExternalToolNodeByText(GUITestOpStatus &os, ExternalToolsTreeNode *parent, const QString &textPattern, bool isExactMatch) {
    QList<ExternalToolsTreeNode *> nodes = parent == nullptr ? getExternalToolsWidget(os)->findChildren<ExternalToolsTreeNode *>() : parent->children;
    for (auto node : nodes) {
        if (node->content == textPattern) {
            return node;
        } else if (!isExactMatch && node->content.contains(textPattern)) {
            return node;
        }
    }
    GT_CHECK_RESULT(false, "External tool node by text not found: " + textPattern, nullptr);
}
#undef GT_METHOD_NAME

WebView *GTUtilsDashboard::getDashboardWebView(HI::GUITestOpStatus &os) {
    Dashboard *dashboard = findDashboard(os);
    return dashboard == nullptr ? nullptr : dashboard->getWebView();
}

QTabWidget *GTUtilsDashboard::getTabWidget(HI::GUITestOpStatus &os) {
    return GTWidget::findExactWidget<QTabWidget *>(os, "WorkflowTabView", GTUtilsMdi::activeWindow(os));
}

QToolButton *GTUtilsDashboard::findLoadSchemaButton(HI::GUITestOpStatus &os) {
    Dashboard *dashboard = findDashboard(os);
    return dashboard == nullptr ? nullptr : dashboard->findChild<QToolButton *>("loadSchemaButton");
}

const QString GTUtilsDashboard::getDashboardName(GUITestOpStatus &os, int dashboardNumber) {
    return GTTabWidget::getTabName(os, getTabWidget(os), dashboardNumber);
}

QStringList GTUtilsDashboard::getOutputFiles(HI::GUITestOpStatus &os) {
    QString selector = "div#outputWidget button.btn.full-width.long-text";
    QList<HIWebElement> outputFilesButtons = GTWebView::findElementsBySelector(os, getDashboardWebView(os), selector, GTGlobals::FindOptions(false));
    QStringList outputFilesNames;
    foreach (const HIWebElement &outputFilesButton, outputFilesButtons) {
        const QString outputFileName = outputFilesButton.toPlainText();
        if (!outputFileName.isEmpty()) {
            outputFilesNames << outputFileName;
        }
    }
    return outputFilesNames;
}

#define GT_METHOD_NAME "clickOutputFile"
void GTUtilsDashboard::clickOutputFile(GUITestOpStatus &os, const QString &outputFileName) {
    const QString selector = "div#outputWidget button.btn.full-width.long-text";
    const QList<HIWebElement> outputFilesButtons = GTWebView::findElementsBySelector(os, getDashboardWebView(os), selector);
    foreach (const HIWebElement &outputFilesButton, outputFilesButtons) {
        QString buttonText = outputFilesButton.toPlainText();
        if (buttonText == outputFileName) {
            click(os, outputFilesButton);
            return;
        }

        if (buttonText.endsWith("...")) {
            buttonText.chop(QString("...").length());
            if (!buttonText.isEmpty() && outputFileName.startsWith(buttonText)) {
                click(os, outputFilesButton);
                return;
            }
        }
    }

    GT_CHECK(false, QString("The output file with name '%1' not found").arg(outputFileName));
}
#undef GT_METHOD_NAME

HIWebElement GTUtilsDashboard::findWebElement(HI::GUITestOpStatus &os, QString text, QString tag, bool exactMatch) {
    return GTWebView::findElement(os, getDashboardWebView(os), text, tag, exactMatch);
}

HIWebElement GTUtilsDashboard::findWebContextMenuElement(HI::GUITestOpStatus &os, QString text) {
    return GTWebView::findContextMenuElement(os, getDashboardWebView(os), text);
}

void GTUtilsDashboard::click(HI::GUITestOpStatus &os, HIWebElement el, Qt::MouseButton button) {
    GTWebView::click(os, getDashboardWebView(os), el, button);
}

bool GTUtilsDashboard::areThereNotifications(HI::GUITestOpStatus &os) {
    openTab(os, Overview);
    return GTWebView::doesElementExist(os, getDashboardWebView(os), "Notifications", "DIV", true);
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
Dashboard *GTUtilsDashboard::findDashboard(HI::GUITestOpStatus &os) {
    QTabWidget *tabWidget = getTabWidget(os);
    return tabWidget == nullptr ? nullptr : qobject_cast<Dashboard *>(tabWidget->currentWidget());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDashboard"
Dashboard *GTUtilsDashboard::getDashboard(HI::GUITestOpStatus &os) {
    auto dashboard = findDashboard(os);
    GT_CHECK_RESULT(dashboard != nullptr, "Dashboard widget not found", nullptr);
    return dashboard;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "openTab"
void GTUtilsDashboard::openTab(HI::GUITestOpStatus &os, Tabs tab) {
    QWidget *dashboard = findDashboard(os);
    GT_CHECK(dashboard != nullptr, "Dashboard widget not found");

    QString tabButtonObjectName = getTabObjectName(tab);
    QToolButton *tabButton = GTWidget::findExactWidget<QToolButton *>(os, tabButtonObjectName, dashboard);
    GT_CHECK(tabButton != nullptr, "Tab button not found: " + tabButtonObjectName);

    GTWidget::click(os, tabButton);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "doesTabExist"
bool GTUtilsDashboard::doesTabExist(HI::GUITestOpStatus &os, Tabs tab) {
    QWidget *dashboard = findDashboard(os);
    GT_CHECK_RESULT(dashboard != nullptr, "Dashboard is not found", false);

    QString tabButtonObjectName = getTabObjectName(tab);
    QWidget *button = dashboard->findChild<QWidget *>(tabButtonObjectName);
    return button != nullptr && button->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNodeText"
QString GTUtilsDashboard::getNodeText(GUITestOpStatus &os, const QString &nodeId) {
    return getExternalToolNode(os, nodeId)->content;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getChildrenNodesCount"
int GTUtilsDashboard::getChildrenNodesCount(GUITestOpStatus &os, const QString &nodeId) {
    return nodeId == TREE_ROOT_ID ? getExternalToolsWidget(os)->getTopLevelNodes().size() : getExternalToolNode(os, nodeId)->children.count();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getChildNodes"
QList<ExternalToolsTreeNode *> GTUtilsDashboard::getChildNodes(GUITestOpStatus &os, const QString &nodeId) {
    return nodeId == TREE_ROOT_ID ? getExternalToolsWidget(os)->getTopLevelNodes() : getExternalToolNode(os, nodeId)->children;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getChildNodeId"
QString GTUtilsDashboard::getChildNodeId(GUITestOpStatus &os, const QString &nodeId, int childIndex) {
    return getDescendantNodeId(os, nodeId, {childIndex});
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDescendantNodeId"
QString GTUtilsDashboard::getDescendantNodeId(GUITestOpStatus &os, const QString &nodeId, const QList<int> &childIndexes) {
    QList<ExternalToolsTreeNode *> childNodes = getChildNodes(os, nodeId);
    QString resultNodeId = nodeId;
    for (int i : childIndexes) {
        GT_CHECK_RESULT(i >= 0 && i < childNodes.size(), "Illegal child index: " + QString::number(i) + ", nodes: " + childNodes.size(), "");
        resultNodeId = childNodes[i]->objectName();
        childNodes = childNodes[i]->children;
    }
    return resultNodeId;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getChildWithTextId"
QString GTUtilsDashboard::getChildWithTextId(GUITestOpStatus &os, const QString &nodeId, const QString &text) {
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
bool GTUtilsDashboard::hasLimitationMessage(GUITestOpStatus &os, const QString &nodeId) {
    return !getLimitationMessage(os, nodeId).isEmpty();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLimitationMessage"
QString GTUtilsDashboard::getLimitationMessage(GUITestOpStatus &os, const QString &nodeId) {
    return nodeId == TREE_ROOT_ID ? getExternalToolsWidget(os)->getLimitationWarningHtml() : getExternalToolNode(os, nodeId)->limitationWarningHtml;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "parseUrlFromContent"
static QString parseUrlFromContent(GUITestOpStatus &os, const QString &content) {
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
QString GTUtilsDashboard::getLogUrlFromNodeLimitationMessage(GUITestOpStatus &os, const QString &nodeId) {
    QString limitationMessage = getLimitationMessage(os, nodeId);
    return parseUrlFromContent(os, limitationMessage);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLogUrlFromOutputContent"
QString GTUtilsDashboard::getLogUrlFromOutputContent(GUITestOpStatus &os, const QString &outputNodeId) {
    auto content = getExternalToolNode(os, outputNodeId)->content;
    return parseUrlFromContent(os, content);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getCopyButtonSize"
QSize GTUtilsDashboard::getCopyButtonSize(GUITestOpStatus &os, const QString &toolRunNodeId) {
    return getCopyButton(os, toolRunNodeId)->rect().size();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickCopyButton"
void GTUtilsDashboard::clickCopyButton(GUITestOpStatus &os, const QString &toolRunNodeId) {
    GTWidget::click(os, getCopyButton(os, toolRunNodeId));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isNodeVisible"
bool GTUtilsDashboard::isNodeVisible(GUITestOpStatus &os, const QString &nodeId) {
    return getExternalToolNode(os, nodeId)->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isNodeCollapsed"
bool GTUtilsDashboard::isNodeCollapsed(GUITestOpStatus &os, const QString &nodeId) {
    return !getExternalToolNode(os, nodeId)->isExpanded();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "collapseNode"
void GTUtilsDashboard::collapseNode(GUITestOpStatus &os, const QString &nodeId) {
    GT_CHECK(isNodeVisible(os, nodeId), QString("Node with ID '%1' is not visible. Some of the parent nodes are collapsed?").arg(nodeId));
    GT_CHECK(!isNodeCollapsed(os, nodeId), QString("Node with ID '%1' is already collapsed.").arg(nodeId));
    clickNodeTitle(os, getExternalToolNode(os, nodeId));
    GT_CHECK(isNodeCollapsed(os, nodeId), QString("Node with ID '%1' was not collapsed.").arg(nodeId));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "expandNode"
void GTUtilsDashboard::expandNode(GUITestOpStatus &os, const QString &nodeId) {
    GT_CHECK(isNodeVisible(os, nodeId), QString("Node with ID '%1' is not visible. Some of the parent nodes are collapsed?").arg(nodeId));
    GT_CHECK(isNodeCollapsed(os, nodeId), QString("Node with ID '%1' is already expanded.").arg(nodeId));
    clickNodeTitle(os, getExternalToolNode(os, nodeId));
    GT_CHECK(!isNodeCollapsed(os, nodeId), QString("Node with ID '%1' was not expanded.").arg(nodeId));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickNodeTitle"
void GTUtilsDashboard::clickNodeTitle(GUITestOpStatus &os, ExternalToolsTreeNode *node) {
    GT_CHECK(node != nullptr, "Node is null!");
    GT_CHECK(node->badgeLabel->titleLabel != nullptr, "Node title label is null!");
    GTWidget::click(os, node->badgeLabel->titleLabel);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}    // namespace U2
