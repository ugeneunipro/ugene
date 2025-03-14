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

#include <U2Core/GObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2Type.h>

namespace U2 {

class U2CORE_EXPORT AssemblyObject : public GObject {
    Q_OBJECT
public:
    AssemblyObject(const QString& objectName, const U2EntityRef& ref, const QVariantMap& hints = QVariantMap());

    /** Returns number of reads in the object. */
    qint64 getReadCount(U2OpStatus& os) const;

    GObject* clone(const U2DbiRef& dstDbiRef, U2OpStatus& os, const QVariantMap& hints = QVariantMap()) const override;

    static U2EntityRef dbi2dbiClone(const AssemblyObject* const srcObj, const U2DbiRef& dstDbiRef, U2OpStatus& os, const QVariantMap& hints = QVariantMap());
    static U2EntityRef dbi2dbiExtractRegion(const AssemblyObject* const srcObj, const U2DbiRef& dstDbiRef, U2OpStatus& os, const U2Region& desiredRegion = U2_REGION_MAX, const QVariantMap& hints = QVariantMap());
};

}  // namespace U2
