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

#ifndef _U2_WORKFLOW_SETTINGS_H_
#define _U2_WORKFLOW_SETTINGS_H_

#include <QFont>
#include <QObject>
#include <QString>

#include <U2Core/global.h>

namespace U2 {

class Watcher : public QObject {
    Q_OBJECT
signals:
    void changed();
    friend class WorkflowSettings;
};

class U2LANG_EXPORT WorkflowSettings : public QObject {
    Q_OBJECT
public:
    static bool showGrid();
    static void setShowGrid(bool v);

    static bool snap2Grid();
    static void setSnap2Grid(bool v);

    static bool monitorRun();
    static void setMonitorRun(bool v);

    static bool isDebuggerEnabled();
    static void setDebuggerEnabled(bool v);

    static QString defaultStyle();
    static void setDefaultStyle(const QString&);

    static QFont defaultFont();
    static void setDefaultFont(const QFont&);

    static const QString getUserDirectory();
    static void setUserDirectory(const QString& dir);

    static QColor getBGColor();
    static void setBGColor(const QColor& color);

    static bool getScriptingMode();
    static void setScriptingMode(bool md);

    static void setExternalToolDirectory(const QString& newDir);
    static const QString getExternalToolDirectory();

    static void setIncludedElementsDirectory(const QString& newDir);
    static const QString getIncludedElementsDirectory();

    static bool isOutputDirectorySet();
    static void setWorkflowOutputDirectory(const QString& newDir);
    static const QString getWorkflowOutputDirectory();

    static Watcher* const watcher;
};

}  // namespace U2

#endif
