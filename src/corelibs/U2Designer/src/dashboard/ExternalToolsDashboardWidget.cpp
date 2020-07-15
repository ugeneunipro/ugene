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

#include "ExternalToolsDashboardWidget.h"

#include <QApplication>
#include <QClipboard>
#include <QHBoxLayout>
#include <QPainter>
#include <QScrollArea>

#include <U2Core/U2SafePoints.h>

#include <U2Gui/HoverQLabel.h>

#include "DomUtils.h"

namespace U2 {

#define NODE_KIND_ACTOR 1
#define NODE_KIND_TOOL 2
#define NODE_KIND_RUN 3
#define NODE_KIND_COMMAND 4
#define NODE_KIND_OUTPUT 5
#define NODE_KIND_LOG_CONTENT 6

#define NODE_CLASS_ACTOR QString("actor-node")
#define NODE_CLASS_TOOL QString("actor-tick-node")
#define NODE_CLASS_RUN QString("tool-run-node")
#define NODE_CLASS_COMMAND QString("command")
#define NODE_CLASS_CONTENT QString("content")
#define NODE_CLASS_IMPORTANT QString("badge-important")
#define NODE_CLASS_BADGE QString("badge")

const QString ExternalToolsDashboardWidget::TREE_ID("treeRoot");

static QString fixOldStyleOpenFileJs(const QString &html) {
    return QString(html).replace("onclick=\"openLog('", "href=\"file://").replace("/logs')", "/logs").replace("_log.txt')", "_log.txt");
}

ExternalToolsDashboardWidget::ExternalToolsDashboardWidget(const QDomElement &dom, const WorkflowMonitor *monitor)
    : monitor(monitor) {
    setMinimumWidth(1100);    // TODO: make it expanding.
    layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);

    QList<QDomElement> actorElementList = DomUtils::findChildElementsByClass(DomUtils::findElementById(dom, TREE_ID), NODE_CLASS_ACTOR, 2);
    for (auto actorSpan : actorElementList) {
        auto actorNode = new ExternalToolsTreeNode(NODE_KIND_ACTOR, actorSpan.attribute("id"), actorSpan.text(), nullptr);
        layout->addWidget(actorNode);
        topLevelNodes << actorNode;

        QList<QDomElement> toolElementList = DomUtils::findChildElementsByClass(actorSpan.nextSiblingElement("ul"), NODE_CLASS_TOOL, 2);
        for (auto toolSpan : toolElementList) {
            auto toolNode = new ExternalToolsTreeNode(NODE_KIND_TOOL, toolSpan.attribute("id"), toolSpan.text(), actorNode);
            layout->addWidget(toolNode);
            addLimitationWarningIfNeeded(toolNode, DomUtils::findParentByTag(toolSpan, "ul"));

            QList<QDomElement> runElementList = DomUtils::findChildElementsByClass(toolSpan.nextSiblingElement("ul"), NODE_CLASS_RUN, 2);
            for (auto runSpan : runElementList) {
                auto runNode = new ExternalToolsTreeNode(NODE_KIND_RUN, runSpan.attribute("id"), runSpan.text(), toolNode, DomUtils::hasClass(runSpan, NODE_CLASS_IMPORTANT));
                layout->addWidget(runNode);
                addLimitationWarningIfNeeded(runNode, DomUtils::findParentByTag(runSpan, "ul"));

                QDomElement commandSpan = runSpan.nextSiblingElement("ul").firstChildElement("li").firstChildElement("span");
                if (!commandSpan.isNull()) {
                    auto commandNode = new ExternalToolsTreeNode(NODE_KIND_COMMAND, commandSpan.attribute("id"), commandSpan.text(), runNode);
                    layout->addWidget(commandNode);

                    QDomElement commandContentSpan = commandSpan.nextSiblingElement("ul").firstChildElement("li").firstChildElement("span");
                    if (!commandContentSpan.isNull()) {
                        auto commandContentNode = new ExternalToolsTreeNode(NODE_KIND_LOG_CONTENT, commandContentSpan.attribute("id"), commandContentSpan.text(), commandNode);
                        layout->addWidget(commandContentNode);
                    }

                    QDomNode outputLi = commandSpan.parentNode();    // previous node for the real outputLi.
                    while (true) {
                        outputLi = outputLi.nextSiblingElement("li");
                        QDomElement outputSpan = outputLi.firstChildElement("span");
                        if (outputLi.isNull() || outputSpan.isNull()) {
                            break;
                        }
                        auto outputNode = new ExternalToolsTreeNode(NODE_KIND_OUTPUT, outputSpan.attribute("id"), outputSpan.text(), runNode, DomUtils::hasClass(outputSpan, NODE_CLASS_IMPORTANT));
                        layout->addWidget(outputNode);

                        QDomElement outputContentSpan = outputSpan.nextSiblingElement("ul").firstChildElement("li").firstChildElement("span");
                        if (!outputContentSpan.isNull()) {
                            QString outputHtml = fixOldStyleOpenFileJs(DomUtils::toString(outputContentSpan, false));
                            layout->addWidget(new ExternalToolsTreeNode(NODE_KIND_LOG_CONTENT, outputContentSpan.attribute("id"), outputHtml, outputNode));
                        }
                    }
                }
            }
        }
    }
    if (!actorElementList.isEmpty()) {
        addLimitationWarningIfNeeded(nullptr, DomUtils::findParentByTag(actorElementList.first(), "ul"));
    }
}

