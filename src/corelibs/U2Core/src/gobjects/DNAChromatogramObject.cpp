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

#include <U2Core/ChromatogramUtils.h>
#include <U2Core/DatatypeSerializeUtils.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GHints.h>
#include <U2Core/RawDataUdrSchema.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "DNAChromatogramObject.h"
#include "GObjectTypes.h"

namespace U2 {

/////// U2Chromatogram Implementation /////////////////////////////////////////////////////////////

U2Chromatogram::U2Chromatogram() : U2RawData() {

}

U2Chromatogram::U2Chromatogram(const U2DbiRef &dbiRef) : U2RawData(dbiRef) {

}

U2Chromatogram::U2Chromatogram(const U2RawData &rawData)
    : U2RawData(rawData)
{

}

U2DataType U2Chromatogram::getType() const {
    return U2Type::Chromatogram;
}

/////// DNAChromatogramObject Implementation //////////////////////////////////////////////////////

DNAChromatogramObject * DNAChromatogramObject::createInstance(const DNAChromatogram &chroma,
    const QString &objectName, const U2DbiRef &dbiRef, U2OpStatus &os, const QVariantMap &hintsMap)
{
    const QString folder = hintsMap.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
    const U2EntityRef chromatogramRef = ChromatogramUtils::import(os, dbiRef, folder, chroma);
    return new DNAChromatogramObject(objectName, chromatogramRef, hintsMap);
}

DNAChromatogramObject::DNAChromatogramObject(const QString &objectName, const U2EntityRef &chromaRef, const QVariantMap &hintsMap)
: GObject(GObjectTypes::CHROMATOGRAM, objectName, hintsMap), cached(false)
{
    entityRef = chromaRef;
}

const DNAChromatogram & DNAChromatogramObject::getChromatogram() const {
    ensureDataLoaded();
    return cache;
}

void DNAChromatogramObject::setChromatogram(U2OpStatus &os, const DNAChromatogram &chromatogram) {
    ChromatogramUtils::updateChromatogramData(os, getEntityRef(), chromatogram);
    CHECK_OP(os, );
    cache = chromatogram;
}

void DNAChromatogramObject::loadDataCore(U2OpStatus &os) {
    const QString serializer = RawDataUdrSchema::getObject(entityRef, os).serializer;
    CHECK_OP(os, );
    SAFE_POINT(DNAChromatogramSerializer::ID == serializer, "Unknown serializer id", );
    const QByteArray data = RawDataUdrSchema::readAllContent(entityRef, os);
    CHECK_OP(os, );
    cache = DNAChromatogramSerializer::deserialize(data, os);
}

GObject * DNAChromatogramObject::clone(const U2DbiRef &dstRef, U2OpStatus &os, const QVariantMap &hints) const {
    GHintsDefaultImpl gHints(getGHintsMap());
    gHints.setAll(hints);
    const QString dstFolder = gHints.get(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();

    U2Chromatogram dstObject;
    RawDataUdrSchema::cloneObject(entityRef, dstRef, dstFolder, dstObject, os);
    CHECK_OP(os, NULL);

    U2EntityRef dstEntRef(dstRef, dstObject.id);
    DNAChromatogramObject *dst = new DNAChromatogramObject(getGObjectName(), dstEntRef, gHints.getMap());
    dst->setIndexInfo(getIndexInfo());
    return dst;
}

}//namespace


