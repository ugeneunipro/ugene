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

#ifndef _U2_EXTERNAL_TOOLS_DASHBOARD_WIDGET_H_
#define _U2_EXTERNAL_TOOLS_DASHBOARD_WIDGET_H_

#include <QDomElement>
#include <QLabel>
#include <QTextBrowser>
#include <QVBoxLayout>

#include <U2Lang/WorkflowMonitor.h>

namespace U2 {

class BadgeLabel;
class HoverQLabel;
class ExternalToolsTreeNode;

class U2DESIGNER_EXPORT ExternalToolsDashboardWidget : public QWidget {
    Q_OBJECT
public:
    ExternalToolsDashboardWidget(const QDomElement &dom, const WorkflowMonitor *monitor = nullptr);

    QString toHtml() const;

    void addLogEntry(const Monitor::LogEntry &entry);

    const QList<ExternalToolsTreeNode *> getTopLevelNodes() const {
        return topLevelNodes;
    }
    const QString &getLimitationWarningHtml() const {
        return limitationWarningHtml;
    }

    /** Object name and DOM element it for external tools tree. */
    static const QString TREE_ID;

    static bool isValidDom(const QDomElement& dom);

private:
    void addLimitationWarning(ExternalToolsTreeNode *parentNode = nullptr, const QString &limitationMessage = "");

    void addLimitationWarningIfNeeded(ExternalToolsTreeNode *parentNode, const QDomElement &listHeadElement);

    ExternalToolsTreeNode *addNodeToLayout(ExternalToolsTreeNode *node);

    const WorkflowMonitor *monitor;
    QVBoxLayout *layout;
    QList<ExternalToolsTreeNode *> topLevelNodes;
    QString limitationWarningHtml;
};

class U2DESIGNER_EXPORT ExternalToolsTreeNode : public QWidget {
    Q_OBJECT
public:
    ExternalToolsTreeNode(int kind, const QString &objectName, const QString &content, ExternalToolsTreeNode *parent, bool isImportant = false);

    void paintEvent(QPaintEvent *event);

    QString toHtml() const;

    QString getSpanClass() const;

    bool isExpanded() const;

    const int kind;

    ExternalToolsTreeNode *parent;

    QList<ExternalToolsTreeNode *> children;

    QString content;

    bool isImportant;

    bool isLogFull;

    BadgeLabel *badgeLabel;

    QString limitationWarningHtml;

    ExternalToolsTreeNode *getLastChildInHierarchyOrSelf();

public slots:
    void sl_toggle();
    void sl_copyRunCommand();

private:
    void updateExpandCollapseState(bool isParentExpanded, bool isApplyToAllLevelOfChildren = false);
};

class U2DESIGNER_EXPORT BadgeLabel : public QWidget {
    Q_OBJECT
public:
    BadgeLabel(int kind, const QString &text, bool isImportant);

    void switchToImportantStyle();

    const int kind;
    HoverQLabel *titleLabel;
    HoverQLabel *copyButton;
    QTextBrowser *logView;
};

}    // namespace U2
#endif
