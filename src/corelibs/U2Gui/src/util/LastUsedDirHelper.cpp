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

#include "LastUsedDirHelper.h"

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>

#include <QDir>

namespace U2 {

LastUsedDirHelper::LastUsedDirHelper(const QString& d, const QString& defaultVal) {
    domain = d;
    dir = getLastUsedDir(domain, defaultVal);
}

LastUsedDirHelper::~LastUsedDirHelper() {
    saveLastUsedDir();
}

void LastUsedDirHelper::saveLastUsedDir() {
    if (!url.isEmpty()) {
        QFileInfo fi(url);
        if (fi.isDir()) { // sometimes 'url' points to a dir, not a file.
            dir = fi.absoluteFilePath();
        } else {
            dir = fi.absoluteDir().absolutePath();
        }
    }
    if (!dir.isEmpty()) {
        setLastUsedDir(dir, domain);
    }
}

#define SETTINGS_ROOT QString("gui/")

QString LastUsedDirHelper::getLastUsedDir(const QString& toolType, const QString& defaultVal) {
    QString key = SETTINGS_ROOT + (toolType.isEmpty() ? "" : toolType + "/") + "lastDir";
    QString defDir = defaultVal;
    if (defDir.isEmpty() && toolType.isEmpty()) {
        QStringList dataDirs = QDir::searchPaths( PATH_PREFIX_DATA );
        if (!dataDirs.isEmpty()) {
            defDir = dataDirs.first() + "/samples";
        }
    }
    QString res = AppContext::getSettings()->getValue(key, defDir).toString();
    return res;
}

void LastUsedDirHelper::setLastUsedDir(const QString& ld, const QString& toolType) {
    QString key = SETTINGS_ROOT + (toolType.isEmpty() ? "" : toolType + "/") + "lastDir";
    AppContext::getSettings()->setValue(key, ld);
}


}//namespace
