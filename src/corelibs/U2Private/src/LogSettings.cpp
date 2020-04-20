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

#include "LogSettings.h"

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include <U2Core/CMDLineRegistry.h>

#include <QColor>

#define SETTINGS_ROOT QString("log_settings/")

namespace U2 {

QString LogCategories::localizedLevelNames[LogLevel_NumLevels];

void LogCategories::init() {
    localizedLevelNames[LogLevel_TRACE]   = tr("TRACE");
    localizedLevelNames[LogLevel_DETAILS] = tr("DETAILS");
    localizedLevelNames[LogLevel_INFO]    = tr("INFO");
    localizedLevelNames[LogLevel_ERROR]   = tr("ERROR");
}

LogSettings::LogSettings() :
    showDate(false),
    showLevel(false),
    showCategory(false),
    enableColor(false),
    toFile(false)
{
    //created in not inited state
    memset(activeLevelGlobalFlag, 0, sizeof(bool) * LogLevel_NumLevels);
}

const LoggerSettings& LogSettings::getLoggerSettings(const QString& cName) {
    QHash<QString, LoggerSettings>::const_iterator it = categories.find(cName);
    if( it == categories.end()) {
        reinitCategories();
        it = categories.find(cName);
        assert(it!=categories.end());
    }
    const LoggerSettings& cs = it.value();
    return cs;
}

void LogSettings::reinitAll() {
    Settings *s = AppContext::getSettings();

    for (int i=0; i < LogLevel_NumLevels; i++) {
        if (i == LogLevel_TRACE) {
            levelColors[i] = QColor(Qt::darkGray).name();
        } else if (i == LogLevel_INFO) {
            levelColors[i] = QColor(Qt::darkBlue).name();
        } else if (i == LogLevel_ERROR) {
            levelColors[i] = QColor(Qt::darkRed).name();
        } else {
            levelColors[i] = QColor(Qt::black).name();
        }
    }

    showDate = s->getValue(SETTINGS_ROOT + "showDate", true).toBool();
    showLevel = s->getValue(SETTINGS_ROOT + "showLevel", true).toBool();
    showCategory = s->getValue(SETTINGS_ROOT + "showCategory", false).toBool();
    logPattern = s->getValue(SETTINGS_ROOT + "datePattern", "hh:mm").toString();
    enableColor = s->getValue(SETTINGS_ROOT + "enableColor", true).toBool();
    toFile = s->getValue(SETTINGS_ROOT + "logToFile", false).toBool();
    outputFile = s->getValue(SETTINGS_ROOT + "outFilePath", QString("")).toString();

    reinitCategories();
}

void LogSettings::reinitCategories() {
    LogCategories::init();
    Settings *s = AppContext::getSettings();
    for (int i=0; i<LogLevel_NumLevels; i++) {
        activeLevelGlobalFlag[i] = s->getValue(SETTINGS_ROOT + "activeFlagLevel"+QString::number(i), i >= LogLevel_INFO).toBool();
    }

    LogServer* ls = LogServer::getInstance();
    const QStringList& categoryList = ls->getCategories();
    foreach(const QString& name, categoryList) {
        if (!categories.contains(name)) {
            LoggerSettings cs;
            cs.categoryName = name;
            for (int i=0; i<LogLevel_NumLevels; i++) {
                cs.activeLevelFlag[i] = s->getValue(SETTINGS_ROOT + "categories/" + cs.categoryName + "/activeFlagLevel" + QString::number(i), activeLevelGlobalFlag[i]).toBool();
            }
            categories[name] = cs;
        }
    }

}

void LogSettings::removeCategory(const QString& name) {
    int n = categories.remove(name);
    assert(n == 1);
    Q_UNUSED(n);
}


void LogSettings::addCategory(const LoggerSettings& newcs) {
    assert(!categories.contains(newcs.categoryName));
    categories[newcs.categoryName] = newcs;
}

void LogSettings::save() {
    Settings *s = AppContext::getSettings();

    foreach(const LoggerSettings& cs, categories.values()) {
        for (int i=0; i < LogLevel_NumLevels; i++) {
            s->setValue(SETTINGS_ROOT + "categories/" + cs.categoryName + "/activeFlagLevel" + QString::number(i), cs.activeLevelFlag[i]);
        }
    }
    for (int i=0; i < LogLevel_NumLevels; i++) {
        s->setValue(SETTINGS_ROOT + "color" + QString::number(i), levelColors[i]);
        s->setValue(SETTINGS_ROOT + "activeFlagLevel"+QString::number(i), activeLevelGlobalFlag[i]);
    }

    s->setValue(SETTINGS_ROOT + "showDate", showDate);
    s->setValue(SETTINGS_ROOT + "showLevel", showLevel);
    s->setValue(SETTINGS_ROOT + "showCategory", showCategory);
    s->setValue(SETTINGS_ROOT + "enableColor", enableColor);
    s->setValue(SETTINGS_ROOT + "datePattern", logPattern);
    s->setValue(SETTINGS_ROOT + "logToFile", toFile);
    s->setValue(SETTINGS_ROOT + "outFilePath", outputFile);
}

bool LogSettings::operator==(const LogSettings& other) const {
    bool res = levelColors == other.levelColors
            && activeLevelGlobalFlag == other.activeLevelGlobalFlag
            && showDate == other.showDate
            && showLevel == other.showLevel
            && showCategory == other.showCategory
            && categories == other.categories;

    return res;
}


void LogSettingsHolder::setSettings(const LogSettings& s) {
    if (settings == s) {
        return;
    }
    settings = s;
    settings.save();

}
}//namespace

