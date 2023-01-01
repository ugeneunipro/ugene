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

#ifndef _U2_USER_APP_SETTINGS_GUI_CONTROLLER_H_
#define _U2_USER_APP_SETTINGS_GUI_CONTROLLER_H_

#include <QUrl>

#include <U2Core/NetworkConfiguration.h>

#include <U2Gui/AppSettingsGUI.h>

#include <ui_UserApplicationsSettingsWidget.h>

namespace U2 {

class UserApplicationsSettingsPageController : public AppSettingsGUIPageController {
    Q_OBJECT
public:
    UserApplicationsSettingsPageController(QObject* p = nullptr);

    AppSettingsGUIPageState* getSavedState() override;

    void saveState(AppSettingsGUIPageState* s) override;

    AppSettingsGUIPageWidget* createWidget(AppSettingsGUIPageState* data) override;

    QMap<QString, QString> translations;

    const QString& getHelpPageId() const override {
        return helpPageId;
    };

private:
    static const QString helpPageId;
};

class UserApplicationsSettingsPageState : public AppSettingsGUIPageState {
    Q_OBJECT
public:
    UserApplicationsSettingsPageState() = default;

    QString translFile;
    QString style;
    bool openLastProjectFlag= false;
    int askToSaveProject= 0;
    bool enableStatistics = false;
    bool tabbedWindowLayout = false;
    bool resetSettings = false;
    bool updatesEnabled = true;
    bool experimentsEnabled = false;
};

class UserApplicationsSettingsPageWidget : public AppSettingsGUIPageWidget, public Ui_UserApplicationsSettingsWidget {
    Q_OBJECT
public:
    UserApplicationsSettingsPageWidget(UserApplicationsSettingsPageController* ctrl);

    void setState(AppSettingsGUIPageState* state) override;

    AppSettingsGUIPageState* getState(QString& err) const override;

private slots:
    void sl_transFileClicked();

private:
    /**
     * @brief STYLE_KEYS_FIXED_REGISTER
     * The bunch of "Appearance" with fixed register.
     * The first value - the original name, produced by QStyleFactory::keys(), the second value - new name with fixed register.
     * Note, that you can't replace any characters, only register could be changed
     * (see here, https://doc.qt.io/qt-5/qstylefactory.html#details, "keys are case insensitive")
     */
    static const QMap<QString, QString> FIXED_CASE_QSTYLE_KEY_MAP;
};

}  // namespace U2

#endif
