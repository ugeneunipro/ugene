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

#ifndef _U2_ATTRIBUTEINFO_H_
#define _U2_ATTRIBUTEINFO_H_

#include <U2Core/global.h>
#include <U2Core/U2OpStatus.h>

namespace U2 {

namespace Workflow {
    class Actor;
}

class U2LANG_EXPORT AttributeInfo {
public:
    AttributeInfo(const QString &actorId, const QString &attrId, const QVariantMap &hints = QVariantMap());

    void validate(const QList<Workflow::Actor*> &actors, U2OpStatus &os) const;
    bool operator== (const AttributeInfo &other) const;
    QString toString() const;

    static AttributeInfo fromString(const QString &value, U2OpStatus &os);

    QString actorId;
    QString attrId;
    QVariantMap hints;

public:
    static const QString TYPE;
    static const QString DEFAULT;
    static const QString DATASETS;

    static const QString LABEL;
};

} // U2

#endif // _U2_ATTRIBUTEINFO_H_