bool ExternalToolsDashboardWidget::isValidDom(const QDomElement &dom) {
    return !DomUtils::findElementById(dom, TREE_ID).isNull();
}

void ExternalToolsDashboardWidget::addLimitationWarningIfNeeded(ExternalToolsTreeNode *parentNode, const QDomElement &listHeadElement) {
    QDomElement span = listHeadElement.lastChildElement("li").firstChildElement("span");
    if (!DomUtils::hasClass(span, "limitation-message")) {
        return;
    }
    QString text = fixOldStyleOpenFileJs(DomUtils::toString(span, false));
    addLimitationWarning(parentNode, text);
}

void ExternalToolsDashboardWidget::addLimitationWarning(ExternalToolsTreeNode *parentNode, const QString &limitationMessage) {
    QString message = limitationMessage;
    if (message.isEmpty()) {
        SAFE_POINT(monitor != nullptr, "WorkflowMonitor is null!", );
        message = "Messages limit on the dashboard exceeded. See <a href=\"" + monitor->getLogsDir() + "\">log files</a>.";
    }
    auto limitationLabel = new QLabel("<code>" + message + "</code>");
    limitationLabel->setStyleSheet("font-size: 16px; background-color: #F0F0F0; color: black; padding: 5px;");
    limitationLabel->setOpenExternalLinks(true);
    if (parentNode == nullptr) {
        if (!limitationWarningHtml.isEmpty()) {
            return;
        }
        layout->addSpacing(20);
        layout->addWidget(limitationLabel);
        limitationWarningHtml = message;
    } else {
        if (!parentNode->limitationWarningHtml.isEmpty()) {
            return;
        }
        parentNode->limitationWarningHtml = message;
        int lastChildIndex = parentNode->children.isEmpty() ? 0 : layout->indexOf(parentNode->children.last());
        layout->insertSpacing(lastChildIndex, 20);
        layout->insertWidget(lastChildIndex, limitationLabel);
    }
}

QString ExternalToolsDashboardWidget::toHtml() const {
    CHECK(!topLevelNodes.isEmpty(), "");
    QString html = "<ul id=\"" + TREE_ID + "\">";
    for (auto node : topLevelNodes) {
        html += node->toHtml();
    }
    if (!limitationWarningHtml.isEmpty()) {
        html += "<li><span class=\"badge limitation-message\">" + limitationWarningHtml + "</span></li>";
    }
    html += "</ul>";
    return html;
}

static ExternalToolsTreeNode *findNode(const QList<ExternalToolsTreeNode *> &nodeList, const QString &objectName) {
    for (auto node : nodeList) {
        if (node->objectName() == objectName) {
            return node;
        }
    }
    return nullptr;
}

#define MAX_SAME_LEVEL_NODES 100
#define MAX_OUTPUT_CONTENT_SIZE 100000

