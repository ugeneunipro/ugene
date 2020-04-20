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

#include <QScriptEngine>

#include "ProjectModel.h"

namespace U2 {

Project::~Project() {

}

void Project::setupToEngine(QScriptEngine *engine) {
    Document::setupToEngine(engine);
    qScriptRegisterMetaType(engine, toScriptValue, fromScriptValue);
    qScriptRegisterSequenceMetaType<QList<Document*> >(engine);
};
QScriptValue Project::toScriptValue(QScriptEngine *engine, Project* const &in) {
    return engine->newQObject(in);
}

void Project::fromScriptValue(const QScriptValue &object, Project* &out) {
    out = qobject_cast<Project*>(object.toQObject());
}

}//namespace
