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

#ifndef _U2_VARIANT_TRACK_OBJECT_H_
#define _U2_VARIANT_TRACK_OBJECT_H_

#include <U2Core/GObject.h>
#include <U2Core/U2Type.h>
#include <U2Core/U2Variant.h>
#include <U2Core/U2Region.h>

namespace U2 {

class U2CORE_EXPORT VariantTrackObject : public GObject {
    Q_OBJECT

public:
    VariantTrackObject(const QString& objectName, const U2EntityRef& trackRef, const QVariantMap& hintsMap = QVariantMap());
    ~VariantTrackObject();


    GObject * clone(const U2DbiRef &dbiRef, U2OpStatus &os, const QVariantMap &hints = QVariantMap()) const;

    /**U2_REGION_MAX to get all variants*/
    U2DbiIterator<U2Variant>* getVariants(const U2Region& reg, U2OpStatus& os) const;
    int getVariantCount(U2OpStatus &os) const;

    void addVariants(const QList<U2Variant>& variants, U2OpStatus& os);

    U2VariantTrack getVariantTrack(U2OpStatus &os) const;
};

}//namespace


#endif //_U2_VARIANT_TRACK_OBJECT_H_