void ExternalToolsDashboardWidget::addLogEntry(const Monitor::LogEntry &entry) {
    SAFE_POINT(monitor != nullptr, "WorkflowMonitor instance is null!", );
    QString newLine = QString(entry.lastLine)
                          .replace("<", "&lt;")
                          .replace(">", "&gt;")
                          .replace("\n", "<br/>")
                          .replace("\r", "");

    QString actorNodeId = "actor_" + entry.actorId;
    ExternalToolsTreeNode *actorNode = findNode(topLevelNodes, actorNodeId);
    if (actorNode == nullptr) {
        if (topLevelNodes.size() >= MAX_SAME_LEVEL_NODES) {
            addLimitationWarning();
            return;
        }
        actorNode = addNodeToLayout(new ExternalToolsTreeNode(NODE_KIND_ACTOR, actorNodeId, entry.actorName, nullptr));
        topLevelNodes << actorNode;
    }

    QString toolNodeId = actorNodeId + "_run_" + QString::number(entry.actorRunNumber);
    ExternalToolsTreeNode *toolNode = findNode(actorNode->children, toolNodeId);
    if (toolNode == nullptr) {
        if (actorNode->children.size() > MAX_SAME_LEVEL_NODES) {
            addLimitationWarning(actorNode);
            return;
        }
        QString toolNodeText = entry.actorName + " run " + QString::number(entry.actorRunNumber);
        toolNode = addNodeToLayout(new ExternalToolsTreeNode(NODE_KIND_TOOL, toolNodeId, toolNodeText, actorNode));
    }

    bool isImportant = entry.contentType == 0;
    QString runNodeId = toolNodeId + "_tool_" + entry.toolName + "_run_" + QString::number(entry.toolRunNumber);
    ExternalToolsTreeNode *runNode = findNode(toolNode->children, runNodeId);
    if (runNode == nullptr) {
        if (toolNode->children.size() > MAX_SAME_LEVEL_NODES) {
            addLimitationWarning(toolNode);
            return;
        }
        QString runNodeText = entry.toolName + " run" + (entry.toolRunNumber > 1 ? " " + QString::number(entry.toolRunNumber) : "");
        runNode = addNodeToLayout(new ExternalToolsTreeNode(NODE_KIND_RUN, runNodeId, runNodeText, toolNode, isImportant));
    } else if (!runNode->isImportant && isImportant) {
        runNode->isImportant = true;
        runNode->badgeLabel->switchToImportantStyle();
    }

    QString outputNodeId = toolNodeId + (entry.contentType == 0 ? "_stderr" : (entry.contentType == 1 ? "_stdout" : "_command"));
    int outputNodeKind = entry.contentType == 2 ? NODE_KIND_COMMAND : NODE_KIND_OUTPUT;
    ExternalToolsTreeNode *outputNode = findNode(runNode->children, outputNodeId);
    QString outputNodeText = entry.contentType == 0 ? "Output log (stderr)" : (entry.contentType == 1 ? "Output log (stdout)" : "Command");
    if (outputNode == nullptr) {
        outputNode = addNodeToLayout(new ExternalToolsTreeNode(outputNodeKind, outputNodeId, outputNodeText, runNode, isImportant));
    }

    QString outputContentNodeId = outputNodeId + "_content";
    ExternalToolsTreeNode *outputContentNode = findNode(outputNode->children, outputContentNodeId);
    if (outputContentNode == nullptr) {
        outputContentNode = addNodeToLayout(new ExternalToolsTreeNode(NODE_KIND_LOG_CONTENT, outputContentNodeId, "", outputNode));
    }

    if (!outputContentNode->isLogFull) {
        QString logLine = newLine;
        if (outputContentNode->content.length() + newLine.length() > MAX_OUTPUT_CONTENT_SIZE) {
            outputContentNode->isLogFull = true;
            QString logUrl = monitor->getLogUrl(entry.actorId, entry.actorRunNumber, entry.toolName, entry.toolRunNumber, entry.contentType);
            logLine = "<br/><br/>The external tool output is too large and can't be visualized on the dashboard. Find full output in <a href=\"" + logUrl + "\">log file</a>.";
        }
        outputContentNode->content.append(logLine);
        outputContentNode->badgeLabel->logView->setHtml("<code>" + outputContentNode->content + "</code>");
    }
}

ExternalToolsTreeNode *ExternalToolsDashboardWidget::addNodeToLayout(ExternalToolsTreeNode *node) {
    if (node->parent == nullptr) {
        layout->addWidget(node);
    } else {
        auto prevNode = node->parent->getLastChildInHierarchyOrSelf();
        int prevIndex = layout->indexOf(prevNode);
        layout->insertWidget(prevIndex, node);
    }
    return node;
}

#define TREE_NODE_X_OFFSET 50

static int getLevelByNodeKind(int kind) {
    switch (kind) {
    case NODE_KIND_ACTOR:
        return 0;
    case NODE_KIND_TOOL:
        return 1;
    case NODE_KIND_RUN:
        return 2;
    case NODE_KIND_COMMAND:
    case NODE_KIND_OUTPUT:
        return 3;
    case NODE_KIND_LOG_CONTENT:
        return 4;
    }
    SAFE_POINT(false, "Unknown kind: " + QString::number(kind), 0);
}

