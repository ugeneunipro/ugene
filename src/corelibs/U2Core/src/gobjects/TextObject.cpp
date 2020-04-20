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

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GHints.h>
#include <U2Core/RawDataUdrSchema.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "TextObject.h"

namespace U2 {

/////// U2Text Implementation ///////////////////////////////////////////////////////////////////

U2Text::U2Text() : U2RawData() {

}

U2Text::U2Text(const U2DbiRef &dbiRef) : U2RawData(dbiRef) {

}

U2DataType U2Text::getType() const {
    return U2Type::Text;
}

/////// TextObject Implementation ///////////////////////////////////////////////////////////////////

TextObject * TextObject::createInstance(const QString &text, const QString &objectName, const U2DbiRef &dbiRef, U2OpStatus &os, const QVariantMap &hintsMap) {
    U2Text object(dbiRef);
    object.visualName = objectName;

    const QString folder = hintsMap.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
    RawDataUdrSchema::createObject(dbiRef, folder, object, os);
    CHECK_OP(os, NULL);

    U2EntityRef entRef(dbiRef, object.id);
    RawDataUdrSchema::writeContent(text.toUtf8(), entRef, os);
    CHECK_OP(os, NULL);

    return new TextObject(objectName, entRef, hintsMap);
}

TextObject::TextObject(const QString &objectName, const U2EntityRef &textRef, const QVariantMap &hintsMap)
: GObject(GObjectTypes::TEXT, objectName, hintsMap)
{
    entityRef = textRef;
}

QString TextObject::getText() const {
    U2OpStatus2Log os;
    QByteArray content = RawDataUdrSchema::readAllContent(entityRef, os);
    CHECK_OP(os, QString());
    return QString::fromUtf8(content);
}

void TextObject::setText(const QString &newText) {
    commitTextToDB(newText);
    setModified(true);
}

GObject * TextObject::clone(const U2DbiRef &dstDbiRef, U2OpStatus &os, const QVariantMap &hints) const {
    GHintsDefaultImpl gHints(getGHintsMap());
    gHints.setAll(hints);
    const QString &dstFolder = gHints.get(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

    U2Text dstObject;
    RawDataUdrSchema::cloneObject(entityRef, dstDbiRef, dstFolder, dstObject, os);
    CHECK_OP(os, NULL);

    U2EntityRef dstEntRef(dstDbiRef, dstObject.id);
    TextObject *dst = new TextObject(getGObjectName(), dstEntRef, gHints.getMap());
    dst->setIndexInfo(getIndexInfo());
    return dst;
}

void TextObject::commitTextToDB(const QString &newText) {
    U2OpStatus2Log os;
    RawDataUdrSchema::writeContent(newText.toUtf8(), entityRef, os);
}

}//namespace
