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

#ifndef _U2_LOG_SETTINGS_WIDGET_IMPL_H_
#define _U2_LOG_SETTINGS_WIDGET_IMPL_H_

#include <QLabel>
#include <QTableWidgetItem>
#include <QUrl>
#include <ui_LogSettingsWidget.h>

#include <LogSettings.h>

#include <U2Gui/AppSettingsGUI.h>


namespace U2 {

class SaveDocumentController;

class LogSettingsPageController : public AppSettingsGUIPageController {
    Q_OBJECT
public:
    LogSettingsPageController(LogSettingsHolder* target, QObject* p = NULL);

    virtual AppSettingsGUIPageState* getSavedState();

    virtual void saveState(AppSettingsGUIPageState* s);

    virtual AppSettingsGUIPageWidget* createWidget(AppSettingsGUIPageState* data);

    const QString& getHelpPageId() const {return helpPageId;}

private:
    LogSettingsHolder* target;
    static const QString helpPageId;
};


class LogSettingsPageState : public AppSettingsGUIPageState {
    Q_OBJECT
public:
    LogSettingsPageState();
    LogSettings settings;
};


class LogSettingsPageWidget: public AppSettingsGUIPageWidget, public Ui_LogSettingsWidget {
    Q_OBJECT
public:
    LogSettingsPageWidget();

    virtual void setState(AppSettingsGUIPageState* state);

    virtual AppSettingsGUIPageState* getState(QString& err) const;

private slots:
    void sl_currentCellChanged ( int currentRow, int currentColumn, int previousRow, int previousColumn);
    void sl_levelStateChanged(int state);
    void sl_catItemStateChanged(QTableWidgetItem *item);
    void sl_changeColor(const QString& v);
    void sl_outFileStateChanged(int state);

private:
    void updateColorLabel(QLabel* l, const QString& color);
    void initSaveController();

    SaveDocumentController *saveController;
};

class LogSettingsTopLineWidget : public QWidget {
    Q_OBJECT
public:
    LogSettingsTopLineWidget(QWidget* parent, const QString& _color, LogLevel _level) 
        : QWidget(parent), color(_color), level(_level), cb(NULL){}
    QString     color;
    LogLevel    level;
    QCheckBox*  cb;
};

}//namespace

#endif
