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

#pragma once

#include <U2Core/GObjectReference.h>

#include <U2Gui/ObjectViewTasks.h>

namespace U2 {

class AssemblyObject;
class UnloadedObject;
class Document;
class AssemblyBrowser;

class OpenAssemblyBrowserTask : public ObjectViewTask {
    Q_OBJECT
public:
    OpenAssemblyBrowserTask(AssemblyObject* obj);
    OpenAssemblyBrowserTask(UnloadedObject* obj);
    OpenAssemblyBrowserTask(Document* doc);
    void open() override;
    static void updateTitle(AssemblyBrowser* ab);
    static AssemblyBrowser* openBrowserForObject(AssemblyObject* obj, const QString& viewName, bool persistent);

private:
    GObjectReference unloadedObjRef;
};

class OpenSavedAssemblyBrowserTask : public ObjectViewTask {
    Q_OBJECT
public:
    OpenSavedAssemblyBrowserTask(const QString& viewName, const QVariantMap& stateData);
    void open() override;
};

class UpdateAssemblyBrowserTask : public ObjectViewTask {
    Q_OBJECT
public:
    UpdateAssemblyBrowserTask(GObjectViewController* v, const QString& stateName, const QVariantMap& stateData)
        : ObjectViewTask(v, stateName, stateData) {
    }

    void update() override;
};

}  // namespace U2
