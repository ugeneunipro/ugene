/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <QUrl>

#include <U2Core/NetworkConfiguration.h>

#include <U2Gui/AppSettingsGUI.h>

#include <ui_NetworkSettingsWidget.h>

namespace U2 {

class NetworkSettingsPageController : public AppSettingsGUIPageController {
    Q_OBJECT
public:
    NetworkSettingsPageController(QObject* p = nullptr);

    virtual AppSettingsGUIPageState* getSavedState();

    virtual void saveState(AppSettingsGUIPageState* s);

    virtual AppSettingsGUIPageWidget* createWidget(AppSettingsGUIPageState* data);

    const QString& getHelpPageId() const {
        return helpPageId;
    };

private:
    static const QString helpPageId;
};

class NetworkSettingsPageState : public AppSettingsGUIPageState {
    Q_OBJECT
public:
    NetworkSettingsPageState();
    NetworkConfiguration config;
    QString webBrowserUrl;
    bool useDefaultWebBrowser;
};

class NetworkSettingsPageWidget : public AppSettingsGUIPageWidget, public Ui_NetworkSettingsWidget {
    Q_OBJECT
public:
    NetworkSettingsPageWidget();

    virtual void setState(AppSettingsGUIPageState* state);

    virtual AppSettingsGUIPageState* getState(QString& err) const;

private slots:
    void sl_HttpChecked(int state);
    void sl_ExceptionsChecked(int state);
    void sl_changeWebBrowserPathButtonClicked();
};

}  // namespace U2

