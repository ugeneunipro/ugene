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

#ifndef _U2_APPSETTINGS_GUI_H_
#define _U2_APPSETTINGS_GUI_H_

#include <U2Core/global.h>

#include <QDialogButtonBox>
#include <QWidget>

namespace U2 {

class AppSettingsGUIPageController;

/** public class to access GUI */
class U2GUI_EXPORT AppSettingsGUI : public QObject {
    Q_OBJECT
public:
    AppSettingsGUI(QObject* p = nullptr) : QObject(p) {};

    virtual bool registerPage(AppSettingsGUIPageController* page, const QString& beforePage = QString()) = 0;

    virtual bool unregisterPage(AppSettingsGUIPageController* page) = 0;

    virtual void showSettingsDialog(const QString& pageId = QString()) const = 0;

    virtual QList<AppSettingsGUIPageController*> getRegisteredPages() const = 0;
};


/** Data stored on the page */
class U2GUI_EXPORT AppSettingsGUIPageState : public QObject {
    Q_OBJECT
public:
    AppSettingsGUIPageState(QObject* p = nullptr) : QObject(p) {}
};

/** Page widget */
class U2GUI_EXPORT AppSettingsGUIPageWidget : public QWidget {
    Q_OBJECT
public:
    AppSettingsGUIPageWidget(QWidget* w = nullptr) : QWidget(w) {}

    //reads data from 'state' and initializes the widget state
    virtual void setState(AppSettingsGUIPageState* state) = 0;

    //returns 'state' of the widget or error if state is not complete
    // the result structure is automatically deleted by controller
    virtual AppSettingsGUIPageState* getState(QString& errMsg) const = 0;

signals:
    void si_setLockState(bool);
};

/** Page factory/controller */
class U2GUI_EXPORT AppSettingsGUIPageController : public QObject {
    Q_OBJECT
public:
    AppSettingsGUIPageController(const QString& s, const QString& _id, QObject* p = nullptr) : QObject(p), name(s), id(_id) {}

    const QString& getPageName() const { return name; }

    const QString& getPageId() const { return id; }

    //should be pure virtual, because every page should have own help page index
    virtual const QString& getHelpPageId() const = 0;

    virtual AppSettingsGUIPageState* getSavedState() = 0;

    virtual void saveState(AppSettingsGUIPageState* s) = 0;

    //creates widget and initializes its values with 'data' content
    virtual AppSettingsGUIPageWidget* createWidget(AppSettingsGUIPageState* data) = 0;

private:
    QString name;
    QString id;
};

#define APP_SETTINGS_GUI_NETWORK    "network"
#define APP_SETTINGS_GUI_LOG        "log"
#define APP_SETTINGS_USER_APPS      "user_apps"
#define APP_SETTINGS_DIRECTORIES    "directories"
#define APP_SETTINGS_RESOURCES      "resources"
#define APP_SETTINGS_EXTERNAL_TOOLS "ets"
#define APP_SETTINGS_FORMAT         "format"

}//namespace

#endif
