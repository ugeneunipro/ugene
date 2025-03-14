/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QMap>
#include <QStringList>

#include <U2Core/global.h>

namespace U2 {

class ExternalTool;

class U2CORE_EXPORT ScriptingTool : public QObject {
    Q_OBJECT
public:
    ScriptingTool(const QString& id, const QString& name, const QString& path, const QStringList& runParams = QStringList());

    const QString getId() const {
        return id;
    }
    const QString getName() const {
        return name;
    }
    const QString getPath() const {
        return path;
    }
    const QStringList getRunParameters() const {
        return runParams;
    }

    static void onPathChanged(ExternalTool* tool, const QStringList& runParams = QStringList());

protected:
    QString id;
    QString name;
    QString path;
    QStringList runParams;  // used for every program run (e.g. -jar in Java)
};

class U2CORE_EXPORT ScriptingToolRegistry : public QObject {
    Q_OBJECT
public:
    ~ScriptingToolRegistry();

    ScriptingTool* getById(const QString& id);

    bool registerEntry(ScriptingTool* t);
    void unregisterEntry(const QString& id);

    QList<ScriptingTool*> getAllEntries() const;
    QStringList getAllNames() const;

protected:
    QMap<QString, ScriptingTool*> registry;
};

}  // namespace U2
