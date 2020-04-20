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

#include <U2Core/U2SafePoints.h>

#include "Variable.h"

namespace U2 {

Variable::Variable() {
    assigned = false;
}

Variable::Variable(const QString &_name)
: name(_name)
{
    assigned = false;
}

Variable::~Variable() {

}

const QString & Variable::getName() const {
    return name;
}

const QString & Variable::getValue() const {
    SAFE_POINT(assigned, QObject::tr("Retrieving value of unassigned variable: %1").arg(name), value);
    return value;
}

void Variable::setValue(const QString &value) {
    assigned = true;
    this->value = value;
}

bool Variable::isAssigned() const {
    return assigned;
}

bool Variable::operator == (const Variable &other) const {
    CHECK(name == other.name, false);
    SAFE_POINT(assigned, QObject::tr("Unassigned variable: %1").arg(name), false);
    SAFE_POINT(other.assigned, QObject::tr("Unassigned variable: %1").arg(other.name), false);
    return (value == other.value);
}

Predicate::Predicate() {

}

Predicate::Predicate(const Variable &v, const QString &value)
: var(v)
{
    var.setValue(value);
}

Variable Predicate::variable() const {
    return var;
}

bool Predicate::isTrue(const QMap<QString, Variable> &vars) const {
    SAFE_POINT(vars.contains(var.getName()), QObject::tr("Variable is not defined: %1").arg(var.getName()), false);
    return (var == vars[var.getName()]);
}

QString Predicate::toString() const {
    return var.getName() + "." + var.getValue();
}

Predicate Predicate::fromString(const QString &string, U2OpStatus &os) {
    QStringList token = string.split("."); // var.value
    if (2 != token.size()) {
        os.setError(QObject::tr("Can not parse predicate from the string: %1").arg(string));
        return Predicate();
    }
    return Predicate(Variable(token[0]), token[1]);
}

bool Predicate::operator < (const Predicate &other) const {
    return (toString() < other.toString());
}

} // U2
