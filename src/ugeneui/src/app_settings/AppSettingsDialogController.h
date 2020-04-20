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

#ifndef _U2_APPSETTINGS_DIALOG_CONTROLLER_H_
#define _U2_APPSETTINGS_DIALOG_CONTROLLER_H_

#include <ui_AppSettingsDialog.h>

#include <QTreeWidgetItem>
#include <QTimerEvent>

class AppSettingsDialogTree;

namespace U2 {

class HelpButton;
class AppSettingsGUIPageController;
class AppSettingsGUIPageState;
class AppSettingsGUIPageWidget;
class AppSettingsTreeItem;

class AppSettingsDialogController : public QDialog, public Ui_AppSettingsDialog {
    Q_OBJECT

public:
    AppSettingsDialogController(const QString& pageId = QString(), QWidget *p = NULL);

public slots:
    virtual void accept();
    virtual void reject();
    void sl_currentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);

protected:
    void timerEvent(QTimerEvent* e);

private slots:
    void sl_setLockState(bool);

private:
    bool checkCurrentState(bool saveStateInItem, bool showError);
    bool turnPage(AppSettingsTreeItem* page);
    void registerPage(AppSettingsGUIPageController* page);

    AppSettingsTreeItem* findPageItem(const QString& id) const;

    AppSettingsTreeItem*    currentPage;
    HelpButton*             helpButton;
};

class AppSettingsTreeItem : public QTreeWidgetItem {
public:
    AppSettingsTreeItem(AppSettingsGUIPageController* pageController);
    AppSettingsGUIPageController*   pageController;
    AppSettingsGUIPageState*        pageState;
    AppSettingsGUIPageWidget*       pageWidget;
};

}//namespace

#endif
