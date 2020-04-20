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

#include <U2Core/AppContext.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/U2SafePoints.h>

#include "ScriptingToolRegistry.h"

namespace U2 {

////////////////////////////////////////
//ScriptingTool
ScriptingTool::ScriptingTool(const QString& _id, const QString& _name, const QString& _path, const QStringList& _runParams)
                            : id(_id), name(_name), path(_path), runParams(_runParams) {}

void ScriptingTool::onPathChanged(ExternalTool *tool, const QStringList& runParams) {
    ScriptingToolRegistry* reg = AppContext::getScriptingToolRegistry();
    CHECK(NULL != reg, );

    if (tool->isValid()){
        if (NULL != reg->getById(tool->getId())){
            reg->unregisterEntry(tool->getId());
        }
        if (!tool->getPath().isEmpty()){
            reg->registerEntry(new ScriptingTool(tool->getId(), tool->getName(), tool->getPath(), runParams));
        }
    } else {
        reg->unregisterEntry(tool->getId());
    }
}

////////////////////////////////////////
//ScriptingToolRegistry
ScriptingToolRegistry::~ScriptingToolRegistry() {
    qDeleteAll(registry.values());
}

ScriptingTool* ScriptingToolRegistry::getById(const QString& id){
    return registry.value(id, NULL);
}

bool ScriptingToolRegistry::registerEntry(ScriptingTool *t){
    if (registry.contains(t->getId())) {
        return false;
    } else {
        registry.insert(t->getId(), t);
        return true;
    }
}

void ScriptingToolRegistry::unregisterEntry(const QString &id){
    delete registry.take(id);
}

QList<ScriptingTool*> ScriptingToolRegistry::getAllEntries() const{
    return registry.values();
}

QStringList ScriptingToolRegistry::getAllNames() const {
    return registry.keys();
}

}//namespace
