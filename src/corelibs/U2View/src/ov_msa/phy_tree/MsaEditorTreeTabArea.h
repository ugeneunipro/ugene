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

#ifndef _U2_MSA_EDITOR_TREE_TAB_AREA_H_
#define _U2_MSA_EDITOR_TREE_TAB_AREA_H_

#include <QPushButton>
#include <QTabWidget>

namespace U2 {

class MSADistanceMatrix;
class MSAEditor;

class MsaEditorTreeTab : public QTabWidget {
    Q_OBJECT
public:
    MsaEditorTreeTab(MSAEditor* msaEditor, QWidget* parent);

    // TODO: method hides base class method to emit 'si_tabsCountChanged'.
    //  The signal should be emitted from inside of 'virtual void tabInserted(int index)'
    //  Need to re-check all use-cases before update.
    int addTab(QWidget* page, const QString& label);

    void deleteTree(int index);

    QWidget* getCurrentWidget() const {
        return currentWidget();
    }

    void addExistingTree();

public slots:
    void sl_addTabTriggered();

private slots:
    void sl_onTabCloseRequested(int index);
    void sl_onCloseOtherTabs();
    void sl_onCloseAllTabs();
    void sl_onCloseTab();
    void sl_onCountChanged(int count);
    void sl_onContextMenuRequested(const QPoint& pos);
signals:
    void si_tabsCountChanged(int curTabsNumber);

private:
    MSAEditor* editor;
    QPushButton* addTabButton;
    QPoint menuPos;
    QAction* closeOtherTabs;
    QAction* closeAllTabs;
    QAction* closeTab;
};

class MsaEditorTreeTabArea : public QWidget {
    Q_OBJECT
public:
    MsaEditorTreeTabArea(MSAEditor* msaEditor, QWidget* parent);

    /** Adds new tab with the given content and title. Activates the tab if 'activate' is true. */
    void addTab(QWidget* page, const QString& label, bool activate = false);

    MsaEditorTreeTab* getCurrentTabWidget() const {
        return treeTabWidget;
    }

    QWidget* getCurrentWidget() const {
        return treeTabWidget->getCurrentWidget();
    }

protected:
    void paintEvent(QPaintEvent*) override;
    MsaEditorTreeTab* createTabWidget();

signals:
    void si_tabsCountChanged(int curTabsNumber);

    /** Emitted when active tab changed. */
    void si_activeTabChanged(int tabIndex);

private:
    MSAEditor* editor;
    MsaEditorTreeTab* treeTabWidget;
    QLayout* currentLayout;
};
}  // namespace U2

#endif
