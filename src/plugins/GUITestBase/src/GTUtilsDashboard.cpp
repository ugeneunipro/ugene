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

#include <QRegularExpression>
#include <QTabWidget>

#include <GTUtilsMdi.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTWebView.h>
#include <primitives/GTWidget.h>

#include "GTUtilsDashboard.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsDashboard"
QMap<QString, GTUtilsDashboard::Tabs> GTUtilsDashboard::initTabMap(){
    QMap<QString, GTUtilsDashboard::Tabs> result;
    result.insert("Overview", GTUtilsDashboard::Overview);
    result.insert("Input", GTUtilsDashboard::Input);
    result.insert("External Tools", GTUtilsDashboard::ExternalTools);
    return result;
}

QString GTUtilsDashboard::getNodeSpanId(const QString &nodeId) {
    // It is defined in ExternalToolsWidget.js.
    return nodeId + "_span";
}

HIWebElement GTUtilsDashboard::getCopyButton(GUITestOpStatus &os, const QString &toolRunNodeId) {
    const QString selector = QString("SPAN#%1 > BUTTON").arg(getNodeSpanId(toolRunNodeId));

    GTGlobals::FindOptions options;
    options.searchInHidden = true;

    return GTWebView::findElementBySelector(os, getDashboard(os), selector, options);
}

HIWebElement GTUtilsDashboard::getNodeSpan(GUITestOpStatus &os, const QString &nodeId) {
    const QString selector = QString("SPAN#%1").arg(getNodeSpanId(nodeId));

    GTGlobals::FindOptions options;
    options.searchInHidden = true;

    return GTWebView::findElementBySelector(os, getDashboard(os), selector, options);
}

HIWebElement GTUtilsDashboard::getNodeUl(GUITestOpStatus &os, const QString &nodeId) {
    const QString selector = QString("UL#%1").arg(nodeId);

    GTGlobals::FindOptions options;
    options.searchInHidden = true;

    return GTWebView::findElementBySelector(os, getDashboard(os), selector, options);
}

#define GT_METHOD_NAME "clickOutputFile"
QString GTUtilsDashboard::getLogUrlFromElement(GUITestOpStatus &os, const HIWebElement &element) {
    Q_UNUSED(os);
        const QString onclickFunction = element.attribute(ON_CLICK);
    QRegularExpression urlFetcher("openLog\\(\\\'(.*)\\\'\\)");
    const QRegularExpressionMatch match = urlFetcher.match(onclickFunction);
    GT_CHECK_RESULT(match.hasMatch(),
                    QString("Can't get URL with a regexp from an element: regexp is '%1', element ID is '%2', element class is '%3'")
                    .arg(urlFetcher.pattern()).arg(element.id()).arg(element.attribute("class")), QString());
    return match.captured(1);
}
#undef GT_METHOD_NAME

const QMap<QString, GTUtilsDashboard::Tabs> GTUtilsDashboard::tabMap = initTabMap();
const QString GTUtilsDashboard::TREE_ROOT_ID = "treeRoot";
const QString GTUtilsDashboard::PARENT_LI = "parent_li";

const QString GTUtilsDashboard::WIDTH = "width";
const QString GTUtilsDashboard::HEIGHT = "height";
const QString GTUtilsDashboard::TITLE = "title";
const QString GTUtilsDashboard::COLLAPSED_NODE_TITLE = "Expand this branch";
const QString GTUtilsDashboard::ON_CLICK = "onclick";

WebView* GTUtilsDashboard::getDashboard(HI::GUITestOpStatus &os) {
    return qobject_cast<WebView *>(getTabWidget(os)->currentWidget());
}

QTabWidget* GTUtilsDashboard::getTabWidget(HI::GUITestOpStatus &os){
    return GTWidget::findExactWidget<QTabWidget *>(os, "WorkflowTabView", GTUtilsMdi::activeWindow(os));
}

const QString GTUtilsDashboard::getDashboardName(GUITestOpStatus &os, int dashboardNumber) {
    return GTTabWidget::getTabName(os, getTabWidget(os), dashboardNumber);
}

