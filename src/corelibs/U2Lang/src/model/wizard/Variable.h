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

#ifndef _U2_VARIABLE_H_
#define _U2_VARIABLE_H_

#include <QMap>

#include <U2Core/U2OpStatus.h>

namespace U2 {

class U2LANG_EXPORT Variable {
public:
    Variable();
    Variable(const QString &name);
    virtual ~Variable();

    const QString & getName() const;
    const QString & getValue() const;
    void setValue(const QString &value);
    bool isAssigned() const;
    bool operator == (const Variable &other) const;

private:
    QString name;
    QString value;
    bool assigned;
};

class U2LANG_EXPORT Predicate {
public:
    Predicate();
    Predicate(const Variable &v, const QString &value);

    Variable variable() const;
    bool isTrue(const QMap<QString, Variable> &vars) const;
    bool operator < (const Predicate &other) const;
    QString toString() const;

    static Predicate fromString(const QString &string, U2OpStatus &os);

private:
    Variable var;
};

} // U2

#endif // _U2_VARIABLE_H_