ExternalToolsTreeNode::ExternalToolsTreeNode(int kind, const QString &objectName, const QString &content, ExternalToolsTreeNode *parent, bool isImportant)
    : kind(kind), parent(parent), content(content), isImportant(isImportant), isLogFull(false), badgeLabel(nullptr) {
    Q_ASSERT(!objectName.isEmpty());
    setObjectName(objectName);
    if (parent != nullptr) {
        parent->children << this;
    }
    setContentsMargins(0, 5, 0, 5);
    auto layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);

    int level = getLevelByNodeKind(kind);
    layout->addSpacing(level * TREE_NODE_X_OFFSET);
    badgeLabel = new BadgeLabel(kind, content, isImportant);
    layout->addWidget(badgeLabel);

    if (badgeLabel->titleLabel) {
        connect(badgeLabel->titleLabel, SIGNAL(clicked()), SLOT(sl_toggle()));
    }

    if (badgeLabel->copyButton && kind == NODE_KIND_RUN) {
        connect(badgeLabel->copyButton, SIGNAL(clicked()), SLOT(sl_copyRunCommand()));
    }

    setVisible(level <= getLevelByNodeKind(NODE_KIND_RUN));
}

bool ExternalToolsTreeNode::isExpanded() const {
    return children.size() > 0 && children.first()->isVisible();
}

ExternalToolsTreeNode *ExternalToolsTreeNode::getLastChildInHierarchyOrSelf() {
    return children.isEmpty() ? this : children.last()->getLastChildInHierarchyOrSelf();
}

void ExternalToolsTreeNode::sl_toggle() {
    bool isExpandedBefore = isExpanded();
    bool isExpandedAfter = !isExpandedBefore;

    // Auto-expand command & output nodes when RUN node is clicked.
    bool expandAllChildren = isExpandedAfter && getLevelByNodeKind(kind) >= getLevelByNodeKind(NODE_KIND_RUN);

    for (auto child : children) {
        child->updateExpandCollapseState(isExpandedAfter, expandAllChildren);
    }
}

void ExternalToolsTreeNode::updateExpandCollapseState(bool isParentExpanded, bool isApplyToAllLevelOfChildren) {
    this->setVisible(isParentExpanded);
    if (!isParentExpanded) {    // make all children invisible (we use flat VBOX layout model for the tree, so parent must hide children manually).
        for (auto child : children) {
            child->updateExpandCollapseState(false);
        }
    } else if (isApplyToAllLevelOfChildren) {    // make children on all levels visible.
        for (auto child : children) {
            child->updateExpandCollapseState(true, true);
        }
    }
}

void ExternalToolsTreeNode::sl_copyRunCommand() {
    if (kind == NODE_KIND_RUN && !children.isEmpty() && !children[0]->children.isEmpty()) {
        QApplication::clipboard()->setText(children[0]->children[0]->content);
    }
}

static bool isLastChild(const ExternalToolsTreeNode *node) {
    return node != nullptr && node->parent != nullptr && node->parent->children.last() == node;
}

#define BRANCH_X_PADDING 15

void ExternalToolsTreeNode::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    if (kind == NODE_KIND_ACTOR || width() == 0 || height() == 0) {
        return;
    }

    QPainter painter(this);
    painter.setPen(QPen(QBrush(QColor("#999999")), 1));

    for (const ExternalToolsTreeNode *node = this; node != nullptr && node->kind != NODE_KIND_ACTOR; node = node->parent) {
        int level = getLevelByNodeKind(node->kind);
        int x = (level - 1) * TREE_NODE_X_OFFSET + BRANCH_X_PADDING;
        if (node == this) {
            painter.drawLine(x, 0, x, isLastChild(node) ? height() / 2 : height());
            int horizontalLineY = height() / 2;
            painter.drawLine(x, horizontalLineY, x + TREE_NODE_X_OFFSET - 5, horizontalLineY);
        } else if (!isLastChild(node)) {
            painter.drawLine(x, 0, x, height());
        }
    }
}

QString ExternalToolsTreeNode::getSpanClass() const {
    QString result = NODE_CLASS_BADGE + (isImportant ? " " + NODE_CLASS_IMPORTANT : "");
    switch (kind) {
    case NODE_KIND_ACTOR:
        return result + " " + NODE_CLASS_ACTOR;
    case NODE_KIND_TOOL:
        return result + " " + NODE_CLASS_TOOL;
    case NODE_KIND_RUN:
        return result + " " + NODE_CLASS_RUN;
    case NODE_KIND_LOG_CONTENT:
        return result + " " + NODE_CLASS_CONTENT;
    }
    return result;
}

