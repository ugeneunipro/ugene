/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#ifndef _U2_EXTERNAL_TOOL_SUPPORT_SETTINGS_H_
#define _U2_EXTERNAL_TOOL_SUPPORT_SETTINGS_H_

#include <QDirIterator>
#include <QFont>
#include <QObject>
#include <QQueue>
#include <QString>

#include <U2Core/Log.h>
#include <U2Core/global.h>

namespace U2 {

class U2OpStatus;

class Watcher : public QObject {
    Q_OBJECT
signals:
    void changed();
    friend class ExternalToolSupportSettings;
};

#define SETTINGS QString("ExternalToolSupport/")

class ExternalToolSupportSettings {
public:
    static int getNumberExternalTools();
    static void setNumberExternalTools(int value);

    static void loadExternalToolsFromAppConfig();
    static void saveExternalToolsToAppConfig();

    static void checkTemporaryDir(U2OpStatus& os);

    /**
     * Calls checkTemporaryDir(os) with 'U2OpStatus2Log os'.
     * Returns true if the dir set correctly.
     */
    static bool checkTemporaryDir(const LogLevel& logLevel = LogLevel::LogLevel_DETAILS);

    static Watcher* const watcher;

private:
    static int prevNumberExternalTools;
};

/**Helper class that iterates through subfolders up to given deep level*/
class LimitedDirIterator {
public:
    // deepLevel = 0 - returns only the root dir
    // deepLevel = 1 - returns the root dir and its subdirs
    //...
    LimitedDirIterator(const QDir& dir, int deepLevel = DEFAULT_DEEP_LEVEL);

    bool hasNext();

    QString next();
    QString filePath();

    static const int DEFAULT_DEEP_LEVEL = 5;

private:
    void fetchNext();

private:
    int deepLevel;

    QQueue<QPair<QString, int>> data;

    QString curPath;
};

}  // namespace U2

#endif