QStringList GTUtilsDashboard::getOutputFiles(HI::GUITestOpStatus &os) {
    const QString selector = "div#outputWidget button.btn.full-width.long-text";
    const QList<HIWebElement> outputFilesButtons = GTWebView::findElementsBySelector(os, getDashboard(os), selector, GTGlobals::FindOptions(false));
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
    const QList<HIWebElement> outputFilesButtons = GTWebView::findElementsBySelector(os, getDashboard(os), selector);
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

HIWebElement GTUtilsDashboard::findElement(HI::GUITestOpStatus &os, QString text, QString tag, bool exactMatch){
    return GTWebView::findElement(os, getDashboard(os), text, tag, exactMatch);
}

HIWebElement GTUtilsDashboard::findTreeElement(HI::GUITestOpStatus &os, QString text){
    return GTWebView::findTreeElement(os, getDashboard(os), text);
}

HIWebElement GTUtilsDashboard::findContextMenuElement(HI::GUITestOpStatus &os, QString text){
    return GTWebView::findContextMenuElement(os, getDashboard(os), text);
}

void GTUtilsDashboard::click(HI::GUITestOpStatus &os, HIWebElement el, Qt::MouseButton button){
    GTWebView::click(os, getDashboard(os), el, button);
}

bool GTUtilsDashboard::areThereNotifications(HI::GUITestOpStatus &os) {
    openTab(os, Overview);
    return GTWebView::doesElementExist(os, getDashboard(os), "Notifications", "DIV", true);
}

#define GT_METHOD_NAME "openTab"
void GTUtilsDashboard::openTab(HI::GUITestOpStatus &os, Tabs tab){
    HIWebElement el = GTWebView::findElement(os, getDashboard(os), tabMap.key(tab), "A");
    GTWebView::click(os, getDashboard(os), el);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "doesTabExist"
bool GTUtilsDashboard::doesTabExist(HI::GUITestOpStatus &os, Tabs tab) {
    return GTWebView::doesElementExist(os, getDashboard(os), tabMap.key(tab), "A");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getRootNode"
QString GTUtilsDashboard::getRootNode(GUITestOpStatus &os) {
    return GTWebView::findElementById(os, getDashboard(os), TREE_ROOT_ID).id();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getNodeText"
QString GTUtilsDashboard::getNodeText(GUITestOpStatus &os, const QString &nodeId) {
    return getNodeSpan(os, nodeId).toPlainText();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getChildrenNodesCount"
int GTUtilsDashboard::getChildrenNodesCount(GUITestOpStatus &os, const QString &nodeId) {
    const QString selector = QString("UL#%1 > LI.%2 > UL").arg(nodeId).arg(PARENT_LI);

    GTGlobals::FindOptions options;
    options.failIfNotFound = false;
    options.searchInHidden = true;

    return GTWebView::findElementsBySelector(os, getDashboard(os), selector, options).size();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getChildNodeId"
QString GTUtilsDashboard::getChildNodeId(GUITestOpStatus &os, const QString &nodeId, int childNum) {
    return getDescendantNodeId(os, nodeId, { childNum });
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getDescendantNodeId"
QString GTUtilsDashboard::getDescendantNodeId(GUITestOpStatus &os, const QString &nodeId, const QList<int> &childNums) {
    QString selector = QString("UL#%1").arg(nodeId);
    foreach (const int childNum, childNums) {
        selector += QString(" > LI:nth-of-type(%1) > UL").arg(childNum + 1);
    }

    GTGlobals::FindOptions options;
    options.searchInHidden = true;

    return GTWebView::findElementBySelector(os, getDashboard(os), selector, options).id();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getChildWithTextId"
QString GTUtilsDashboard::getChildWithTextId(GUITestOpStatus &os, const QString &nodeId, const QString &text) {
    const int childrenCount = getChildrenNodesCount(os, nodeId);
    QString resultChildId;
    QStringList quotedChildrenTexts;
    for (int i = 0; i < childrenCount; i++) {
        const QString currentChildId = getChildNodeId(os, nodeId, i);
        const QString childText = getNodeText(os, currentChildId);
        quotedChildrenTexts << "\'" + childText + "\'";
        if (text == childText) {
            GT_CHECK_RESULT(resultChildId.isEmpty(),
                     QString("Expected text '%1' is not unique among the node with ID '%2' children")
                     .arg(text).arg(nodeId), "");
            resultChildId = currentChildId;
        }
    }

    GT_CHECK_RESULT(!resultChildId.isEmpty(),
             QString("Child with text '%1' not found among the node with ID '%2' children; there are children with the following texts: %3")
             .arg(text).arg(nodeId).arg(quotedChildrenTexts.join(", ")), "");

    return resultChildId;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "doesNodeHaveLimitationMessageNode"
bool GTUtilsDashboard::doesNodeHaveLimitationMessageNode(GUITestOpStatus &os, const QString &nodeId) {
    const QString selector = QString("UL#%1 > LI.%2 > SPAN.limitation-message").arg(nodeId).arg(PARENT_LI);

    GTGlobals::FindOptions options;
    options.failIfNotFound = false;

    return !GTWebView::findElementsBySelector(os, getDashboard(os), selector, options).isEmpty();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLimitationMessageNodeText"
QString GTUtilsDashboard::getLimitationMessageNodeText(GUITestOpStatus &os, const QString &nodeId) {
    const QString selector = QString("UL#%1 > LI.%2 > SPAN.limitation-message").arg(nodeId).arg(PARENT_LI);
    return GTWebView::findElementBySelector(os, getDashboard(os), selector).toPlainText();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLimitationMessageLogUrl"
QString GTUtilsDashboard::getLimitationMessageLogUrl(GUITestOpStatus &os, const QString &nodeId) {
    const QString selector = QString("UL#%1 > LI.%2 > SPAN.limitation-message > A").arg(nodeId).arg(PARENT_LI);
    return getLogUrlFromElement(os, GTWebView::findElementBySelector(os, getDashboard(os), selector));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getCopyButtonSize"
QSize GTUtilsDashboard::getCopyButtonSize(GUITestOpStatus &os, const QString &toolRunNodeId) {
    return getCopyButton(os, toolRunNodeId).geometry().size();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickCopyButton"
void GTUtilsDashboard::clickCopyButton(GUITestOpStatus &os, const QString &toolRunNodeId) {
    click(os, getCopyButton(os, toolRunNodeId));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isNodeVisible"
bool GTUtilsDashboard::isNodeVisible(GUITestOpStatus &os, const QString &nodeId) {
    return getNodeSpan(os, nodeId).isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isNodeCollapsed"
bool GTUtilsDashboard::isNodeCollapsed(GUITestOpStatus &os, const QString &nodeId) {
    const HIWebElement nodeSpanElement = getNodeSpan(os, nodeId);
    return nodeSpanElement.attribute(TITLE, "") == COLLAPSED_NODE_TITLE;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "collapseNode"
void GTUtilsDashboard::collapseNode(GUITestOpStatus &os, const QString &nodeId) {
    GT_CHECK(isNodeVisible(os, nodeId),
             QString("SPAN of the node with ID '%1' is not visible. Some of the parent nodes are collapsed?").arg(nodeId));

    GT_CHECK(!isNodeCollapsed(os, nodeId),
             QString("UL of the node with ID '%1' is not visible. It is already collapsed.").arg(nodeId));

    click(os, getNodeSpan(os, nodeId));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "expandNode"
void GTUtilsDashboard::expandNode(GUITestOpStatus &os, const QString &nodeId) {
    GT_CHECK(isNodeVisible(os, nodeId),
             QString("SPAN of the node with ID '%1' is not visible. Some of the parent nodes are collapsed?").arg(nodeId));

    GT_CHECK(isNodeCollapsed(os, nodeId),
             QString("UL of the node with ID '%1' is visible. It is already expanded.").arg(nodeId));

    click(os, getNodeSpan(os, nodeId));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLogUrlFromNode"
QString GTUtilsDashboard::getLogUrlFromNode(GUITestOpStatus &os, const QString &outputNodeId) {
    const QString logFileLinkSelector = QString("SPAN#%1 A").arg(getNodeSpanId(outputNodeId));
    return getLogUrlFromElement(os, GTWebView::findElementBySelector(os, getDashboard(os), logFileLinkSelector));
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}   // namespace U2
