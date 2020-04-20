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

#ifndef _U2_RESOURCE_SETTINGS_GUI_CONTROLLER_H_
#define _U2_RESOURCE_SETTINGS_GUI_CONTROLLER_H_

#include <ui_ResourceSettingsWidget.h>

#include <U2Core/NetworkConfiguration.h>
#include <U2Gui/AppSettingsGUI.h>

#include <QUrl>

namespace U2 {


class ResourceSettingsGUIPageController: public AppSettingsGUIPageController {
    Q_OBJECT
public:
    ResourceSettingsGUIPageController(QObject* p = NULL);

    virtual AppSettingsGUIPageState* getSavedState();

    virtual void saveState(AppSettingsGUIPageState* s);

    virtual AppSettingsGUIPageWidget* createWidget(AppSettingsGUIPageState* data);

    const QString& getHelpPageId() const {return helpPageId;};

private:
    static const QString helpPageId;
};


class ResourceSettingsGUIPageState : public AppSettingsGUIPageState {
    Q_OBJECT
public:
    ResourceSettingsGUIPageState() : nCpus(0), nThreads(0), maxMem(0){}
    int nCpus;
    int nThreads;
    int maxMem;
};


class ResourceSettingsGUIPageWidget: public AppSettingsGUIPageWidget, public Ui_ResourceSettingsWidget {
    Q_OBJECT
public:
    ResourceSettingsGUIPageWidget(ResourceSettingsGUIPageController* ctrl);

    virtual void setState(AppSettingsGUIPageState* state);

    virtual AppSettingsGUIPageState* getState(QString& err) const;

private slots:
    void sl_threadsCountChanged(int n);
    void sl_cpuCountChanged(int n);
};

}//namespace

#endif
