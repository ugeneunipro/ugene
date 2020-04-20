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

#ifndef _U2_FORMAT_SETTINGS_GUI_CONTROLLER_H_
#define _U2_FORMAT_SETTINGS_GUI_CONTROLLER_H_

#include <ui_FormatSettingsWidget.h>

#include <U2Core/FormatSettings.h>

#include <U2Gui/AppSettingsGUI.h>

namespace U2 {


class FormatSettingsGUIPageController: public AppSettingsGUIPageController {
    Q_OBJECT
public:
    FormatSettingsGUIPageController(QObject *p = NULL);

    virtual AppSettingsGUIPageState *getSavedState();

    virtual void saveState(AppSettingsGUIPageState *s);

    virtual AppSettingsGUIPageWidget *createWidget(AppSettingsGUIPageState *data);

    const QString& getHelpPageId() const {return helpPageId;};

private:
    static const QString helpPageId;
};


class FormatSettingsGUIPageState : public AppSettingsGUIPageState {
    Q_OBJECT
public:
    FormatSettingsGUIPageState() : caseMode(NO_CASE_ANNS) {}
    CaseAnnotationsMode caseMode;
};


class FormatSettingsGUIPageWidget: public AppSettingsGUIPageWidget, public Ui_FormatSettingsWidget {
    Q_OBJECT
public:
    FormatSettingsGUIPageWidget(FormatSettingsGUIPageController *ctrl);

    virtual void setState(AppSettingsGUIPageState *state);

    virtual AppSettingsGUIPageState *getState(QString &err) const;

private:
    QMap<CaseAnnotationsMode, QString> caseAnnsModeNames;
};

} // U2

#endif // _U2_FORMAT_SETTINGS_GUI_CONTROLLER_H_
