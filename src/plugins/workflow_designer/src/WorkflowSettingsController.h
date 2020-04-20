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

#ifndef _U2_W_SETTINGS_GUI_CONTROLLER_H_
#define _U2_W_SETTINGS_GUI_CONTROLLER_H_

#include <ui_WorkflowSettingsWidget.h>

#include <U2Gui/AppSettingsGUI.h>

namespace U2 {

#define WorkflowSettingsPageId QString("wds")

class WorkflowSettingsPageController : public AppSettingsGUIPageController {
    Q_OBJECT
public:
    WorkflowSettingsPageController(QObject* p = NULL);

    virtual AppSettingsGUIPageState* getSavedState();

    virtual void saveState(AppSettingsGUIPageState* s);

    virtual AppSettingsGUIPageWidget* createWidget(AppSettingsGUIPageState* data);

    const QString& getHelpPageId() const {return helpPageId;};

private:
    static const QString helpPageId;
};


class WorkflowSettingsPageState : public AppSettingsGUIPageState {
    Q_OBJECT
public:
    bool showGrid;
    bool snap2grid;
    bool lockRun;
    bool enableDebugger;
    QString style;
    QFont font;
    QString path;
    QString externalToolCfgDir;
    QString includedElementsDir;
    QString workflowOutputDir;
    QColor color;
    bool showEmptyPorts;
};


class WorkflowSettingsPageWidget: public AppSettingsGUIPageWidget, public Ui_WorkflowSettingsWidget {
    Q_OBJECT
public:
    WorkflowSettingsPageWidget(WorkflowSettingsPageController* ctrl);

    virtual void setState(AppSettingsGUIPageState* state);

    virtual AppSettingsGUIPageState* getState(QString& err) const;

    virtual bool eventFilter( QObject * watched, QEvent * event );

private slots:
    void sl_getDirectory();
    void sl_getExternalToolCfgDir();
    void sl_getIncludedElementsDir();
    void sl_getWorkflowOutputDir();
    void sl_getColor();
};

}//namespace

#endif
