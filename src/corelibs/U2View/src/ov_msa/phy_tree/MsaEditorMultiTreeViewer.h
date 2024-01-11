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

#include <QStringList>
#include <QToolBar>
#include <QWidget>

#include <U2Core/global.h>

namespace U2 {

class MsaEditor;
class MsaEditorTreeTabArea;
class GObjectViewWindow;
class MsaEditorTreeTab;

class U2VIEW_EXPORT MsaEditorMultiTreeViewer : public QWidget {
    Q_OBJECT
public:
    MsaEditorMultiTreeViewer(const QString& title, MsaEditor* msaEditor);

    void addTreeView(GObjectViewWindow* treeView);

    QWidget* getCurrentWidget() const;

    MsaEditorTreeTab* getCurrentTabWidget() const;

    const QStringList& getTreeNames() const;

signals:
    void si_tabsCountChanged(int tabsCount);

    /** Emitted when active tree tab changes. */
    void si_activeTreeViewChanged();

public slots:
    void sl_onTabCloseRequested(QWidget*);

private:
    MsaEditorTreeTabArea* treeTabArea = nullptr;
    QWidget* titleWidget = nullptr;
    MsaEditor* editor = nullptr;
    QList<QWidget*> treeViewList;
    QStringList tabsNameList;
};

}  // namespace U2
