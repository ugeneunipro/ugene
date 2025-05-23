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

#include "McaReferenceCharController.h"

#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/SequenceObjectContext.h>

#include "McaEditor.h"

namespace U2 {

OffsetRegions::OffsetRegions() {
}

void OffsetRegions::append(const U2Region& region, int offset) {
    regions.append(region);
    offsets.append(offset);
}

int OffsetRegions::findIntersectedRegion(const U2Region& region) const {
    return region.findIntersectedRegion(regions);
}

U2Region OffsetRegions::getRegion(int i) const {
    SAFE_POINT(0 <= i && i < regions.size(), "Index out of range", U2Region());
    return regions[i];
}

int OffsetRegions::getOffset(int i) const {
    SAFE_POINT(0 <= i && i < offsets.size(), "Index out of range", 0);
    return offsets[i];
}

int OffsetRegions::getSize() const {
    SAFE_POINT(regions.size() == offsets.size(), "Invalid state!", 0);
    return regions.size();
}

void OffsetRegions::clear() {
    regions.clear();
    offsets.clear();
}

McaReferenceCharController::McaReferenceCharController(QObject* p, McaEditor* editor)
    : QObject(p),
      refObject(nullptr),
      ungappedLength(-1) {
    SequenceObjectContext* ctx = editor->getReferenceContext();
    SAFE_POINT(ctx != nullptr, "SequenceObjectContext is NULL", );
    refObject = ctx->getSequenceObject();
    SAFE_POINT(ctx != nullptr, "Reference U2SequenceObject is NULL", );
    initRegions();

    connect(refObject, &U2SequenceObject::si_sequenceChanged, this, &McaReferenceCharController::sl_update);
    connect(editor->getMaObject(), &MsaObject::si_alignmentChanged, this, &McaReferenceCharController::sl_update);
}

OffsetRegions McaReferenceCharController::getCharRegions(const U2Region& region) const {
    int i = charRegions.findIntersectedRegion(region);
    CHECK(i != -1, OffsetRegions());

    OffsetRegions result;
    do {
        result.append(region.intersect(charRegions.getRegion(i)),
                      charRegions.getOffset(i));
        if (charRegions.getRegion(i).contains(region.endPos())) {
            return result;
        }
        i++;
    } while (i < charRegions.getSize());
    return result;
}

int McaReferenceCharController::getUngappedPosition(int pos) const {
    CHECK(charRegions.getSize() != 0, -1);

    int ungappedPos = 0;
    int i = 0;
    do {
        if (charRegions.getRegion(i).contains(pos)) {
            ungappedPos += pos - charRegions.getRegion(i).startPos;
            return ungappedPos;
        }
        ungappedPos += charRegions.getRegion(i).length;
        i++;
    } while (i < charRegions.getSize());
    return -1;
}

int McaReferenceCharController::getGappedPos(int pos) const {
    U2OpStatusImpl os;
    const QByteArray data = refObject->getWholeSequenceData(os);
    SAFE_POINT_OP(os, -1);
    int newPos = 0;
    int symbolsCounter = 0;
    for (const QChar& c : qAsConst(data)) {
        const bool notGapChar = c != U2Msa::GAP_CHAR;
        if (symbolsCounter == pos && notGapChar) {
            break;
        }
        if (notGapChar) {
            symbolsCounter++;
        }
        newPos++;
    }
    return newPos;
}

int McaReferenceCharController::getUngappedLength() const {
    return ungappedLength;
}

void McaReferenceCharController::sl_update() {
    initRegions();
}

void McaReferenceCharController::initRegions() {
    charRegions.clear();
    SAFE_POINT(refObject != nullptr, "MCA reference object is NULL", );

    U2OpStatusImpl os;
    QByteArray data = refObject->getWholeSequenceData(os);

    SAFE_POINT_OP(os, );
    U2Region current;
    int gapCounter = 0;
    for (int i = 0; i < data.size(); i++) {
        if (data.at(i) != U2Msa::GAP_CHAR) {
            if (current.isEmpty()) {
                // start new region
                current = U2Region(i, 1);
            } else {
                // extend the current
                current.length++;
            }
        } else {
            if (!current.isEmpty()) {
                // append the region to result.
                charRegions.append(current, gapCounter);
                current = U2Region();
            }
            gapCounter++;
        }
    }
    if (!current.isEmpty()) {
        charRegions.append(current, gapCounter);
    }
    ungappedLength = data.size() - gapCounter;
    emit si_cacheUpdated();
}

}  // namespace U2
