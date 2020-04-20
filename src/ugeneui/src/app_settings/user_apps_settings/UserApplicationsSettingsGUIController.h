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

#ifndef _U2_USER_APP_SETTINGS_GUI_CONTROLLER_H_
#define _U2_USER_APP_SETTINGS_GUI_CONTROLLER_H_

#include <ui_UserApplicationsSettingsWidget.h>

#include <U2Core/NetworkConfiguration.h>
#include <U2Gui/AppSettingsGUI.h>

#include <QUrl>

namespace U2 {


class UserApplicationsSettingsPageController : public AppSettingsGUIPageController {
    Q_OBJECT
public:
    UserApplicationsSettingsPageController(QObject* p = NULL);

    virtual AppSettingsGUIPageState* getSavedState();

    virtual void saveState(AppSettingsGUIPageState* s);

    virtual AppSettingsGUIPageWidget* createWidget(AppSettingsGUIPageState* data);
    
    QMap<QString, QString> translations;

    const QString& getHelpPageId() const {return helpPageId;};

private:
    static const QString helpPageId;
};


class UserApplicationsSettingsPageState : public AppSettingsGUIPageState {
    Q_OBJECT
public:
    UserApplicationsSettingsPageState()
        : openLastProjectFlag(false), askToSaveProject(0), enableStatistics(false),
        tabbedWindowLayout(false), resetSettings(false), updatesEnabled(true) {}

    QString translFile;
    QString style;
    bool openLastProjectFlag;
    int askToSaveProject;
    bool enableStatistics;
    bool tabbedWindowLayout;
    bool resetSettings;
    bool updatesEnabled;
};


class UserApplicationsSettingsPageWidget: public AppSettingsGUIPageWidget, public Ui_UserApplicationsSettingsWidget {
    Q_OBJECT
public:
    UserApplicationsSettingsPageWidget(UserApplicationsSettingsPageController* ctrl);

    virtual void setState(AppSettingsGUIPageState* state);

    virtual AppSettingsGUIPageState* getState(QString& err) const;

private slots:
    void sl_transFileClicked();
};

}//namespace

#endif
