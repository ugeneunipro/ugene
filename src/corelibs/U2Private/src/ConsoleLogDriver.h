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

#ifndef _U2_CONSOLE_LOG_DRIVER_H_
#define _U2_CONSOLE_LOG_DRIVER_H_

#include <U2Core/Log.h>
#include "LogSettings.h"

namespace U2 {

class U2PRIVATE_EXPORT ConsoleLogDriver : public QObject, public LogListener, public LogSettingsHolder {
    Q_OBJECT
public:
    static const QString LOG_SHOW_DATE_CMD_OPTION;
    static const QString LOG_SHOW_LEVEL_CMD_OPTION;
    static const QString LOG_SHOW_CATEGORY_CMD_OPTION;
    static const QString LOG_LEVEL_NONE_CMD_OPTION;
    static const QString LOG_LEVEL_ERROR_CMD_OPTION;
    static const QString LOG_LEVEL_INFO_CMD_OPTION;
    static const QString LOG_LEVEL_DETAILS_CMD_OPTION;
    static const QString LOG_LEVEL_TRACE_CMD_OPTION;
    static const QString LOG_SETTINGS_ACTIVE_FLAG;
    static const QString COLOR_OUTPUT_CMD_OPTION;
    //static const QString LOG_FORMAT;
    //static const QString LOG_LEVEL;

    static void setLogCmdlineHelp();
    void setLogSettings();

private:
    static bool helpRegistered;

public:
    ConsoleLogDriver();
    virtual ~ConsoleLogDriver();

    void onMessage(const LogMessage& msg);

private:
    void setCmdLineSettings();
    QString getEffectiveCategory(const LogMessage& msg) const;
    QString getLevelName(int i) const;

private slots:
    QString prepareText(const LogMessage& msg) const;
private:
    bool printToConsole;
    bool colored;

};

} //namespace

#endif
