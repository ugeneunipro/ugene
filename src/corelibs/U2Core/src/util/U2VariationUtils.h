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

#ifndef _U2_VARIATION_UTILS_H_
#define _U2_VARIATION_UTILS_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/FormatSettings.h>
#include <U2Core/U2Variant.h>
#include <U2Core/U2Feature.h>
#include <U2Core/U2DbiUtils.h>

namespace U2 {

/**
 * U2Variant and related structures utility functions
 */

enum CallVariationsMode {
    Mode_Variations = 0,
    Mode_Similar,
    Mode_All
};

class U2CORE_EXPORT U2VariationUtils {
public:
    //track manipulations
    static void addVariationsToTrack(const U2EntityRef &ref, const QList<U2Variant> &variants,
        U2OpStatus &os);
    static U2VariantTrack createVariantTrack(const U2DbiRef &dbiRef, const QString &seqName,
        U2OpStatus &os);
    //convertors
    static AnnotationData variantToAnnotation(const U2Variant &var);
    static U2Feature variantToFeature(const U2Variant &var);
    //variations revealing
    static QList<U2Variant> getSNPFromSequences(const QByteArray &refSeq, const QByteArray &varSeq,
        CallVariationsMode mode = Mode_Variations, bool ignoreGaps = false,
        const QString &namePrefix = QString(), int nameStartIdx = 0);
};

} //namespace


#endif //_U2_VARIATION_UTILS_H_
