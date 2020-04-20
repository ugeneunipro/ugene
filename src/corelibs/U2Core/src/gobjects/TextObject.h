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

#ifndef _U2_TEXT_OBJECT_H_
#define _U2_TEXT_OBJECT_H_

#include <U2Core/GObject.h>
#include <U2Core/U2RawData.h>
#include <U2Core/UdrRecord.h>

namespace U2 {

class U2CORE_EXPORT U2Text : public U2RawData {
public:
    U2Text();
    U2Text(const U2DbiRef &dbiRef);

    U2DataType getType() const;
};

class U2CORE_EXPORT TextObject : public GObject {
    Q_OBJECT
public:
    static TextObject * createInstance(const QString &text, const QString &objectName,
        const U2DbiRef &dbiRef, U2OpStatus &os, const QVariantMap &hintsMap = QVariantMap());

    TextObject(const QString &objectName, const U2EntityRef &textRef,
        const QVariantMap &hintsMap = QVariantMap());

    QString getText() const;

    void setText(const QString &newText);

    GObject * clone(const U2DbiRef &dstDbiRef, U2OpStatus &os, const QVariantMap &hints = QVariantMap()) const;

private:
    void commitTextToDB(const QString &newText);
};

}//namespace


#endif
