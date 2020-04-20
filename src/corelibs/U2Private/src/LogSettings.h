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

#ifndef _U2_LOG_SETTINGS_H_
#define _U2_LOG_SETTINGS_H_

#include <U2Core/global.h>
#include <U2Core/Log.h>
#include <U2Core/AppContext.h>

#include <QHash>

namespace U2 {

struct U2PRIVATE_EXPORT LoggerSettings {
    LoggerSettings() {
        qFill(activeLevelFlag, activeLevelFlag + LogLevel_NumLevels, 0);
    }

    bool operator==(const LoggerSettings& other) const {
        return categoryName == other.categoryName && activeLevelFlag == other.activeLevelFlag;
    }

    QString categoryName;
    bool    activeLevelFlag[LogLevel_NumLevels];

};

class U2PRIVATE_EXPORT LogCategories : QObject {
    Q_OBJECT
public:
    static void init();
    static const QString getLocalizedLevelName(LogLevel l) {
        return localizedLevelNames[l];
    }
protected:
    static QString localizedLevelNames[LogLevel_NumLevels];
};

class U2PRIVATE_EXPORT LogSettings {
public:
    LogSettings();

    void removeCategory(const QString& name);

    void addCategory(const LoggerSettings& newcs);

    const LoggerSettings& getLoggerSettings(const QString& cName);

    const QHash<QString, LoggerSettings> getLoggerSettings() const {return categories;}

    bool operator==(const LogSettings& other) const;

    void save();

    void reinitAll();
    void reinitCategories();

    QString levelColors[LogLevel_NumLevels];
    bool    activeLevelGlobalFlag[LogLevel_NumLevels];
    QString logPattern;

    bool showDate;
    bool showLevel;
    bool showCategory;
    bool enableColor;
    bool toFile;
    QString outputFile;

//private:
    QHash<QString, LoggerSettings> categories;
};

class U2PRIVATE_EXPORT LogSettingsHolder {
public:
    const LogSettings& getSettings() const {return settings;}

    virtual void setSettings(const LogSettings& s);

protected:
    mutable LogSettings settings;

};

} //namespace

#endif