QString ExternalToolsTreeNode::toHtml() const {
    QString html = "<li>\n";
    html += "<span id=\"" + objectName() + "\" class=\"" + getSpanClass() + "\">" + content + "</span>\n";
    if (!children.isEmpty()) {
        html += "<ul>\n";
        for (auto child : children) {
            html += child->toHtml();
        }
        html += "</ul>\n";
    }
    if (!limitationWarningHtml.isEmpty()) {
        html += "<li><span class=\"badge limitation-message\">" + limitationWarningHtml + "</span></li>\n";
    }
    html += "</li>\n";
    return html;
}

#define RUN_NODE_NORMAL_COLOR "#50A976"
#define RUN_NODE_IMPORTANT_COLOR "#CC6666"

static QString getBadgeLabelStyle(int kind, bool isImportant) {
    QString style = "border-radius: 6px; padding: 2px 4px; color: white;";
    switch (kind) {
    case NODE_KIND_ACTOR:
        return style + "background-color: #92939E;";
    case NODE_KIND_TOOL:
        return style + "background-color: #bdb0a0;";
    case NODE_KIND_RUN:
        return style + QString("background-color: ") + (isImportant ? RUN_NODE_IMPORTANT_COLOR : RUN_NODE_NORMAL_COLOR) + ";";
    case NODE_KIND_COMMAND:
        return style + "background-color: #79ACAC;";
    case NODE_KIND_OUTPUT:
        return style + QString("background-color: ") + (isImportant ? RUN_NODE_IMPORTANT_COLOR : "#6699CC") + ";";
    case NODE_KIND_LOG_CONTENT:
        return style + "font-size: 16px; background-color: #F0F0F0; color: black;";
    }
    return style;
};

BadgeLabel::BadgeLabel(int kind, const QString &text, bool isImportant)
    : kind(kind), titleLabel(nullptr), copyButton(nullptr), logView(nullptr) {
    auto layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);

    QString style = getBadgeLabelStyle(kind, isImportant);
    bool isCopyButtonVisible = kind == NODE_KIND_RUN;
    QString finalStyle = isCopyButtonVisible ? style + ";border-top-right-radius: 0; border-bottom-right-radius: 0;" : style;
    if (kind == NODE_KIND_LOG_CONTENT) {
        logView = new QTextBrowser();
        logView->setStyleSheet("QTextBrowser {" + finalStyle + "}");
        logView->setTextInteractionFlags(Qt::TextBrowserInteraction);
        logView->setContextMenuPolicy(Qt::DefaultContextMenu);
        logView->setOpenExternalLinks(true);
        logView->setHtml("<code>" + text + "</code>");
        layout->addWidget(logView);
    } else {
        titleLabel = new HoverQLabel(text, "QLabel {" + finalStyle + "}", "QLabel {" + finalStyle + "; color: black;}");
        layout->addWidget(titleLabel);
    }

    if (isCopyButtonVisible) {
        QString copyButtonStyle = style + ";border-top-left-radius: 0; border-bottom-left-radius: 0; border-left: 1px solid #eee;";
        copyButton = new HoverQLabel("", "QLabel {" + copyButtonStyle + "}", "QLabel {" + copyButtonStyle + "; color: black; background: #777;}");
        copyButton->setPixmap(QPixmap(":U2Designer/images/copy.png"));
        copyButton->setObjectName("copyButton");
        layout->addWidget(copyButton);
    }
    if (kind != NODE_KIND_LOG_CONTENT) {
        layout->addStretch(1);
    }
}

void BadgeLabel::switchToImportantStyle() {
    CHECK(kind == NODE_KIND_RUN, );
    titleLabel->normalStyle = titleLabel->normalStyle.replace(RUN_NODE_NORMAL_COLOR, RUN_NODE_IMPORTANT_COLOR);
    titleLabel->hoveredStyle = titleLabel->hoveredStyle.replace(RUN_NODE_NORMAL_COLOR, RUN_NODE_IMPORTANT_COLOR);
    titleLabel->setStyleSheet(titleLabel->normalStyle);
    copyButton->setStyleSheet(copyButton->styleSheet().replace(RUN_NODE_NORMAL_COLOR, RUN_NODE_IMPORTANT_COLOR));
}

}    // namespace U2
