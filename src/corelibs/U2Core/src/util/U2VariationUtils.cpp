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

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2VariantDbi.h>

#include "U2VariationUtils.h"

namespace U2 {

void U2VariationUtils::addVariationsToTrack(const U2EntityRef& ref, const QList<U2Variant>& variants, U2OpStatus& os) {
    DbiConnection con(ref.dbiRef, os);
    CHECK_OP(os, );

    U2VariantDbi* vdbi = con.dbi->getVariantDbi();
    SAFE_POINT(vdbi != NULL, "Varian DBI is NULL", );

    U2VariantTrack track = vdbi->getVariantTrack(ref.entityId, os);
    CHECK_OP(os, );

    BufferedDbiIterator<U2Variant> bufIter(variants);
    vdbi->addVariantsToTrack(track, &bufIter, os);
}

U2VariantTrack U2VariationUtils::createVariantTrack(const U2DbiRef &dbiRef, const QString& seqName, U2OpStatus& os) {
    DbiConnection con(dbiRef, os);
    CHECK_OP(os, U2VariantTrack());

    U2VariantDbi* vdbi = con.dbi->getVariantDbi();
    SAFE_POINT(vdbi != NULL, "Varian DBI is NULL", U2VariantTrack());

    U2VariantTrack track;
    track.sequenceName = seqName;
    vdbi->createVariantTrack(track, TrackType_All, U2ObjectDbi::ROOT_FOLDER, os);

    return track;
}

AnnotationData U2VariationUtils::variantToAnnotation(const U2Variant &var) {
    AnnotationData d;

    U2Region varRegion;
    varRegion.startPos = var.startPos;
    varRegion.length = var.endPos - var.startPos + 1;

    d.location->regions << varRegion;
    d.qualifiers << U2Qualifier("public_id", var.publicId);
    d.qualifiers << U2Qualifier("ref_data", var.refData);
    d.qualifiers << U2Qualifier("obs_data", var.obsData);
    d.name = "variation";

    return d;
}

U2Feature U2VariationUtils::variantToFeature(const U2Variant& var) {
    U2Feature res;

    res.id = var.id;
    res.name = "variation";
    res.location.region = U2Region(var.startPos, var.endPos - var.startPos + 1);

    return res;
}

QList<U2Variant> U2VariationUtils::getSNPFromSequences(const QByteArray& refSeq, const QByteArray& varSeq, CallVariationsMode mode, bool ignoreGaps,
    const QString& namePrefix, int nameStartIdx) {
    QList<U2Variant> res;
    qint64 len = qMin(refSeq.size(), varSeq.size());

    for (qint64 i = 0; i < len; i++) {
        char refChar = refSeq.at(i);
        char obsChar = varSeq.at(i);

        bool addVariation = false;

        if (!(ignoreGaps && (refChar == '-' || obsChar == '-'))) {

            switch (mode) {
            case Mode_Variations:
                addVariation = (refChar != obsChar);
                break;
            case Mode_Similar:
                addVariation = (refChar == obsChar);
                break;
            case Mode_All:
                addVariation = true;
            }
        }

        if (addVariation) {
            U2Variant var;
            var.refData = QByteArray(1, refChar);
            var.obsData = QByteArray(1, obsChar);
            var.startPos = i + nameStartIdx;
            var.publicId = (QString(namePrefix + "%1").arg(i + nameStartIdx));
            res.append(var);
        }
    }

    return res;
}

} //namespace
